#pragma once

#include <DirectXMath.h>
#include <memory>
#include "SimpleShader/SimpleShader.h"
#include <unordered_map>
#include "Transform.h"
#include "Camera.h"

class Material
{
public:
	Material(DirectX::XMFLOAT3, float, std::shared_ptr<SimpleVertexShader>, std::shared_ptr<SimplePixelShader>);
	~Material();

	//Getters
	DirectX::XMFLOAT3 GetColorTint();
	float GetRoughness();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	
	//Setters
	void SetColorTint(DirectX::XMFLOAT3);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader>);
	void SetPixelShader(std::shared_ptr<SimplePixelShader>);
	void SetRoughness(float);

	void AddTextureSRV(std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>);
	void AddSampler(std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>);

	//Before Draw
	void PrepareMaterialForDraw(Transform*, std::shared_ptr<Camera>);
private:
	DirectX::XMFLOAT3 colorTint;
	float roughness; //obsolete
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
};

