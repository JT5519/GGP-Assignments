#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <vector>
#include<memory>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "SimpleShader/SimpleShader.h"
#include "Material.h"
#include "Lights.h"
#include "Sky.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void LoadTexturesAndSamplerState();
	void CreateMaterials();
	void CreateMeshesAndEntitites();
	void CreateLights();
	void CreateSkyBox();
	void CreateShadowMapResources();
	void RenderShadowMaps();

	void UpdateImGui(float deltaTime);
	void UpdateStatsUI();
	void UpdateEntityCameraControlUI();
	void UpdateLights();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	//Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	//Custom Shaders
	std::shared_ptr<SimpleVertexShader> shadowVertexShader;
	std::shared_ptr<SimplePixelShader> customPixelShader;

	// Texture and texture-related constructs (how to have a vector of com pointers?)
	//Albedo Map SRVs
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV3;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV4;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV5;
	//Normal Map SRVs
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV3;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV4;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV5;
	//Roughness Map SRVs
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessSRV1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessSRV2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessSRV3;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessSRV4;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessSRV5;
	//Metalness Map SRVs
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalnessSRV1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalnessSRV2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalnessSRV3;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalnessSRV4;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalnessSRV5;
	//Sampler State
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	/*Skybox related*/
	//srv
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV;
	//shaders
	std::shared_ptr<SimpleVertexShader> skyVertexShader;
	std::shared_ptr<SimplePixelShader> skyPixelShader;
	//sky class
	std::shared_ptr<Sky> skyBox;
	/*Skybox related end*/

	//Materials
	std::vector<std::shared_ptr<Material>> materials;

	// Meshes and Entities
	std::vector<std::shared_ptr<Mesh>> gameMeshes;
	std::vector<std::shared_ptr<Entity>> gameEntities;

	//Camera
	std::shared_ptr<Camera> mainCamera;

	//Lights
	std::vector<Light> lights;
	int numOfLightsInGame;

	//Shadows
	int shadowMapResolution;
	float shadowProjectionSize;
	//shadow textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV3;
	//shadow depth stencils
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV1;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV2;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV3;
	//shadow sampler and rasterizer
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	//Shadow view and projection matrices
	DirectX::XMFLOAT4X4 shadowViewMatrix1;
	DirectX::XMFLOAT4X4 shadowViewMatrix2;
	DirectX::XMFLOAT4X4 shadowViewMatrix3;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
};

