#include "pch.h"
#include "LineDrawer.h"
#include "BufferHelper.h"
#include "ShaderHelper.h"
#include "Engine\Engine.h"

namespace SE
{
	std::vector<CLineDrawer::SVertex> CLineDrawer::ourVerts;
	int CLineDrawer::ourVertIndex = 0;
	ID3D11Buffer* CLineDrawer::ourVertBuffer = nullptr;
	ID3D11PixelShader* CLineDrawer::ourPS = nullptr;
	ID3D11VertexShader* CLineDrawer::ourVS = nullptr;
	ID3D11InputLayout* CLineDrawer::ourIL = nullptr;
	std::string CLineDrawer::ourVSData;
	bool CLineDrawer::myIsEnabled = true;

	void CLineDrawer::DeInit()
	{
		ourVertBuffer->Release();
		ourPS->Release();
		ourVS->Release();
	}

	void CLineDrawer::DrawLine(float3 aStart, float3 anEnd, float4 aCol, bool aIsScreenSpace)
	{
		if (myIsEnabled)
		{
			AddVert({ {aStart, 1}, aCol, (float)aIsScreenSpace });
			AddVert({ {anEnd, 1}, aCol, (float)aIsScreenSpace });
		}
	}

	bool CLineDrawer::Init()
	{
		if (!Helper::ShaderHelper::CreatePixelShader(&ourPS, "Shaders/LinePS"))
		{
			return false;
		}

		if (!Helper::ShaderHelper::CreateVertexShader(&ourVS, "Shaders/LineVS", &ourVSData))
		{
			return false;
		}

		// Start Layout
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SPACE",    0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		HRESULT result = CEngine::GetInstance()->GetDXDevice()->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), ourVSData.data(), ourVSData.size(), &ourIL);
		if (FAILED(result))
		{
			return false;
		}

		return true;
	}

	void CLineDrawer::Render()
	{
		if (!myIsEnabled || ourVertIndex == 0)
			return;

		auto& context = CEngine::GetInstance()->GetDXDeviceContext();
		//context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		//context->IASetInputLayout(ourIL);
		//uint stride = sizeof(SVertex);
		//uint offset = 0;
		//context->IASetVertexBuffers(0, 1, &ourLineBuffer, &stride, &offset);
		//context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0u);
		//
		//context->VSSetShader(ourVS, nullptr, 0u);
		//context->PSSetShader(ourPS, nullptr, 0u);
		//context->DrawInstanced(3, ourLineIndex, 0, 0);

		//D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };
		//ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		//HRESULT result = context->Map(ourVertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		//if (FAILED(result))
		//{
		//	ourVertIndex = 0;
		//	return;
		//}
		//memcpy(bufferData.pData, &ourVerts[0], sizeof(SVertex) * ourVertIndex);
		//context->Unmap(ourVertBuffer, 0);

		if (ourVertBuffer)
		{
			ourVertBuffer->Release();
		}

		ID3D11Device* device = CEngine::GetInstance()->GetDXDevice();

		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.ByteWidth = sizeof(SVertex) * static_cast<uint>(ourVerts.size());
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };
		vertexData.pSysMem = ourVerts.data();

		ID3D11Buffer* vertexBuffer;
		device->CreateBuffer(&bufferDesc, &vertexData, &vertexBuffer);

		ourVertBuffer = vertexBuffer;

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		context->IASetInputLayout(ourIL);

		uint stride = sizeof(SVertex);
		uint offset = 0;
		context->IASetVertexBuffers(0, 1, &ourVertBuffer, &stride, &offset);

		context->VSSetShader(ourVS, nullptr, 0);
		context->PSSetShader(ourPS, nullptr, 0);

		context->Draw(ourVertIndex, 0);
		ourVertIndex = 0;
	}

	void CLineDrawer::Clear()
	{
		ourVertIndex = 0;
	}

	void CLineDrawer::AddVert(SVertex aVert)
	{
		if (ourVertIndex >= ourVerts.size())
		{
			ourVerts.push_back(aVert);
		}
		else
		{
			ourVerts[ourVertIndex] = aVert;
		}
		ourVertIndex++;
	}

}

namespace Debug
{
	void DrawLine2D(float2 aStart, float2 anEnd, float4 aCol, bool aIsScreenSpace)
	{
		SE::CLineDrawer::DrawLine({ aStart.x, aStart.y, .5f }, { anEnd.x, anEnd.y, .5f }, aCol, aIsScreenSpace);
	}

	void DrawCircle(float2 aCenter, float aRadius, bool aIsScreenSpace)
	{
		float2 firstPoint = { 0, 0 };
		float2 lastPoint = { 0, 0 };
		float r = aRadius;
		bool first = true;
		for (float a = 0; a <= 2 * 3.1415927f; a += /*2 * */.31415927f)
		{
			float x = (r * cos(a));
			float y = r * sin(a);

			if (first)
			{
				first = false;
				firstPoint = { x, y };
				lastPoint = firstPoint;
			}
			else
			{
				DrawLine2D({ aCenter.x + lastPoint.x, aCenter.y + lastPoint.y }, { x + aCenter.x, aCenter.y + y }, { 1, 1, 1, 1 }, aIsScreenSpace);
				lastPoint = { x, y };
			}
		}
		DrawLine2D({ aCenter.x + lastPoint.x, aCenter.y + lastPoint.y }, { firstPoint.x + aCenter.x, aCenter.y + firstPoint.y }, { 1, 1, 1, 1 }, aIsScreenSpace);
	}
}