#pragma once
#include "Material.h"
#include <Engine/JsonDocument.h>

namespace SE
{
	class MaterialFactory
	{
	public:
		MaterialFactory();
		Material CreateMaterial(const std::string& aPath);

	private:
		uptr(JsonDocument) myDoc;
	};

}
