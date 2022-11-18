#pragma once

#include <wrl/client.h>
#include "Mesh.h"
#include <memory>
#include "SimpleShader/SimpleShader.h"
#include "Camera.h"

class Sky
{
public:
	Sky(std::shared_ptr<Mesh> cubeMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState,
	  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV,
		std::shared_ptr<SimplePixelShader> skyPS,
		std::shared_ptr<SimpleVertexShader> skyVS,
		Microsoft::WRL::ComPtr<ID3D11Device> device);
	
	~Sky();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);


private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> stencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	
	std::shared_ptr<Mesh> skyMesh;
	std::shared_ptr<SimplePixelShader> skyPixelShader;
	std::shared_ptr<SimpleVertexShader> skyVertexShader;
};
