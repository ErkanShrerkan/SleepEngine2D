#include "pch.h"
#include "RenderCommandManager.h"
#include "RenderCommand.h"

namespace SE
{
	void RenderCommandManager::RegisterCommand(RenderSkinnedMeshCommand newCommand)
	{
		myRenderCommands.push_back(newCommand);
	}

	void RenderCommandManager::Clear()
	{
		myRenderCommands.clear();
	}

}