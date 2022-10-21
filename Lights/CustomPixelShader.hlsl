#include "ShaderIncludes.hlsli"
#include "ShaderHelpers.hlsli"

//constant buffer definition
cbuffer ExternalData : register(b0)
{
    float4 colorTint;
}

float4 main(VertexToPixel input) : SV_TARGET
{
    float pseudoZ = cross(float3(input.uv.x, 0, 0), float3(0, input.uv.y, 0)).z; 
    return float4(random(input.uv.x), random(input.uv.y), random(pseudoZ), 1)*colorTint; //Behold the noise! (Better without colorTint)
}

