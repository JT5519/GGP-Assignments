#pragma once

#include "Transform.h"
#include <DirectXMath.h>

class Camera
{	
public:
	Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, DirectX::XMFLOAT3 initialRotation, float fov, float nearClipDistance, float farClipDistance, float moveSpeed, float mouseLookSpeed, bool isOrthographic);
	~Camera();

	//Getters
	DirectX::XMFLOAT4X4	GetViewMatrix();
	DirectX::XMFLOAT4X4	GetProjectionMatrix();

	//Public Updates
	void Update(float dt);
	void UpdateProjectionMatrix(float aspectRatio);

private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	float fov;
	float nearClipDistance;
	float farClipDistance;
	float movementSpeed;
	float mouseLookSpeed;
	bool isOrthographic;

	//Private Updates
	void UpdateViewMatrix();
};

