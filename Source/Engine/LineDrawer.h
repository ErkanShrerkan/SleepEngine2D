#pragma once
#include <vector>
#include <d3d11.h>


namespace SE
{

	class CLineDrawer
	{
		struct SVertex
		{
			float4 pos;
			float4 col;
			float space;
		};

	public:
		static bool Init();
		static void DeInit();
		static void DrawLine(float3 aStart, float3 anEnd, float4 aCol = {1, 1, 1, 1}, bool aIsScreenSpace = false);
		static void Render();
		static void Clear();
		static void SetEnabled(bool anIsEnabled = true) { myIsEnabled = anIsEnabled; }

	private:
		static void AddVert(SVertex aVert);

	private:
		static std::vector<SVertex> ourVerts;
		static int ourVertIndex;
		static ID3D11Buffer* ourVertBuffer;
		static ID3D11PixelShader* ourPS;
		static ID3D11VertexShader* ourVS;
		static ID3D11InputLayout* ourIL;
		static std::string ourVSData;
		static bool myIsEnabled;
	};

}

namespace Debug
{
	void DrawLine2D(float2 aStart, float2 anEnd, float4 aCol = { 1, 1, 1, 1 }, bool aIsScreenSpace = false);
	void DrawCircle(float2 aCenter, float aRadius, bool aIsScreenSpace = false);
}