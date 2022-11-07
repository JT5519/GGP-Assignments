#include "ShaderIncludes.hlsli"
#include "ShaderHelpers.hlsli"

//texture and sampler
Texture2D SurfaceTexture : register(t0); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers 

//Specular texture
Texture2D SpecularTexture : register(t1);

//Normal texture
Texture2D NormalMap : register(t2);

//constant buffer definition
cbuffer ExternalData : register(b0)
{
    float3 colorTint;
    float roughness;
    float3 cameraPosition;
    float3 ambientTerm;
    Light lights[5];
}

float4 main(VertexToPixel input) : SV_TARGET
{
    //Normalize normal and tanget
    input.normal = normalize(input.normal); 
    input.tangent = normalize(input.tangent); 
    
    // Gram-Schmidt orthonormalization
    input.tangent = normalize(input.tangent - input.normal * dot(input.tangent, input.normal)); 
    
    //Calculate bitangent and TBN matrix
    float3 biTangent = cross(input.tangent, input.normal);
    float3x3 TBN = float3x3(input.tangent, biTangent, input.normal);

    //Sampling and unpacking normal map
    float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    //Transforming the unpacked normal
    input.normal = mul(unpackedNormal, TBN);
    
    //Sampling texture for surface color
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb * colorTint;
    
    //Sampling specular texture for specular scaling value
    float specularScalingValue = SpecularTexture.Sample(BasicSampler, input.uv).r;
    
    //ambient term
    float3 finalColor =  surfaceColor * ambientTerm;
    
    //directional and point terms
    for (int i = 0; i < 5;i++)
    {
        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            finalColor += Directional(lights[i], input.normal, cameraPosition, input.worldPosition, roughness, specularScalingValue, surfaceColor);
        }
        else if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            finalColor += Point(lights[i], input.normal, cameraPosition, input.worldPosition, roughness, specularScalingValue, surfaceColor);
        }
        else if (lights[i].Type == LIGHT_TYPE_SPOT)
        {
            //do nothing for now
        }
    }
    
    return float4(finalColor,1.0f);
}