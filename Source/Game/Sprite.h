#pragma once
#include <Engine\Sprite.h>
#include "Component.h"

class Sprite : public Component
{
public:
    Sprite(){}
    Sprite(const std::string& aTexturePath);
    virtual void Start() override;
    virtual void Update() override;
    virtual void Reload() override;
    ~Sprite();

public:
    void DrawRect();
    void SetPosition(const Vector2f& aPosition);
    void SetSize(const Vector2f& aSize);
    void SetSizeRelativeToScreen(const Vector2f& aSize);
    void SetSizeRelativeToImage(const Vector2f& aSize);
    void SetSizeRelativeToAnotherImage(const float2& aSize, Sprite& aSprite);
    void SetSizeRelativeToAnotherImage(const float2& aSize, Sprite* aSprite);
    void SetColor(const Vector4f& aColor);
    void SetPivot(const Vector2f& aPivot);
    void SetRotation(const float& aRotation);
    void SetMask(const std::string& aFilePath);
    void SetRect(const float4 aRect);
    void Render();
    void SetIsScreenSpace(bool aToggle) { myIsScreenSpace = aToggle; }
    void SetShaderType(SE::SpriteShaderType aType) { mySprite->SetShaderType(aType); }
    void SetShaderData(float someData);
    void SetTexture(const std::string& aTexturePath);
    std::string GetTextureName();
    float GetShaderData() { return mySprite->GetShaderData(); }
    SE::SpriteShaderType GetShaderType() { return mySprite->GetShaderType(); }
    Vector2f& GetPosition();
    Vector2f GetTopLeftPosition();
    Vector2f& GetSize();
    Vector2f& GetPivot();
    Vector4f& GetColor();
    Vector2f GetImageSize();
    Vector2f GetNormalizedImageSize();
    float4 GetRect();
    float GetRotation();
    SE::CSprite* GetSprite() { return mySprite; }

private:
    void Release();

private:
    // not proud of this sprite setup but it's very low priority to fix
    std::string myTexture = "";
    float4 myColor = { 1, 1, 1, 1 };
    float4 myRect = { 0, 0, 1, 1 };
    float2 mySize;
    float2 myPosition;
    float2 myPivot;
    float myRotation = 0;
    bool myIsScreenSpace = false;
    bool myRender = true;

private:
    SE::CSprite* mySprite;
};

