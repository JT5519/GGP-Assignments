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
	CreateMaterials();
	CreateMeshesAndEntitites();

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
	customPixelShader = std::make_shared<SimplePixelShader>(device, context, FixPath(L"CustomPixelShader.cso").c_str());
}

void Game::CreateMaterials()
{
	//materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f,1.0f), vertexShader, pixelShader)); //red tint
	//materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(0.5f, 1.0f, 0.5f, 1.0f), vertexShader, pixelShader)); //green tint
	//materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), vertexShader, pixelShader)); //blue tint

	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), vertexShader, customPixelShader)); //red tint
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), vertexShader, customPixelShader)); //green tint
	materials.push_back(std::make_shared<Material>(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), vertexShader, pixelShader)); //blue tint
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
			gameMeshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/torus.obj").c_str(), device, context));
			gameMeshes.push_back(std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.obj").c_str(), device, context));
		}
	}

	//Creating Entities
	{
		//Cube
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[0],materials[0]));
		
		//Cylinder
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[1], materials[1]));
		gameEntities[1]->GetTransform()->SetPosition(5.0f,0.0f,0.0f);
		
		//Helix (only one to use a material with the existing pixel shader, rest use materials that use the custom pixel shader)
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[2], materials[2]));
		gameEntities[2]->GetTransform()->SetPosition(10.0f, 0.0f, 0.0f);

		//Torus
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[3], materials[0]));
		gameEntities[3]->GetTransform()->SetPosition(-5.0f, 0.0f, 0.0f);

		//Sphere
		gameEntities.push_back(std::make_shared<Entity>(gameMeshes[4], materials[1]));
		gameEntities[4]->GetTransform()->SetPosition(-10.0f, 0.0f, 0.0f);
	}
}

void Game::UpdateImGui(float deltaTime)
{
	// Get a reference to our custom input manager
	Input& input = Input::GetInstance();
	
	// Reset input manager's gui state so we don�t
	// taint our own input (you�ll uncomment later)
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
	
	// Determine new input capture (you�ll uncomment later)
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

	//draw each entity
	for (std::shared_ptr<Entity> entity : gameEntities)
	{
		entity->Draw(mainCamera);
	}
	
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