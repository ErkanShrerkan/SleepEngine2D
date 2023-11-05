#pragma once

namespace SE
{
	class RenderSkinnedMeshCommand;
	class RenderCommandManager
	{
	public:
		void RegisterCommand(RenderSkinnedMeshCommand newCommand);
		void Clear();

	private:
		std::vector<RenderSkinnedMeshCommand> myRenderCommands;
	};

}
