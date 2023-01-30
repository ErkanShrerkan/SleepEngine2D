// Copyright(c) 2022 Björn Ottosson

#include "Structs.hlsli"
#include "Functions.hlsli"

//float3x3 inverse(float3x3 m)
//{
//    // computes the inverse of a matrix m
//    double det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
//                 m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
//                 m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

//    double invdet = 1 / det;

//    float3x3 minv;
//    minv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invdet;
//    minv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invdet;
//    minv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invdet;
//    minv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invdet;
//    minv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invdet;
//    minv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invdet;
//    minv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invdet;
//    minv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invdet;
//    minv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invdet;

//    return minv;
//}

//const float softness_scale = 0.2; // controls softness of RGB clipping
//const float offset = 0.75; // controls how colors desaturate as they brighten. 0 results in that colors never fluoresce, 1 in very saturated colors 
//const float chroma_scale = 1.2; // overall scale of chroma

//const float3x3 rec2020toSrgb = float3x3(
//    1.6603034854, -0.5875701425, -0.0728900602,
//    -0.1243755953, 1.1328344814, -0.0083597372,
//    -0.0181122800, -0.1005836085, 1.1187703262);

//const float3x3 displayP3toSrgb = float3x3(
//    1.2248021163, -0.2249112615, -0.0000475721,
//    -0.0419281049, 1.0420298967, -0.0000026429,
//    -0.0196088092, -0.0786321233, 1.0983153702);

//const float3x3 SrgbToSrgb = float3x3(
//    1.0, 0.0, 0.0,
//    0.0, 1.0, 0.0,
//    0.0, 0.0, 1.0);
    
//const float3x3 sourceColorSpaceToSrgb = float3x3(
//    1.0, 0.0, 0.0,
//    0.0, 1.0, 0.0,
//    0.0, 0.0, 1.0); // change for different input color space

//// Origin: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
//// Using this since it was easy to differentiate, same technique would work for any curve 
//float3 s_curve(float3 x)
//{
//    float a = 2.51f;
//    float b = 0.03f;
//    float c = 2.43f;
//    float d = 0.59f;
//    float e = 0.14f;
//    x = max(x, 0.0);
//    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
//}

//// derivative of s-curve
//float3 d_s_curve(float3 x)
//{
//    float a = 2.51f;
//    float b = 0.03f;
//    float c = 2.43f;
//    float d = 0.59f;
//    float e = 0.14f;
    
//    x = max(x, 0.0);
//    float3 r = (x * (c * x + d) + e);
//    return (a * x * (d * x + 2.0 * e) + b * (e - c * x * x)) / (r * r);
//}

//float3 tonemap_per_channel(float3 c)
//{
//    return s_curve(c);
//}

//float2 findCenterAndPurity(float3 x)
//{
//    // Matrix derived for (c_smooth+s_smooth) to be an approximation of the macadam limit
//    // this makes it some kind of g0-like estimate
//    float3x3 M = float3x3(
//        2.26775149, -1.43293879, 0.1651873,
//        -0.98535505, 2.1260072, -0.14065215,
//        -0.02501605, -0.26349465, 1.2885107);

//    x = mul(x, M);
    
//    float x_min = min(x.r, min(x.g, x.b));
//    float x_max = max(x.r, max(x.g, x.b));
    
//    float c = 0.5 * (x_max + x_min);
//    float s = (x_max - x_min);
    
//    // math trickery to create values close to c and s, but without producing hard edges
//    float3 y = (x - c) / s;
//    float third = float3(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0);
//    float c_smooth = c + dot(y * y * y, third) * s;
//    float s_smooth = sqrt(dot(x - c, x - c) / 2.0);
//    return float2(c_smooth, s_smooth);
//}

//float3 toLms(float3 c)
//{
//    float3x3 rgbToLms = float3x3(
//        0.4122214708, 0.5363325363, 0.0514459929,
//        0.2119034982, 0.6806995451, 0.1073969566,
//        0.0883024619, 0.2817188376, 0.6299787005);

//    float3 lms_ = mul(c, rgbToLms);
//    float third = float3(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0);
//    return sign(lms_) * pow(abs(lms_), third);
//}

//float calculateC(float3 lms)
//{
//    // Most of this could be precomputed
//    // Creating a transform that maps R,G,B in the target gamut to have same distance from grey axis

//    float3 lmsR = toLms(float3(1.0, 0.0, 0.0));
//    float3 lmsG = toLms(float3(0.0, 1.0, 0.0));
//    float3 lmsB = toLms(float3(0.0, 0.0, 1.0));
    
//    float3 uDir = (lmsR - lmsG) / sqrt(2.0);
//    float3 vDir = (lmsR + lmsG - 2.0 * lmsB) / sqrt(6.0);
    
//    float3x3 to_uv = inverse(float3x3(
//    1.0, uDir.x, vDir.x,
//    1.0, uDir.y, vDir.y,
//    1.0, uDir.z, vDir.z
//    ));
    
//    float3 _uv = mul(lms, to_uv);
    
//    return sqrt(_uv.y * _uv.y + _uv.z * _uv.z);
    
//    float a = 1.9779984951f * lms.x - 2.4285922050f * lms.y + 0.4505937099f * lms.z;
//    float b = 0.0259040371f * lms.x + 0.7827717662f * lms.y - 0.8086757660f * lms.z;

//    return sqrt(a * a + b * b);
//}

//float2 calculateMC(float3 c)
//{
//    float3 lms = toLms(c);
    
//    float M = findCenterAndPurity(lms).x;
    
//    return float2(M, calculateC(lms));
//}

//float2 expandShape(float3 rgb, float2 ST)
//{
//    float2 MC = calculateMC(rgb);
//    float2 STnew = float2((MC.x) / MC.y, (1.0 - MC.x) / MC.y);
//    STnew = (STnew + 3.0 * STnew * STnew * MC.y);
    
//    return float2(min(ST.x, STnew.x), min(ST.y, STnew.y));
//}

//float expandScale(float3 rgb, float2 ST, float scale)
//{
//    float2 MC = calculateMC(rgb);
//    float Cnew = (1.0 / ((ST.x / (MC.x)) + (ST.y / (1.0 - MC.x))));

//    return max(MC.y / Cnew, scale);
//}

//float2 approximateShape()
//{
//    float m = -softness_scale * 0.2;
//    float s = 1.0 + (softness_scale * 0.2 + softness_scale * 0.8);
    
//    float2 ST = float2(1000.0, 1000.0);
//    ST = expandShape(m + s * float3(1.0, 0.0, 0.0), ST);
//    ST = expandShape(m + s * float3(1.0, 1.0, 0.0), ST);
//    ST = expandShape(m + s * float3(0.0, 1.0, 0.0), ST);
//    ST = expandShape(m + s * float3(0.0, 1.0, 1.0), ST);
//    ST = expandShape(m + s * float3(0.0, 0.0, 1.0), ST);
//    ST = expandShape(m + s * float3(1.0, 0.0, 1.0), ST);
    
//    float scale = 0.0;
//    scale = expandScale(m + s * float3(1.0, 0.0, 0.0), ST, scale);
//    scale = expandScale(m + s * float3(1.0, 1.0, 0.0), ST, scale);
//    scale = expandScale(m + s * float3(0.0, 1.0, 0.0), ST, scale);
//    scale = expandScale(m + s * float3(0.0, 1.0, 1.0), ST, scale);
//    scale = expandScale(m + s * float3(0.0, 0.0, 1.0), ST, scale);
//    scale = expandScale(m + s * float3(1.0, 0.0, 1.0), ST, scale);
    
//    return ST / scale;
//}

//float3 tonemap_hue_preserving(float3 c)
//{
//    float3x3 toLms = float3x3(
//        0.4122214708, 0.5363325363, 0.0514459929,
//        0.2119034982, 0.6806995451, 0.1073969566,
//        0.0883024619, 0.2817188376, 0.6299787005);
        
//    float3x3 fromLms = float3x3(
//        +4.0767416621f, -3.3077115913, +0.2309699292,
//        -1.2684380046f, +2.6097574011, -0.3413193965,
//        -0.0041960863f, -0.7034186147, +1.7076147010);
        
//    float3 lms_ = mul(c, toLms);
//    float third = float3(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0);
//    float3 lms = sign(lms_) * pow(abs(lms_), third);
    
//    float2 MP = findCenterAndPurity(lms);
    
//    // apply tone curve
    
//    // Approach 1: scale chroma based on derivative of chrome curve
//    //if (true)
//    //{
//        float I = (MP.x + (1.0 - offset) * MP.y);
//        // Remove comment to see what the results are with Oklab L
//        //I = dot(lms, float3(0.2104542553f, 0.7936177850f, - 0.0040720468f));
        
//        lms = lms * I * I;
//        I = I * I * I;
//        float3 dLms = lms - I;

//        float Icurve = s_curve(float3(I, I, I)).x;
//        lms = 1.0f + chroma_scale * dLms * d_s_curve(float3(I, I, I)) / Icurve;
//        I = pow(Icurve, 1.0 / 3.0);

//        lms = lms * I;
//    //}
//   // 
//    // Approach 2: Separate color into a whiteness/blackness part, apply scale to them independendtly
//    //if (false)
//    //{
//    //    lms = chroma_scale * (lms - MP.x) + MP.x;
//    //
//    //    float invBlackness = (MP.x + MP.y);
//    //    float whiteness = (MP.x - MP.y);
//    //    
//    //    float invBlacknessC = pow(s_curve(float3(invBlackness * invBlackness * invBlackness)).x, 1.0 / 3.0);
//    //    float whitenessC = pow(s_curve(float3(whiteness * whiteness * whiteness)).x, 1.0 / 3.0);
//    //    
//    //    lms = (invBlacknessC + whitenessC) / 2.0 + (lms - (invBlackness + whiteness) / 2.0) * (invBlacknessC - whitenessC) / (invBlackness - whiteness);
//    //}
    
    
//    // compress to a smooth approximation of the target gamut
//    { 
//        float M = findCenterAndPurity(lms).x;
//        float2 ST = approximateShape(); // this can be precomputed, only depends on RGB gamut
//        float C_smooth_gamut = (1.0) / ((ST.x / (M)) + (ST.y / (1.0 - M)));
//        float C = calculateC(lms);

//        lms = (lms - M) / sqrt(C * C / C_smooth_gamut / C_smooth_gamut + 1.0) + M;
//    }
    
//    float3 rgb = mul(lms, mul(lms, mul(lms, fromLms)));

//    return rgb;
//}

//float3 softSaturate(float3 x, float3 a)
//{
//    a = clamp(a, 0.0, softness_scale);
//    a = 1.0 + a;
//    x = min(x, a);
//    float3 b = (a - 1.0) * sqrt(a / (2.0 - a));
//    return 1.0 - (sqrt((x - a) * (x - a) + b * b) - b) / (sqrt(a * a + b * b) - b);
//}

//float3 softClipColor(float3 color)
//{
//    // soft clip of rgb values to avoid artifacts of hard clipping
//    // causes hues distortions, but is a smooth mapping
//    // not quite sure this mapping is easy to invert, but should be possible to construct similar ones that do
    
//    float grey = 0.2;
        
//    float3 x = color - grey;

//    float3 xsgn = sign(x);
//    float3 xscale = 0.5 + xsgn * (0.5 - grey);
//    x /= xscale;

//    float maxRGB = max(color.r, max(color.g, color.b));
//    float minRGB = min(color.r, min(color.g, color.b));
 
//    float softness_0 = maxRGB / (1.0 + softness_scale) * softness_scale;
//    float softness_1 = (1.0 - minRGB) / (1.0 + softness_scale) * softness_scale;
    
//    float3 softness = float3(0.5, .5, .5) * (softness_0 + softness_1 + xsgn * (softness_1 - softness_0));

//    return grey + xscale * xsgn * softSaturate(abs(x), softness);
//}

// Origin: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    float3 f = float3(
    clamp((x.x * (a * x.x + b)) / (x.x * (c * x.x + d) + e), 0.0, 1.0),
    clamp((x.y * (a * x.y + b)) / (x.y * (c * x.y + d) + e), 0.0, 1.0),
    clamp((x.z * (a * x.z + b)) / (x.z * (c * x.z + d) + e), 0.0, 1.0));
    return f;
}


float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);
        
    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);
    
    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;

    float3 color = FullscreenTexture.Sample(defaultSampler, input.myUV).rgb;
    color = tonemap_s_gamut3_cine(color);
    //color += mapping;
    //returnValue.myColor.rgb = color;
    //returnValue.myColor.a = 1;
    //
    //float exposure = 2.5;
    //
    //// reinhard tone mapping
    //float3 mapped = float3(1, 1, 1) - exp(mul(-color, exposure));
    ////mapped = color;
    ////color = tonemap_hue_preserving(color);
    ////color = softClipColor(color);
    ////color = tonemap_per_channel(color);
    returnValue.myColor.rgb = color; //pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
    returnValue.myColor.a = 1;
    return returnValue;
}