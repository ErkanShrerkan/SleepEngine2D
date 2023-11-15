#include "pch.h"
#include "Model.h"

namespace SE
{
    const std::vector<Model::Mesh>& Model::GetMeshes() const
    {
        return myMeshes;
    }
    const std::string& Model::GetPath() const noexcept
    {
        return myPath;
    }
    Model::Skeleton& Model::GetSkeleton()
    {
        return mySkeleton;
    }

    Model::~Model()
    {
    }
}