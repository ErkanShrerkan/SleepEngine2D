#pragma once
#include <string>
#include <CommonUtilities\Container\KeyedPool.hpp>
#include <set>

struct ID3D11VertexShader;
struct ID3D11Buffer;
struct ID3D11InputLayout;
namespace SE
{
    class CSprite;
    class CSpriteFactory
    {
    public:
        ~CSpriteFactory();
        void InitFactory();
        SE::CSprite* GetSprite(const std::string& aSpritePath);
        void ReleaseSprite(CSprite* aSprite);
    private:
        SE::CSprite* CreateSprite(const std::string& aSpritePath);
        std::set<CSprite*> mySprites;
    private:
        struct SVertex
        {
            float x, y, z, w;  // Position
            float r, g, b, a;  // Color
            float u, v;		   // UV Mapping
        };

        bool myHasInited = false;
        ID3D11VertexShader* myVertexShader;
        ID3D11Buffer* myIndexBuffer;
        ID3D11Buffer* myVertexBuffer;
        ID3D11InputLayout* myInputLayout;
    };
}
