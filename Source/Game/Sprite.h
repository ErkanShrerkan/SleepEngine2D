#pragma once
#include <Engine\Sprite.h>
#include "Component.h"

class Sprite : public Component
{
public:
    Sprite(){}
    Sprite(const std::string& aTexturePath);
    virtual void Start() override;
    virtual void Reload() override;
    ~Sprite();

public:
    void DrawRect();
    void Render();
    Sprite& SetPosition(const Vector2f& aPosition);
    Sprite& SetSize(const Vector2f& aSize);
    Sprite& SetSizeRelativeToScreen(const Vector2f& aSize);
    Sprite& SetSizeRelativeToImage(const Vector2f& aSize);
    Sprite& SetSizeRelativeToAnotherImage(const float2& aSize, Sprite& aSprite);
    Sprite& SetSizeRelativeToAnotherImage(const float2& aSize, Sprite* aSprite);
    Sprite& SetWidthSizePreservedImageRatio(const float& aSize);
    Sprite& SetHeightSizePreservedImageRatio(const float& aSize);
    Sprite& SetColor(const Vector4f& aColor);
    Sprite& SetPivot(const Vector2f& aPivot);
    Sprite& SetRotation(const float& aRotation);
    Sprite& SetMask(const std::string& aFilePath);
    Sprite& SetRect(const float4 aRect);
    Sprite& SetIsScreenSpace(bool aToggle) { myIsScreenSpace = aToggle; }
    Sprite& SetShaderType(SE::SpriteShaderType aType) { mySprite->SetShaderType(aType); }
    Sprite& SetShaderData(float someData);
    Sprite& SetTexture(const std::string& aTexturePath);
    std::string GetTextureName();
    bool GetSpace() { return myIsScreenSpace; }
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
    ExposableString myTexture;
    float4 myColor = { 1, 1, 1, 1 };
    float4 myRect = { 0, 0, 1, 1 };
    float2 mySize;
    float2 myPosition;
    float2 myPivot;
    float myRotation = 0;
    bool myIsScreenSpace = false;
    bool myRender = true;
    bool myDrawRect = false;

private:
    SE::CSprite* mySprite;
};

