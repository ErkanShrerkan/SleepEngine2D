#pragma once

namespace SE
{
	class RenderSkinnedMeshCommand;
	class RenderCommandManager
	{
	public:
		void RegisterCommand(RenderSkinnedMeshCommand newCommand);
		std::vector<RenderSkinnedMeshCommand>& GetSkinnedMeshRenderCommands();
		void Clear();

	private:
		std::vector<RenderSkinnedMeshCommand> myRenderCommands;
	};

}
