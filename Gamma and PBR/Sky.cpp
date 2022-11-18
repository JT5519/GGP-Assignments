#include "Sky.h"

Sky::Sky(std::shared_ptr<Mesh> cubeMesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, 
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV,
	std::shared_ptr<SimplePixelShader> skyPS,
	std::shared_ptr<SimpleVertexShader> skyVS,
	Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	skyMesh = cubeMesh;
	sampler = samplerState;
	textureSRV = skySRV;
	skyPixelShader = skyPS;
	skyVertexShader = skyVS;

	//creating rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());

	//creating depth-stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&depthStencilDesc, stencilState.GetAddressOf());
}

Sky::~Sky() {}

void Sky::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera)
{
	context->RSSetState(rasterizerState.Get());
	context->OMSetDepthStencilState(stencilState.Get(), 0);

	skyVertexShader->SetMatrix4x4("view", camera->GetViewMatrix());
	skyVertexShader->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	skyVertexShader->CopyAllBufferData();

	skyPixelShader->SetShaderResourceView("CubeMap", textureSRV);
	skyPixelShader->SetSamplerState("BasicSampler", sampler);
	skyPixelShader->CopyAllBufferData();

	skyVertexShader->SetShader();
	skyPixelShader->SetShader();

	skyMesh->Draw();

	//reset render states
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}
