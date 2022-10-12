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
	void CreateMaterials();
	void CreateMeshesAndEntitites();

	void UpdateImGui(float deltaTime);
	void UpdateStatsUI();
	void UpdateEntityCameraControlUI();
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
	std::shared_ptr<SimplePixelShader> customPixelShader;

	//Materials
	std::vector<std::shared_ptr<Material>> materials;

	// Meshes and Entities
	std::vector<std::shared_ptr<Mesh>> gameMeshes;
	std::vector<std::shared_ptr<Entity>> gameEntities;

	//Camera
	std::shared_ptr<Camera> mainCamera;
};

