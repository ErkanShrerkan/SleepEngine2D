#pragma once
#include "Material.h"

class JsonDocument;
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
