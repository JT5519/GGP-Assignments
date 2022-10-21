#include "ShaderIncludes.hlsli"
#include "ShaderHelpers.hlsli"

//constant buffer definition
cbuffer ExternalData : register(b0)
{
    float3 surfaceColor;
    float roughness;
    float3 cameraPosition;
    float3 ambientTerm;
    Light lights[5];
}

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    
    //ambient term
    float3 finalColor = ambientTerm * surfaceColor;
    
    //directional and point terms
    for (int i = 0; i < 5;i++)
    {
        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            finalColor += Directional(lights[i], input.normal, cameraPosition, input.worldPosition, roughness, surfaceColor);
        }
        else if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            finalColor += Point(lights[i], input.normal, cameraPosition, input.worldPosition, roughness, surfaceColor);
        }
        else if (lights[i].Type == LIGHT_TYPE_SPOT)
        {
            //do nothing for now
        }
    }
    
    return float4(finalColor,1.0f);
}