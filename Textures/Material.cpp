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

void Material::AddTextureSRV(std::string textureName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ textureName, srv });
}

void Material::AddSampler(std::string samplerName, Microsoft::WRL::ComPtr<ID3D11SamplerState> ss)
{
	samplers.insert({ samplerName,ss });
}

void Material::PrepareMaterialForDraw(Transform* transform, std::shared_ptr<Camera> camera)
{
	//Set vertex shader constant buffer data
	{
		vertexShader->SetMatrix4x4("world", transform->GetWorldMatrix());
		vertexShader->SetMatrix4x4("worldInvTranspose", transform->GetWorldInverseTransposeMatrix());
		vertexShader->SetMatrix4x4("view", camera->GetViewMatrix());
		vertexShader->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	}
	vertexShader->CopyAllBufferData();

	//Set pixel shader constant buffer data
	{
		pixelShader->SetFloat3("colorTint", colorTint);
		pixelShader->SetFloat("roughness", roughness);
		pixelShader->SetFloat3("cameraPosition", camera->GetTransform()->GetPosition());
		pixelShader->SetFloat3("ambientTerm", camera->GetAmbientColor());
	}
	pixelShader->CopyAllBufferData();

	//set pixel shader texture and sampler data
	for (auto& t : textureSRVs) { pixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { pixelShader->SetSamplerState(s.first.c_str(), s.second); }

	//Set shaders as active
	vertexShader->SetShader();
	pixelShader->SetShader();
}


