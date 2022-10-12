#pragma once

#include <DirectXMath.h>
#include <memory>
#include "SimpleShader/SimpleShader.h"

class Material
{
public:
	Material(DirectX::XMFLOAT4, std::shared_ptr<SimpleVertexShader>, std::shared_ptr<SimplePixelShader>);
	~Material();

	//Getters
	DirectX::XMFLOAT4 GetColorTint();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	
	//Setters
	void SetColorTint(DirectX::XMFLOAT4);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader>);
	void SetPixelShader(std::shared_ptr<SimplePixelShader>);
private:
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
};

