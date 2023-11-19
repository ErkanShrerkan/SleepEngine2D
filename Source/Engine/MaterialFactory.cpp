#include "pch.h"
#include "MaterialFactory.h"
#include <algorithm>

namespace SE
{
	MaterialFactory::MaterialFactory()
	{
		myDoc = std::make_unique<JsonDocument>();
	}

	Material MaterialFactory::CreateMaterial(const std::string& aPath)
	{
		bool error = myDoc->ParseFile(aPath);

		assert(!error && "failed parse");

		auto& mat = myDoc->GetDocument()["Material"];

		std::string ps = mat["PS"].GetString();
		std::string vs = mat["VS"].GetString();

		Material m;
		m.SetPS(ps);
		m.SetVS(vs);

		for (auto& texture : mat["Textures"].GetArray())
		{
			m.AddTexture(texture.GetString());
		}

		return m;
	}

}
