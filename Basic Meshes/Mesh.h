#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "Vertex.h"

class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	context;
	
	unsigned int indexCount;

public:
	Mesh(Vertex* vertices,
		unsigned int verticesNum,
		unsigned int* indices,
		unsigned int indicesNum,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>	context);
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	unsigned int GetIndexCount();
	void Draw();
};

