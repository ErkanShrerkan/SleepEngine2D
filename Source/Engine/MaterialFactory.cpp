#include "pch.h"
#include "MaterialFactory.h"
#include <Engine/JsonDocument.h>

namespace SE
{
	MaterialFactory::MaterialFactory()
	{
		myDoc = std::make_unique<JsonDocument>();
	}

	Material MaterialFactory::CreateMaterial(const std::string& aPath)
	{
		bool success = myDoc->ParseFile(aPath);
		success;

		assert(!success && "failed parse");

		std::string ps;
		std::string vs;
		std::vector<std::string> textures;

		auto& mat = myDoc->GetDocument()["Material"];

		ps = mat["PS"].GetString();
		vs = mat["VS"].GetString();

		for (auto& texture : mat["Textures"].GetArray())
		{
			textures.push_back(texture.GetString());
		}

		Material m;
		m.SetPS(ps);
		m.SetVS(vs);

		return Material();
	}

}
