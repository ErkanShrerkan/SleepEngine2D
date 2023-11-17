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

		std::string ps;
		std::string vs;

		auto& mat = myDoc->GetDocument()["Material"];

		ps = mat["PS"].GetString();
		vs = mat["VS"].GetString();

		Material m;

		for (auto& texture : mat["Textures"].GetArray())
		{
			m.AddTexture(texture.GetString());
		}

		m.SetPS(ps);
		m.SetVS(vs);

		return m;
	}

}
