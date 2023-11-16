#include "pch.h"
#include "DeferredRenderer.h"
#include "RenderCommandManager.h"

#include "FrameBufferData.h"
#include "ObjectBufferData.h"

// render command
#include "Model.h"
#include "Material.h"

#include "Engine.h"
#include "DirectX11Framework.h"
#include "DebugProfiler.h"

namespace SE
{
	CDeferredRenderer::CDeferredRenderer()
	{
	}

	CDeferredRenderer::~CDeferredRenderer()
	{
		if (myFrameBuffer)
		{
			myFrameBuffer->Release();
		}

		if (myObjectBuffer)
		{
			myObjectBuffer->Release();
		}
	}

	bool CDeferredRenderer::Init(CDirectX11Framework* aFramework)
	{
		if (!aFramework)
		{
			/* Error Message */
			return false;
		}

		myContext = aFramework->GetContext();
		if (!myContext)
		{
			/* Error Message */
			return false;
		}

		HRESULT result;

		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		ID3D11Device* device = CEngine::GetInstance()->GetDXDevice();

		bufferDesc.ByteWidth = sizeof(FrameBufferData);
		result = device->CreateBuffer(&bufferDesc, nullptr, &myFrameBuffer);
		if (FAILED(result))
		{
			return false;
		}

		bufferDesc.ByteWidth = sizeof(ObjectBufferData);
		result = device->CreateBuffer(&bufferDesc, nullptr, &myObjectBuffer);

		if (FAILED(result))
		{
			return false;
		}

		return true;
	}

	bool CDeferredRenderer::BindDataToBuffer(
		ID3D11Buffer* aBuffer,
		void* aDataPtr,
		uint aDataSize)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE subresource = { 0 };
		ZeroMemory(&subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(aBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
		if (FAILED(result))
		{
			return false;
		}
		memcpy(subresource.pData, aDataPtr, aDataSize);
		myContext->Unmap(aBuffer, 0);
		return true;
	}

	void CDeferredRenderer::GenerateGBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE subresource = { 0 };

		FrameBufferData& fbd = Singleton<FrameBufferData>();

		BindDataToBuffer(myFrameBuffer, &fbd, sizeof(FrameBufferData));

		myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
		myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
		myContext->GSSetConstantBuffers(0, 1, &myFrameBuffer);
		myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		auto& smrc = Singleton<RenderCommandManager>().GetSkinnedMeshRenderCommands();

		for (auto& command : smrc)
		{
			ObjectBufferData obd;

			Model* model = command.GetModel();
			Material& mat = command.GetMaterial();
			float4x4& transform = command.GetTransform();

			obd.myHasBones = 0;

			if (command.IsAnimated())
			{
				obd.myHasBones = 1;
				obd.myNumBones = (unsigned)model->GetSkeleton().myJoints.size();
				auto jointTransforms = command.GetPose();
				memcpy(&obd.myBones[0], jointTransforms.data(), sizeof(Matrix4x4f) * obd.myNumBones);
			}

			obd.myToWorld = transform;

			BindDataToBuffer(myObjectBuffer, &obd, sizeof(ObjectBufferData));

			auto& meshes = model->GetMeshes();

			auto ps = mat.GetPS();
			auto vs = mat.GetVS();
			auto layout = vs.GetInputLayout();
			layout;

			myContext->IASetInputLayout(nullptr);
			myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
			myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
			myContext->VSSetShader(vs.Raw(), nullptr, 0);
			myContext->PSSetShader(ps.Raw(), nullptr, 0);

			int offset = 0;
			for (auto& texture : mat.GetTextures())
			{
				myContext->PSSetShaderResources(8 + offset, 1, texture->GetPointerToShaderResourceView());
				offset++;
			}

			for (auto& mesh : meshes)
			{
				myContext->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
				myContext->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

				myContext->DrawIndexed(mesh.myNumberOfIndices, 0, 0);
				Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
			}
		}

		Singleton<RenderCommandManager>().Clear();
	}
}