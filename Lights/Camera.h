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
	Transform* GetTransform();
	DirectX::XMFLOAT3 GetAmbientColor();

	//Public Updates
	void Update(float dt);
	void UpdateProjectionMatrix(float aspectRatio);
	void SetAmbientColor(DirectX::XMFLOAT3);

private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT3 cameraAmbientColor;  //keeping ambient color as a property of the specific camera being used. 

	float fov;
	float nearClipDistance;
	float farClipDistance;
	float movementSpeed;
	float mouseLookSpeed;
	bool isOrthographic;

	//Private Updates
	void UpdateViewMatrix();
};

