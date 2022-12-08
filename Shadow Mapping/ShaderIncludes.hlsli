#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier!
#define __GGP_SHADER_INCLUDES__

struct VertexShaderInput
{ 
	float3 localPosition	: POSITION;     // XYZ position
	float3 normal			: NORMAL;		//Normal
    float3 tangent          : TANGENT;      //Tanget
	float2 uv               : TEXCOORD;		//UV
};


struct VertexToPixel
{
	float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
	float2 uv : TEXCOORD; // UV
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 worldPosition : POSITION;
    float4 posForShadow[3] : SHADOWPOS;
};

struct VertexToPixel_Sky
{
    float4 screenPosition : SV_POSITION; // XYZW position (System Value Position)
    float3 sampleDir : DIRECTION;
};

struct VertexToPixel_Shadow
{
    float4 screenPosition : SV_POSITION;
};

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

#define MAX_SPECULAR_EXPONENT 256.0f
#define MAX_LIGHTS 5

struct Light
{
    int Type; 
    float3 Direction; 
    float Range; 
    float3 Position; 
    float Intensity; 
    float3 Color; 
    float SpotFalloff;
    int CastsShadows;
    float2 Padding; 
};

#endif