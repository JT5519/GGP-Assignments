#include "ShaderIncludes.hlsli"

//constant buffer definition
cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix projection;
}

VertexToPixel_Sky main(VertexShaderInput input)
{
    VertexToPixel_Sky output;
    
    // Create a version of the view matrix without translation
    matrix viewNoTranslation = view;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    // Calculate output position w/ just view & projection
    matrix viewProj = mul(projection, viewNoTranslation);
    output.screenPosition = mul(viewProj, float4(input.localPosition, 1.0f));
    
    //ensure depth is 1
    output.screenPosition.z = output.screenPosition.w;
    
    output.sampleDir = input.localPosition;
    
    return output;

}