#include "pch.h"
#include "ModelFactory.h"

#include "TextureHelper.h"
#include "ShaderHelper.h"

#include "Timer.h"
#include "Engine.h"
#include "Model.h"

#include <memory>
#include <fstream>
#include <d3d11.h>

namespace SE
{
	ModelFactory::ModelFactory()
	{
	}

	ModelFactory::~ModelFactory()
	{
	}

	Model* ModelFactory::GetModel(const std::string& aPath)
	{
#pragma warning(disable:4244)
// '=': conversion from 'int' to 'char', possible loss of data
		std::string lowerCasePath = aPath;
		std::transform(aPath.begin(), aPath.end(), lowerCasePath.begin(), std::tolower);
#pragma warning(default:4244)

		std::replace(lowerCasePath.begin(), lowerCasePath.end(), '\\', '/');

		std::string ext(lowerCasePath.end() - 4, lowerCasePath.end());
		if (ext == ".erc")
		{
			if (myModels.find(lowerCasePath) == myModels.end())
			{
				return LoadERC(lowerCasePath);
			}
			else
			{
				return myModels.at(lowerCasePath);
			}
		}

		assert("model not found or failed to load :/");

		return nullptr;
	}

	Model* ModelFactory::LoadERC(const std::string& aPath)
	{
		CommonUtilities::Timer timer;
		printf("%s\n", aPath.c_str());
		timer.Update();

		std::ifstream t(aPath, std::ios::in | std::ios::binary);
		std::stringstream buffer;
		buffer << t.rdbuf();
		t.close();

		if (buffer.fail())
		{
			return nullptr;
		}

		std::string ercData = buffer.str();

		timer.Update();
		printf("ercData load time: %f\n", timer.GetDeltaTime());

		auto device = CEngine::GetInstance()->GetDXDevice();

		int index = 0;
		int mMeshes = *reinterpret_cast<int*>(&ercData[0]);
		index += sizeof(int);
		float3 minPoint = *reinterpret_cast<float3*>(&ercData[index]);
		index += sizeof(float3);
		float3 maxPoint = *reinterpret_cast<float3*>(&ercData[index]);
		index += sizeof(float3);

		std::vector<Model::Mesh> mMeshV;
		Model::Skeleton skeleton;

		bool hasBones = *reinterpret_cast<bool*>(&ercData[index]);
		index += sizeof(bool);

		if (hasBones)
		{
			int numJoints = *reinterpret_cast<int*>(&ercData[index]);
			index += sizeof(int);
			skeleton.myJoints.resize(numJoints);
			int size = sizeof(Model::Skeleton::Joint) * numJoints;
			memcpy(&skeleton.myJoints[0], &ercData[index], size);
			index += size;
			int numNames = *reinterpret_cast<int*>(&ercData[index]);
			index += sizeof(int);
			for (int i = 0; i < numNames; i++)
			{
				uint name = *reinterpret_cast<int*>(&ercData[index]);
				index += sizeof(uint);
				uint jointIndex = *reinterpret_cast<int*>(&ercData[index]);
				index += sizeof(uint);
				skeleton.myJointNameToIndex[name] = jointIndex;
			}
		}
		timer.Update();
		printf("anim data load time: %f\n", timer.GetDeltaTime());

		if (aPath == "models/ch_player/playerarms.erc")
		{
			int i = 0;
			i;
		}

		for (size_t i = 0; i < mMeshes; i++)
		{
			HRESULT result;
			Model::Mesh mesh;
			std::vector<Vertex> verts;
			std::vector<int> inds;

			int mNumVerts = *reinterpret_cast<int*>(&ercData[index]);
			index += sizeof(int);
			verts.resize(mNumVerts);
			int size = sizeof(Vertex) * mNumVerts;
			memcpy(&verts[0], &ercData[index], size);
			index += size;
			int mNumInds = *reinterpret_cast<int*>(&ercData[index]);
			index += sizeof(int);
			inds.resize(mNumInds);
			size = sizeof(int) * mNumInds;
			memcpy(&inds[0], &ercData[index], size);
			index += size;
			char mTexPath[128];
			size = sizeof(char) * 128;
			memcpy(&mTexPath[0], &ercData[index], size);
			index += size;

			D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
			vertexBufferDescription.ByteWidth = sizeof(Vertex) * static_cast<UINT>(mNumVerts);
			vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
			vertexSubresourceData.pSysMem = verts.data();

			ID3D11Buffer* vertexBuffer;
			result = device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
			if (FAILED(result)) { return nullptr; }

			D3D11_BUFFER_DESC indexBufferDescription = { 0 };
			indexBufferDescription.ByteWidth = sizeof(UINT) * static_cast<UINT>(mNumInds);
			indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
			indexSubresourceData.pSysMem = inds.data();

			ID3D11Buffer* indexBuffer;
			result = device->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
			if (FAILED(result)) { return nullptr; }

			mesh.myNumberOfVertices = static_cast<uint>(mNumVerts);
			mesh.myNumberOfIndices = static_cast<uint>(mNumInds);
			mesh.myStride = sizeof(Vertex);
			mesh.myOffset = 0;
			mesh.myVertexBuffer = vertexBuffer;
			mesh.myIndexBuffer = indexBuffer;
			mMeshV.push_back(mesh);
		}
		timer.Update();
		printf("mesh data load time: %f\n", timer.GetDeltaTime());
		printf("total time: %f\n\n", timer.GetTotalTime());
		Model* model = new Model();
		if (hasBones)
		{
			model->mySkeleton = skeleton;
		}
		model->myMeshes = mMeshV;
		model->myMinimumPoint = minPoint;
		model->myMaximumPoint = maxPoint;
		model->myPath = aPath;

		myModels[aPath] = model;

		return model;
	}
}
