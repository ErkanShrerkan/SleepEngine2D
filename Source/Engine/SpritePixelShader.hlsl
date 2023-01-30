#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Sprite main(VertexToPixel_Sprite input)
{
    float4 color = spriteTexture.Sample(pointSampler, input.myUV);
    float mask = maskTexture.Sample(pointSampler, input.myRectUV).r;
    color.rgb = color.rgb * mask;
    color.a *= mask;
    color *= mySpriteColor;

    color.rgb = GammaToLinear(color.rgb);
    
    PixelOutput_Sprite output;
    output.myColor.rgba = color;
    return output;
}
