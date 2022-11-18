#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "Vertex.h"

class Mesh
{
public:
	Mesh(Vertex* vertices,
		unsigned int verticesNum,
		unsigned int* indices,
		unsigned int indicesNum,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>	context);
	
	Mesh(const wchar_t* filename,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>	context);
	
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	unsigned int GetIndexCount();
	void Draw();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	context;

	unsigned int indexCount;

	void InitMeshAndCreateBuffers(Vertex* vertices,
		unsigned int verticesNum,
		unsigned int* indices,
		unsigned int indicesNum,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>	context);

	void CalculateTangents(Vertex* verts,
		int numVerts,
		unsigned int* indices,
		int numIndices);
};

