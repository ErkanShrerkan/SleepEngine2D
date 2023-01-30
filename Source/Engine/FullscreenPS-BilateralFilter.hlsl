#include "Structs.hlsli"
#include "Functions.hlsli"

#define RADIUS     (4)
#define DIAMETER   (2*RADIUS+1)

#define COORDCOEFF (1.41)

#define LUMCOEFF   (8.)
#define GRADCOEFF  (6.)

#define LUMPLANECORRECTION (1.)

#define GAMMA        (2.2)
//#define pow3(float3 x, float y)    (pow( max(x, 0.) , float3(y) ))
#define LUMWEIGHT    (float4(0.2126,0.7152,0.0722,0.3333))
#define GRADIENT_RADIUS (8)

#define GRADIENT_RADIUSf float(GRADIENT_RADIUS)
#define GRADIENT_RADIUSi22f (4./float(GRADIENT_RADIUS*GRADIENT_RADIUS))
#define PI           (3.14159265359)

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
	float2 uv = input.myUV;
    
    float4 thisColor = FullscreenTexture.Sample(defaultSampler, uv);
	float4 thisGradX = 0;
    float4 thisGradY = 0;

	float4 finalColor = 0;

    float4 diffColorToGradient = 0;
    float4 diffGradientX = 0;
    float4 diffGradientY = 0;
    float sum = 0.;
    float coeff;
    float2 pos;
    float4 color = 0.;
    float4 gradX = 0.;
    float4 gradY = 0.;
    
    float GradCoeff2 = GRADCOEFF*GRADCOEFF;
    float LumCoeff2 = LUMCOEFF*LUMCOEFF;
    float CoordCoeff2 = COORDCOEFF*COORDCOEFF/float(RADIUS*RADIUS);
    
	float2 pixel = float2(1.f / PPBD_screenResolution.x, 1.f / PPBD_screenResolution.y);

    for( int i = -RADIUS ; i <= RADIUS ; i++ ){
        for( int j = -RADIUS ; j <= RADIUS ; j++ ){
            
            pos = float2(uv.x + i * pixel.x, uv.y + j * pixel.y);
            color = FullscreenTexture.Sample(defaultSampler, pos);
            //gradX = texture(inputDX,pos);
            //gradY = texture(inputDY,pos);
            
            diffGradientX = 0;// thisGradX - gradX;
            diffGradientY = 0;// thisGradY - gradY;
            
            diffColorToGradient = thisColor - color;
                //- (thisGradX*float(i) + thisGradY*float(j))*GRADIENT_RADIUSi22f*LUMPLANECORRECTION;
            
            coeff = exp( -(
                float(i*i+j*j)*CoordCoeff2
                + dot(diffColorToGradient,diffColorToGradient) * LumCoeff2
                + dot( mul(diffGradientX, diffGradientX) + mul(diffGradientY, diffGradientY), LUMWEIGHT ) * GradCoeff2
                ));
            
            
            if( i == -RADIUS && j == -RADIUS )
			{
            	finalColor = color*coeff;
            } 
			else 
			{
            	finalColor += color*coeff;
            }
            
            sum += coeff;
        }
    }
	finalColor = finalColor/sum;
    
    float4 normGrad = sqrt(mul(thisGradX, thisGradX) + mul(thisGradY, thisGradY)) * GRADIENT_RADIUSi22f;
    
    finalColor = thisColor - 2.*(finalColor - thisColor);
    
	//float gammaInv = 1. / GAMMA;
	//finalColor.r = pow( max(finalColor.r, 0.) , gammaInv);
	//finalColor.g = pow( max(finalColor.g, 0.) , gammaInv);
	//finalColor.b = pow( max(finalColor.b, 0.) , gammaInv);
    //finalColor.a = 0.;

    PixelOutput_Fullscreen returnValue;
	//
	//const int kSize = (MSIZE-1)/2;
	//float kernel[MSIZE];
	//float4 originalColor = FullscreenTexture.Sample(defaultSampler, input.myUV).rgba;
	//float3 color = 0;
	//
	//float Z = 0.0;
	//for (int j = 0; j <= kSize; ++j)
	//{
	//	kernel[kSize+j] = kernel[kSize - j] = normpdf(j, SIGMA);
	//}
//
	//float3 sampleColor;
	//float factor;
	//float bZ = 1.0 / normpdf(0.0, BSIGMA);
	//float2 res = float2(PPBD_resolution.x / 2, PPBD_resolution.y / 2);
	//
	//for (int i=-kSize; i <= kSize; ++i)
	//{
	//	for (int j=-kSize; j <= kSize; ++j)
	//	{
	//		sampleColor = FullscreenTexture.Sample(defaultSampler, (input.myUV + float2(i / res.x, j / res.y))).rgb;
	//		factor = normpdf3(sampleColor - originalColor.rgb, BSIGMA) * bZ * kernel[kSize + j] * kernel[kSize + i];
	//		Z += factor;
	//		color += factor * sampleColor;
	//	}
	//}
	//
	returnValue.myColor = finalColor;
    return returnValue; 
}