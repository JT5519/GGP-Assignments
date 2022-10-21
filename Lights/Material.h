#pragma once

#include <DirectXMath.h>
#include <memory>
#include "SimpleShader/SimpleShader.h"

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
private:
	DirectX::XMFLOAT3 colorTint;
	float roughness;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
};

