#include "ShaderIncludes.hlsli"
#include "ShaderHelpers.hlsli"

//textures
Texture2D AlbedoMap : register(t0); // Albedo texture
Texture2D NormalMap : register(t1); //Normal texture
Texture2D RoughnessMap : register(t2);
Texture2D MetalnessMap : register(t3);

//shadow map textures (fixed 3, ideally would prefer some sort of array for dynamic number of lights and shadows)
Texture2D ShadowMap1 : register(t4);
Texture2D ShadowMap2 : register(t5);
Texture2D ShadowMap3 : register(t6);

//samplers
SamplerState BasicSampler : register(s0); // "s" registers for samplers 
SamplerComparisonState ShadowSampler : register(s1);

//constant buffer definition
cbuffer ExternalData : register(b0)
{
    float3 colorTint;
    float3 cameraPosition;
    float3 ambientTerm;
    Light lights[MAX_LIGHTS];
    int lightCount;
}

float4 main(VertexToPixel input) : SV_TARGET
{
    //Sampling albedo map for surface color
    float3 albedoColor = AlbedoMap.Sample(BasicSampler, input.uv).rgb;
    float3 surfaceColor = pow(albedoColor, 2.2f) * colorTint;
    
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
    
    //Sampling roughness map
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    
    //Sampling metalness map
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    
    // Specular color determination
    // Assume albedo texture is actually holding specular color where metalness == 1
    // Note the use of lerp here - metal is generally 0 or 1, but might be in between
    // because of linear texture sampling, so we lerp the specular color to match
    float3 specularColor = lerp(F0_NON_METAL.rrr, albedoColor.rgb, metalness);
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    //directional and point terms
    for (int i = 0; i < lightCount; i++)
    {
        float shadowAmount = 1.0f;
        
        //first 3 lights are directional lights with shadows
        if(i<3)
        {
            // SHADOW MAPPING --------------------------------
	        // Note: This is only for a SINGLE light!  If you want multiple lights to cast shadows,
	        // you need to do all of this multiple times IN THIS SHADER.
            float2 shadowUV = input.posForShadow[i].xy / input.posForShadow[i].w * 0.5f + 0.5f;
            shadowUV.y = 1.0f - shadowUV.y;

             // Calculate this pixel's depth from the light
            float depthFromLight = input.posForShadow[i].z / input.posForShadow[i].w;
    
             // Sample the shadow map using a comparison sampler, which
	         // will compare the depth from the light and the value in the shadow map
	         // Note: This is applied below, after we calc our DIRECTIONAL LIGHT
            switch (i)
            {
                case 0:
                    shadowAmount = ShadowMap1.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
                    break;
                case 1:
                    shadowAmount = ShadowMap2.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
                    break;
                case 2:
                    shadowAmount = ShadowMap3.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
                    break;
            } 
        }
        
        if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
        {
            float3 directionalColor = Directional(lights[i], input.normal, cameraPosition, input.worldPosition, roughness, metalness, specularColor, surfaceColor);
            finalColor += directionalColor * (lights[i].CastsShadows ? shadowAmount : 1.0f);
        }
        else if (lights[i].Type == LIGHT_TYPE_POINT)
        {
            finalColor += Point(lights[i], input.normal, cameraPosition, input.worldPosition, roughness, metalness, specularColor, surfaceColor);
        }
        else if (lights[i].Type == LIGHT_TYPE_SPOT)
        {
            //do nothing for now
        }
    }
    
    return float4(pow(finalColor, 1.0f / 2.2f), 1.0f);
}