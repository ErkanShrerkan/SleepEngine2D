#pragma once
#include <map>

struct ID3D11InputLayout;
namespace SE
{
	class Model;
	class Material;
	class ModelFactory
	{
	public:
		struct Vertex
		{
			float x, y, z, w;  // Position
			float r, g, b, a;  // Colors
			float u, v;		  // UV Mapping
			// TODO: Make block of 4 ???
			float nx, ny, nz;	  // Normal
			float tx, ty, tz;	  // Tangent
			float bx, by, bz;	  // Binormal

			unsigned ids[4];
			float weights[4];
		};

	public:
		ModelFactory();
		~ModelFactory();

		Model* GetModel(const std::string& aPath);

	private:
		Model* LoadERC(const std::string& aPath);

	private:
		std::map<std::string, Model*> myModels;
	};
}

