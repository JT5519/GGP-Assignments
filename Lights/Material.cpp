#include "Material.h"


Material::Material(DirectX::XMFLOAT3 colorTint, float roughness, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader)
{
	this->colorTint = colorTint;
	this->roughness = roughness;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
}

Material::~Material() {}

DirectX::XMFLOAT3 Material::GetColorTint()
{
	return colorTint;
}

float Material::GetRoughness()
{
	return roughness;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return pixelShader;
}

void Material::SetColorTint(DirectX::XMFLOAT3 colorTint)
{
	this->colorTint = colorTint;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader)
{
	this->vertexShader = vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader)
{
	this->pixelShader = pixelShader;
}

void Material::SetRoughness(float roughness)
{
	this->roughness = roughness;
}


