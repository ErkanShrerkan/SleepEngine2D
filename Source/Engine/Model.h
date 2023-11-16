#pragma once
#include <d3d11.h>
#include <unordered_map>

namespace SE
{
	class Model
	{
		friend class ModelFactory;
	public:
		struct Mesh
		{
			uint myStride;
			uint myOffset;
			uint myNumberOfVertices;
			uint myNumberOfIndices;

			ID3D11Buffer* myVertexBuffer;
			ID3D11Buffer* myIndexBuffer;
		};

		struct Skeleton
		{
			struct Joint
			{
				uint myIndex;
				uint myName;
				float4x4 myBindPoseInverse;
			};
			std::vector<Joint> myJoints;
			std::unordered_map<uint, uint> myJointNameToIndex;
		};

		const std::vector<Mesh>& GetMeshes() const;
		std::vector<Mesh>& GetMeshes() { return myMeshes; }
		const std::string& GetPath() const noexcept;
		Skeleton& GetSkeleton();
		bool HasBones() { return myHasBones; }
		~Model();

	private:
		Skeleton mySkeleton;
		bool myHasBones = false;
		float3 myMinimumPoint;
		float3 myMaximumPoint;
		std::string myPath;
		std::vector<Mesh> myMeshes;

	public:
		inline const float3& GetMinimumPoint() const
		{
			return myMinimumPoint;
		}

		inline const float3& GetMaximumPoint() const
		{
			return myMaximumPoint;
		}
	};
}
