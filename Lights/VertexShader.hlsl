#include "ShaderIncludes.hlsli"
#include "ShaderHelpers.hlsli"

//constant buffer definition
cbuffer ExternalData : register(b0)
{
	matrix world;
    matrix worldInvTranspose;
	matrix view;
	matrix projection;
}

VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	output.uv = input.uv;
	
    output.normal = mul((float3x3) world, input.normal);
    output.normal = mul((float3x3) worldInvTranspose, input.normal);

    output.worldPosition = mul(world, float4(input.localPosition, 1)).xyz;
	
	return output;
}