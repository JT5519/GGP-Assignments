#include "ShaderIncludes.hlsli"
#include "ShaderHelpers.hlsli"

cbuffer externalData : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

VertexToPixel_Shadow main(VertexShaderInput input)
{
    VertexToPixel_Shadow output;
    
    matrix wvp = mul(projection, mul(view, world));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	
    return output;
}