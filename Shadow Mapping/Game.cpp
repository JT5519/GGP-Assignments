#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Helpers.h"
#include "Entity.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_dx11.h"
#include "ImGui/backends/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// DirectX Helper 
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs

	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	LoadTexturesAndSamplerState();
	CreateMaterials();
	CreateMeshesAndEntitites();
	CreateLights();
	CreateSkyBox();
	CreateShadowMapResources();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		//context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		//context->VSSetShader(vertexShader.Get(), 0, 0);
		//context->PSSetShader(pixelShader.Get(), 0, 0);
	}

	//Camera Creation
	{
		mainCamera = std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XM_PI / 3, 0.01f, 1000.0f, 1.0f, 5.0f, false);

		//set ambient color
		mainCamera->SetAmbientColor(DirectX::XMFLOAT3(0.1f, 0.1f, 0.25f));
	}

	// Initialize ImGui itself & platform/renderer backends
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(device.Get(), context.Get());

		// Pick a style (uncomment one of these 3)
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();
	}
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PixelShader.cso").c_str());
	
	shadowVertexShader = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"VertexShader_Shadow.cso").c_str());
	customPixelShader = std::make_shared<SimplePixelShader>(device, context, FixPath(L"CustomPixelShader.cso").c_str());
	
	skyVertexShader = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"VertexShader_Sky.cso").c_str());
	skyPixelShader = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PixelShader_Sky.cso").c_str());
}

void Game::LoadTexturesAndSamplerState()
{
	//albedo textures
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/bronze_albedo.png").c_str(), 0, textureSRV1.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/cobblestone_albedo.png").c_str(), 0, textureSRV2.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/floor_albedo.png").c_str(), 0, textureSRV3.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/paint_albedo.png").c_str(), 0, textureSRV4.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/rough_albedo.png").c_str(), 0, textureSRV5.GetAddressOf());
	//normal maps
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/bronze_normals.png").c_str(), 0, normalSRV1.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/cobblestone_normals.png").c_str(), 0, normalSRV2.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/floor_normals.png").c_str(), 0, normalSRV3.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/paint_normals.png").c_str(), 0, normalSRV4.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/rough_normals.png").c_str(), 0, normalSRV5.GetAddressOf());

	//roughness maps
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/bronze_roughness.png").c_str(), 0, roughnessSRV1.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/cobblestone_roughness.png").c_str(), 0, roughnessSRV2.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/floor_roughness.png").c_str(), 0, roughnessSRV3.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/paint_roughness.png").c_str(), 0, roughnessSRV4.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/rough_roughness.png").c_str(), 0, roughnessSRV5.GetAddressOf());

	//metalness maps
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/bronze_metal.png").c_str(), 0, metalnessSRV1.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/cobblestone_metal.png").c_str(), 0, metalnessSRV2.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/floor_metal.png").c_str(), 0, metalnessSRV3.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/paint_metal.png").c_str(), 0, metalnessSRV4.GetAddressOf());
	DirectX::CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/PBR/rough_metal.png").c_str(), 0, metalnessSRV5.GetAddressOf());

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
}

void Game::CreateMaterials()
{
	//material 1
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f, vertexShader, pixelShader)); 
	materials[0]->AddTextureSRV("AlbedoMap", textureSRV1);
	materials[0]->AddTextureSRV("NormalMap", normalSRV1);
	materials[0]->AddTextureSRV("RoughnessMap", roughnessSRV1);
	materials[0]->AddTextureSRV("MetalnessMap", metalnessSRV1);

	//material 2
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f, vertexShader, pixelShader));
	materials[1]->AddTextureSRV("AlbedoMap", textureSRV2);
	materials[1]->AddTextureSRV("NormalMap", normalSRV2);
	materials[1]->AddTextureSRV("RoughnessMap", roughnessSRV2);
	materials[1]->AddTextureSRV("MetalnessMap", metalnessSRV2);

	//material 3
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f, vertexShader, pixelShader));
	materials[2]->AddTextureSRV("AlbedoMap", textureSRV3);
	materials[2]->AddTextureSRV("NormalMap", normalSRV3);
	materials[1]->AddTextureSRV("RoughnessMap", roughnessSRV3);
	materials[1]->AddTextureSRV("MetalnessMap", metalnessSRV3);

	//material 4
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f, vertexShader, pixelShader));
	materials[3]->AddTextureSRV("AlbedoMap", textureSRV4);
	materials[3]->AddTextureSRV("NormalMap", normalSRV4);
	materials[3]->AddTextureSRV("RoughnessMap", roughnessSRV4);
	materials[3]->AddTextureSRV("MetalnessMap", metalnessSRV4);

	//material 5
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f, vertexShader, pixelShader));
	materials[4]->AddTextureSRV("AlbedoMap", textureSRV5);
	materials[4]->AddTextureSRV("NormalMap", normalSRV5);
	materials[4]->AddTextureSRV("RoughnessMap", roughnessSRV5);
	materials[4]->AddTextureSRV("MetalnessMap", metalnessSRV5);

	//Add sampler states
	materials[0]->AddSampler("BasicSampler", samplerState);
	materials[1]->AddSampler("BasicSampler", samplerState);
	materials[2]->AddSampler("BasicSampler", samplerState);
	materials[3]->AddSampler("BasicSampler", samplerState);
	materials[4]->AddSampler("BasicSampler", samplerState);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateMeshesAndEntitites()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 gold = XMFLOAT4(1.0f, 0.85f, 0.0f, 1.0f);
	XMFLOAT4 black = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	//Creating Meshes
	{	
		//2D meshes
		{
			////Triangle
			//{
			//	// Set up the vertices of the triangle we would like to draw
			//	// - We're going to copy this array, exactly as it exists in CPU memory
			//	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
			//	// - Note: Since we don't have a camera or really any concept of
			//	//    a "3d world" yet, we're simply describing positions within the
			//	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
			//	// - This means (0,0) is at the very center of the screen.
			//	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
			//	//    Screen Coords", which are ways to describe a position without
			//	//    knowing the exact size (in pixels) of the image/window/etc.  
			//	// - Long story short: Resizing the window also resizes the triangle,
			//	//    since we're describing the triangle in terms of the window itself
			//	Vertex vertices[] =
			//	{
			//		{ XMFLOAT3(+0.0f, +0.25f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f)},
			//		{ XMFLOAT3(+0.25f, -0.25f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f)},
			//		{ XMFLOAT3(-0.25f, -0.25f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f)},
			//	};

			//	// Set up indices, which tell us which vertices to use and in which order
			//	// - This is redundant for just 3 vertices, but will be more useful later
			//	// - Indices are technically not required if the vertices are in the buffer 
			//	//    in the correct order and each one will be used exactly once
			//	// - But just to see how it's done...
			//	unsigned int indices[] = { 0, 1, 2 };

			//	gameMeshes.push_back(std::make_shared<Mesh>(vertices, 3, indices, 3, device, context));
			//}

			//// Hexagon
			//{
			//	Vertex vertices[] =
			//	{
			//		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f)},
			//		{ XMFLOAT3(+0.5f, +0.0f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//		{ XMFLOAT3(+0.25f, +0.433f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//		{ XMFLOAT3(-0.25f, +0.433f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//		{ XMFLOAT3(-0.5f, +0.0f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//		{ XMFLOAT3(-0.25f, -0.433f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//		{ XMFLOAT3(+0.25f, -0.433f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//	};

			//	unsigned int indices[] = { 1, 0, 2, 2, 0, 3, 3, 0, 4, 4, 0, 5, 5, 0, 6, 6, 0, 1 };

			//	gameMeshes.push_back(std::make_shared<Mesh>(vertices, 7, indices, 18, device, context));
			//}

			////Square
			//{
			//	Vertex vertices[] =
			//	{
			//		{ XMFLOAT3(+0.5f, +0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//		{ XMFLOAT3(-0.5f, +0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), XMFLOAT3(0.0f,0.0f,-1.0f), XMFLOAT2(0.0f,0.0f) },
			//	};

			//	unsigned int indices[] = { 1, 0, 3, 1, 3, 2 };

			//	gameMeshes.push_back(std::make_shared<Mesh>(vertices, 4, indices, 6, device, context));
			//}
		}

		//3D meshes
		{
			gameMeshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cube.obj").c_str(), device, context));
			gameMeshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cylinder.obj").c_str(), device, context));
			gameMeshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/helix.obj").c_str(), device, context));
			gameMeshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.obj").c_str(), device, context));
			gameMeshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/torus.obj").c_str(), device, context));
			gameMeshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cube.obj").c_str(), device, context));
		}
	}

	//Creating Entities
	{
		//Cube
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[0],materials[0]));
		gameEntities[0]->GetTransform()->SetRotation(DirectX::XM_PIDIV4, DirectX::XM_PIDIV4, 0.0f);
		//Cylinder
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[1], materials[1]));
		gameEntities[1]->GetTransform()->SetPosition(5.0f,0.0f,0.0f);
		
		//Helix 
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[2], materials[2]));
		gameEntities[2]->GetTransform()->SetPosition(10.0f, 0.0f, 0.0f);

		//Torus
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[3], materials[3]));
		gameEntities[3]->GetTransform()->SetPosition(-5.0f, 0.0f, 0.0f);

		//Sphere
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[4], materials[4]));
		gameEntities[4]->GetTransform()->SetPosition(-10.0f, 0.0f, 0.0f);

		//Ground
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[5], materials[2]));
		gameEntities[5]->GetTransform()->SetPosition(0.0f, -3.0f, 0.0f);
		gameEntities[5]->GetTransform()->SetScale(20.0f, 1.0f, 20.0f);
	}
}

void Game::CreateLights()
{
	lights.clear();
	lights.resize(MAX_LIGHTS);
	
	numOfLightsInGame = 0;

	// Light 1 
	{
		Light directionalLight = {};
		directionalLight.Type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight.Direction = DirectX::XMFLOAT3(1.0f,-1.0f,1.0f); //+x,+z direction
		directionalLight.Color = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f); //red light
		directionalLight.Intensity = 5.0f;
		directionalLight.CastsShadows = 1;
		lights[numOfLightsInGame] = directionalLight;
		numOfLightsInGame++;
	}
	
	// Light 2
	{
		Light directionalLight = {};
		directionalLight.Type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight.Direction = DirectX::XMFLOAT3(-1.0f,-1.0f,1.0f); //-x,+z direction
		directionalLight.Color = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f); //green light
		directionalLight.Intensity = 5.0f;
		directionalLight.CastsShadows = 1;
		lights[numOfLightsInGame] = directionalLight;
		numOfLightsInGame++;
	}

	// Light 3
	{
		Light directionalLight = {};
		directionalLight.Type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight.Direction = DirectX::XMFLOAT3(0.0f, -1.0f, -1.0f); //-z direction
		directionalLight.Color = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f); //blue light
		directionalLight.Intensity = 5.0f;
		directionalLight.CastsShadows = 1;
		lights[numOfLightsInGame] = directionalLight;
		numOfLightsInGame++;
	}
}

void Game::CreateSkyBox()
{
	//load sky box cube map
	DirectX::CreateDDSTextureFromFile(device.Get(), FixPath(L"../../Assets/Textures/SunnyCubeMap.dds").c_str(), 0, skySRV.GetAddressOf());

	//create sky class object
	skyBox = std::make_shared<Sky>(gameMeshes[0], samplerState, skySRV, skyPixelShader, skyVertexShader, device);
}

void Game::CreateShadowMapResources()
{
	// Create shadow requirements ------------------------------------------
	shadowMapResolution = 1024;
	shadowProjectionSize = 30.0f;
	
	//Init 1st Shadow Map stuff
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;

		// Create the actual texture that will be the shadow map
		D3D11_TEXTURE2D_DESC shadowDesc = {};
		shadowDesc.Width = shadowMapResolution;
		shadowDesc.Height = shadowMapResolution;
		shadowDesc.ArraySize = 1;
		shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		shadowDesc.CPUAccessFlags = 0;
		shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		shadowDesc.MipLevels = 1;
		shadowDesc.MiscFlags = 0;
		shadowDesc.SampleDesc.Count = 1;
		shadowDesc.SampleDesc.Quality = 0;
		shadowDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

		// Create the depth/stencil
		D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
		shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
		shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		shadowDSDesc.Texture2D.MipSlice = 0;
		device->CreateDepthStencilView(shadowTexture.Get(), &shadowDSDesc, shadowDSV1.GetAddressOf());

		// Create the SRV for the shadow map
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		device->CreateShaderResourceView(shadowTexture.Get(), &srvDesc, shadowSRV1.GetAddressOf());
	}	
	
	//Init 2nd Shadow Map stuff
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;

		// Create the actual texture that will be the shadow map
		D3D11_TEXTURE2D_DESC shadowDesc = {};
		shadowDesc.Width = shadowMapResolution;
		shadowDesc.Height = shadowMapResolution;
		shadowDesc.ArraySize = 1;
		shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		shadowDesc.CPUAccessFlags = 0;
		shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		shadowDesc.MipLevels = 1;
		shadowDesc.MiscFlags = 0;
		shadowDesc.SampleDesc.Count = 1;
		shadowDesc.SampleDesc.Quality = 0;
		shadowDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

		// Create the depth/stencil
		D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
		shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
		shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		shadowDSDesc.Texture2D.MipSlice = 0;
		device->CreateDepthStencilView(shadowTexture.Get(), &shadowDSDesc, shadowDSV2.GetAddressOf());

		// Create the SRV for the shadow map
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		device->CreateShaderResourceView(shadowTexture.Get(), &srvDesc, shadowSRV2.GetAddressOf());
	}	
	
	//Init 3rd Shadow Map stuff
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;

		// Create the actual texture that will be the shadow map
		D3D11_TEXTURE2D_DESC shadowDesc = {};
		shadowDesc.Width = shadowMapResolution;
		shadowDesc.Height = shadowMapResolution;
		shadowDesc.ArraySize = 1;
		shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		shadowDesc.CPUAccessFlags = 0;
		shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		shadowDesc.MipLevels = 1;
		shadowDesc.MiscFlags = 0;
		shadowDesc.SampleDesc.Count = 1;
		shadowDesc.SampleDesc.Quality = 0;
		shadowDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

		// Create the depth/stencil
		D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
		shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
		shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		shadowDSDesc.Texture2D.MipSlice = 0;
		device->CreateDepthStencilView(shadowTexture.Get(), &shadowDSDesc, shadowDSV3.GetAddressOf());

		// Create the SRV for the shadow map
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		device->CreateShaderResourceView(shadowTexture.Get(), &srvDesc, shadowSRV3.GetAddressOf());
	}

	// Create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // COMPARISON filter!
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible positive value storable in the depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	// Create Projection Matrix
	XMMATRIX shProj = XMMatrixOrthographicLH(shadowProjectionSize, shadowProjectionSize, 0.1f, 100.0f);
	XMStoreFloat4x4(&shadowProjectionMatrix, shProj);
}

void Game::RenderShadowMaps()
{
	// Need to create a viewport that matches the shadow map resolution
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = (float)shadowMapResolution;
	viewport.Height = (float)shadowMapResolution;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);
	
	float dScale = -1 * 20.0f;
	
	// Render 1st shadow map
	{
		// Initial pipeline setup - No RTV necessary - Clear shadow map
		context->OMSetRenderTargets(0, 0, shadowDSV1.Get());
		context->ClearDepthStencilView(shadowDSV1.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		context->RSSetState(shadowRasterizer.Get());

		// Update View matrix to light 1
		XMMATRIX shView = XMMatrixLookAtLH(
			XMVectorSet(dScale * lights[0].Direction.x, dScale * lights[0].Direction.y, dScale * lights[0].Direction.z, 0.0f),
			XMVectorSet(0, 0, 0, 0),
			XMVectorSet(0, 1, 0, 0));
		XMStoreFloat4x4(&shadowViewMatrix1, shView);

		// Turn on our shadow map Vertex Shader
		// and turn OFF the pixel shader entirely
		shadowVertexShader->SetShader();
		shadowVertexShader->SetMatrix4x4("view", shadowViewMatrix1);
		shadowVertexShader->SetMatrix4x4("projection", shadowProjectionMatrix);
		context->PSSetShader(0, 0, 0); // No PS

		//create camera to pass to entity to draw. This camera is actually the light.
		std::shared_ptr<Camera> lightCamera = std::make_shared<Camera>(1.0f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XM_PI / 3, 0.01f, 1000.0f, 1.0f, 5.0f, false);

		//set camera to the lights orientation and set projection to shadowMap viewport values
		lightCamera->SetViewMatrix(shadowViewMatrix1);
		lightCamera->SetProjectionMatrix(shadowProjectionMatrix);

		// Loop and draw all entities
		for (auto& e : gameEntities)
		{
			shadowVertexShader->SetMatrix4x4("world", e->GetTransform()->GetWorldMatrix());
			shadowVertexShader->CopyAllBufferData();

			// Draw the mesh
			e->Draw(lightCamera);
		}
	}

	// Render 2nd shadow map
	{
		// Initial pipeline setup - No RTV necessary - Clear shadow map
		context->OMSetRenderTargets(0, 0, shadowDSV2.Get());
		context->ClearDepthStencilView(shadowDSV2.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		context->RSSetState(shadowRasterizer.Get());

		// Update View matrix to light 2
		XMMATRIX shView = XMMatrixLookAtLH(
			XMVectorSet(dScale * lights[1].Direction.x, dScale * lights[1].Direction.y, dScale * lights[1].Direction.z, 0.0f),
			XMVectorSet(0, 0, 0, 0),
			XMVectorSet(0, 1, 0, 0));
		XMStoreFloat4x4(&shadowViewMatrix2, shView);

		// Turn on our shadow map Vertex Shader
		// and turn OFF the pixel shader entirely
		shadowVertexShader->SetShader();
		shadowVertexShader->SetMatrix4x4("view", shadowViewMatrix2);
		shadowVertexShader->SetMatrix4x4("projection", shadowProjectionMatrix);
		context->PSSetShader(0, 0, 0); // No PS

		//create camera to pass to entity to draw. This camera is actually the light.
		std::shared_ptr<Camera> lightCamera = std::make_shared<Camera>(1.0f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XM_PI / 3, 0.01f, 1000.0f, 1.0f, 5.0f, false);

		//set camera to the lights orientation and set projection to shadowMap viewport values
		lightCamera->SetViewMatrix(shadowViewMatrix2);
		lightCamera->SetProjectionMatrix(shadowProjectionMatrix);

		// Loop and draw all entities
		for (auto& e : gameEntities)
		{
			shadowVertexShader->SetMatrix4x4("world", e->GetTransform()->GetWorldMatrix());
			shadowVertexShader->CopyAllBufferData();

			// Draw the mesh
			e->Draw(lightCamera);
		}
	}

	// Render 3rd shadow map
	{
		// Initial pipeline setup - No RTV necessary - Clear shadow map
		context->OMSetRenderTargets(0, 0, shadowDSV3.Get());
		context->ClearDepthStencilView(shadowDSV3.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		context->RSSetState(shadowRasterizer.Get());

		// Update View matrix to light 2
		XMMATRIX shView = XMMatrixLookAtLH(
			XMVectorSet(dScale * lights[2].Direction.x, dScale * lights[2].Direction.y, dScale * lights[2].Direction.z, 0.0f),
			XMVectorSet(0, 0, 0, 0),
			XMVectorSet(0, 1, 0, 0));
		XMStoreFloat4x4(&shadowViewMatrix3, shView);

		// Turn on our shadow map Vertex Shader
		// and turn OFF the pixel shader entirely
		shadowVertexShader->SetShader();
		shadowVertexShader->SetMatrix4x4("view", shadowViewMatrix3);
		shadowVertexShader->SetMatrix4x4("projection", shadowProjectionMatrix);
		context->PSSetShader(0, 0, 0); // No PS

		//create camera to pass to entity to draw. This camera is actually the light.
		std::shared_ptr<Camera> lightCamera = std::make_shared<Camera>(1.0f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XM_PI / 3, 0.01f, 1000.0f, 1.0f, 5.0f, false);

		//set camera to the lights orientation and set projection to shadowMap viewport values
		lightCamera->SetViewMatrix(shadowViewMatrix3);
		lightCamera->SetProjectionMatrix(shadowProjectionMatrix);

		// Loop and draw all entities
		for (auto& e : gameEntities)
		{
			shadowVertexShader->SetMatrix4x4("world", e->GetTransform()->GetWorldMatrix());
			shadowVertexShader->CopyAllBufferData();

			// Draw the mesh
			e->Draw(lightCamera);
		}
	}

	// After rendering the shadow map, go back to the screen
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	viewport.Width = (float)this->windowWidth;
	viewport.Height = (float)this->windowHeight;
	context->RSSetViewports(1, &viewport);
	context->RSSetState(0);
}

void Game::UpdateImGui(float deltaTime)
{
	// Get a reference to our custom input manager
	Input& input = Input::GetInstance();
	
	// Reset input manager's gui state so we don’t
	// taint our own input (you’ll uncomment later)
	input.SetKeyboardCapture(false);
	input.SetMouseCapture(false);
	
	// Feed fresh input data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)this->windowWidth;
	io.DisplaySize.y = (float)this->windowHeight;
	io.KeyCtrl = input.KeyDown(VK_CONTROL);
	io.KeyShift = input.KeyDown(VK_SHIFT);
	io.KeyAlt = input.KeyDown(VK_MENU);
	io.MousePos.x = (float)input.GetMouseX();
	io.MousePos.y = (float)input.GetMouseY();
	io.MouseDown[0] = input.MouseLeftDown();
	io.MouseDown[1] = input.MouseRightDown();
	io.MouseDown[2] = input.MouseMiddleDown();
	io.MouseWheel = input.GetMouseWheel();
	input.GetKeyArray(io.KeysDown, 256);
	
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	// Determine new input capture (you’ll uncomment later)
	input.SetKeyboardCapture(io.WantCaptureKeyboard);
	input.SetMouseCapture(io.WantCaptureMouse);
	
	// Show the demo window
	//ImGui::ShowDemoWindow();
}

void Game::UpdateStatsUI()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Text("Framerate: %f", io.Framerate);
	ImGui::Text("Window Size: %d x %d", windowWidth,windowHeight);

}

void Game::UpdateEntityCameraControlUI()
{
	ImGui::Begin("Entity and Camera Control");

	//Entity Controls
	XMFLOAT3 pos = gameEntities[0]->GetTransform()->GetPosition();

	if (ImGui::DragFloat3("Triangle 1 Position", &pos.x))
	{
		gameEntities[0]->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
	}

	ImGui::End();
}

void Game::UpdateLights()
{
	ImGui::Begin("Light Controls");
	
	//Light 1
	{
		XMFLOAT3 color = lights[0].Color;
		bool shadow = lights[0].CastsShadows;
		if (ImGui::ColorEdit3("Light 1 (1,0,1) Color", &color.x))
		{
			lights[0].Color = color;
		}

		if (ImGui::Checkbox("Light 1 Shadowmap",&shadow), &shadow)
		{
			lights[0].CastsShadows = shadow;
		}
	}	
	
	//Light 2
	{
		XMFLOAT3 color = lights[1].Color;
		bool shadow = lights[1].CastsShadows;
		if (ImGui::ColorEdit3("Light 2 (-1,0,1) Color", &color.x))
		{
			lights[1].Color = color;
		}

		if (ImGui::Checkbox("Light 2 Shadowmap",&shadow), &shadow)
		{
			lights[1].CastsShadows = shadow;
		}
	}
	
	//Light 3
	{
		XMFLOAT3 color = lights[2].Color;
		bool shadow = lights[2].CastsShadows;
		if (ImGui::ColorEdit3("Light 3 (0,0,-1) Color", &color.x))
		{
			lights[2].Color = color;
		}

		if (ImGui::Checkbox("Light 3 Shadowmap",&shadow), &shadow)
		{
			lights[2].CastsShadows = shadow;
		}
	}

	ImGui::End();
}

// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	//update camera's projection matrix
	if (mainCamera)
	{
		mainCamera->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	//Update UI
	UpdateImGui(deltaTime);

	//Display ImGUI windows
	{
		//Update Lights
		UpdateLights();

		////Update Stats UI
		//UpdateStatsUI();

		////Update Entity and Camera Control UI
		//UpdateEntityCameraControlUI();	
	}

	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	//Update entities every frame
	{
		////Triangle 1
		//gameEntities[0]->GetTransform()->MoveAbsolute(sin(totalTime)*deltaTime, 0.0f, 0.0f);

		////Triangle 2
		//gameEntities[1]->GetTransform()->Rotate(0.0f, 0.0f, deltaTime);

		////Hexagon 1 
		//gameEntities[2]->GetTransform()->SetScale(0.5f, 0.5f, 1.0f);
		//gameEntities[2]->GetTransform()->SetPosition(sin(totalTime) * deltaTime, cos(totalTime) * deltaTime, 0.0f);
		//
		////Hexagon 2 
		//gameEntities[3]->GetTransform()->MoveAbsolute(0.0f, sin(totalTime) * deltaTime, 0.0f);
		//
		////Square 
		//gameEntities[4]->GetTransform()->Rotate(0.0f, 0.0f, 5*deltaTime);
	}
	
	//Camera Update
	mainCamera->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		const float bgColor[4] = { 0.4f, 0.6f, 0.75f, 1.0f }; // Cornflower Blue
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// Render the shadow map before rendering anything to the screen
	RenderShadowMaps();

	std::vector <DirectX::XMFLOAT4X4> viewMatrices;
	viewMatrices.push_back(shadowViewMatrix1);
	viewMatrices.push_back(shadowViewMatrix2);
	viewMatrices.push_back(shadowViewMatrix3);

	//draw each entity
	for (std::shared_ptr<Entity> entity : gameEntities)
	{
		std::shared_ptr<SimpleVertexShader> vs = entity->GetMaterial()->GetVertexShader();
		std::shared_ptr<SimplePixelShader> ps = entity->GetMaterial()->GetPixelShader();
		
		//set shadow info for vertex shader
		vs->SetData("shadowView", &viewMatrices[0],sizeof(DirectX::XMFLOAT4X4)*viewMatrices.size());
		vs->SetMatrix4x4("shadowProjection", shadowProjectionMatrix);

		//set lights for pixel shader
		ps->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
		ps->SetInt("lightCount", numOfLightsInGame);
		ps->SetShaderResourceView("ShadowMap1", shadowSRV1);
		ps->SetShaderResourceView("ShadowMap2", shadowSRV2);
		ps->SetShaderResourceView("ShadowMap3", shadowSRV3);
		ps->SetSamplerState("ShadowSampler", shadowSampler);

		//draw entity
		entity->Draw(mainCamera);
	}
	
	//draw skybox
	skyBox->Draw(context, mainCamera);

	// Draw ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		swapChain->Present(vsync ? 1 : 0, 0);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	}
}