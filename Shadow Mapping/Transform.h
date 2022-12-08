#pragma once

#include <DirectXMath.h>
class Transform
{
public:
	Transform();
	~Transform();
	
	//Setters
	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	//Getters
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();

	//Transformers
	void MoveAbsolute(float x, float y, float z);
	void MoveRelative(float x, float y, float z); //relative to orientation of transform
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);

private:
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 forward;

	bool isWorldMatrixDirty; //true if matrix needs to be updated.
	bool isRotated; //true if only rotation has been updated.

	void UpdateMatrices();
	void UpdateOrientation();
};

