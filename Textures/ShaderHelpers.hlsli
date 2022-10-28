#ifndef __GGP_SHADER_HELPERS__ 
#define __GGP_SHADER_HELPERS__

#include "ShaderIncludes.hlsli"

float random(float2 s)
{
    return frac(atan(dot(s, float2(27.9898, 32.233))) * 39198.5453123);
}

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float Diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

float Specular(float3 dirToView, float3 dirReflected, float roughness, float specularScalingValue)
{
    float specExponent = clamp(1.0f - roughness, 0.05f, 1.0f) * MAX_SPECULAR_EXPONENT; //clamp so that for perfect matte materials, exp is not 0
    return pow(saturate(dot(dirToView, dirReflected)) * specularScalingValue, specExponent);
}

float3 Directional(Light light, float3 normal, float3 viewPosition, float3 worldPosition, float roughness, float specularScalingValue, float3 surfaceColor)
{
    //diffuse
    float3 dirToLight = normalize(-light.Direction);
    float diffuse = Diffuse(normal, dirToLight);   
    
    //specular
    float3 dirToView = normalize(viewPosition - worldPosition);
    float3 dirOfReflection = reflect(light.Direction, normal);
    float specular = Specular(dirToView, dirOfReflection, roughness, specularScalingValue);
    
    //diffuse + specular
    return (diffuse * surfaceColor + specular) * light.Color * light.Intensity;
}

float3 Point(Light light, float3 normal, float3 viewPosition, float3 worldPosition, float roughness, float specularScalingValue, float3 surfaceColor)
{
    float3 dirToLight = normalize(light.Position - worldPosition);
    
    //diffuse
    float diffuse = Diffuse(normal, dirToLight);
    
    //specular
    float3 dirToView = normalize(viewPosition - worldPosition);
    float3 dirOfReflection = reflect(-dirToLight, normal);
    float specular = Specular(dirToView, dirOfReflection, roughness, specularScalingValue);
    
    //attenuation
    float attenuation = Attenuate(light, worldPosition);
    
    //attenuated diffuse + specular
    return (diffuse * surfaceColor + specular) * light.Color * light.Intensity * attenuation;
}
#endif