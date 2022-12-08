#include "Camera.h"
#include "Input.h"

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, DirectX::XMFLOAT3 initialRotation, float fov, float nearClipDistance, float farClipDistance, float moveSpeed, float mouseLookSpeed, bool isOrthographic)
{
	transform.SetPosition(initialPosition.x,initialPosition.y,initialPosition.z);

	transform.SetRotation(initialRotation.x, initialRotation.y, initialRotation.z);

	this->fov = fov;
	this->nearClipDistance = nearClipDistance;
	this->farClipDistance = farClipDistance;
	this->movementSpeed = moveSpeed;
	this->mouseLookSpeed = mouseLookSpeed;
	this->isOrthographic = isOrthographic;

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

Transform* Camera::GetTransform()
{
	return &transform;
}

DirectX::XMFLOAT3 Camera::GetAmbientColor()
{
	return cameraAmbientColor;
}

void Camera::SetViewMatrix(DirectX::XMFLOAT4X4 newViewMatrix)
{
	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMLoadFloat4x4(&newViewMatrix));
}

void Camera::SetProjectionMatrix(DirectX::XMFLOAT4X4 newProjectionMatrix)
{
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMLoadFloat4x4(&newProjectionMatrix));
}

void Camera::Update(float dt)
{
	Input& input = Input::GetInstance();
	
	//forward or backward
	if (input.KeyDown('W'))
	{
		transform.MoveRelative(0, 0, movementSpeed * dt);
	}
	else if (input.KeyDown('S'))
	{
		transform.MoveRelative(0, 0, -1 * movementSpeed * dt);
	}

	//right or left
	if (input.KeyDown('D'))
	{
		transform.MoveRelative(movementSpeed * dt, 0, 0);
	}
	else if (input.KeyDown('A'))
	{
		transform.MoveRelative(-1 * movementSpeed * dt, 0, 0);
	}

	//up or down (absolute)
	if (input.KeyDown('Q'))
	{
		transform.MoveAbsolute(0, movementSpeed * dt, 0);
	}
	else if (input.KeyDown('E'))
	{
		transform.MoveAbsolute(0, -1 * movementSpeed * dt, 0);
	}

	if (input.MouseLeftDown())
	{
		int cursorMovementX = input.GetMouseXDelta();
		int cursorMovementY = input.GetMouseYDelta();

		transform.Rotate(cursorMovementY * mouseLookSpeed * dt, 0, 0);
		transform.Rotate(0, cursorMovementX * mouseLookSpeed * dt, 0);
		////clamp x rotation
		//float xRot = transform.GetPitchYawRoll().x;
		//xRot = xRot > DirectX::XM_PIDIV2 ? DirectX::XM_PIDIV2 : (xRot < -1 * DirectX::XM_PIDIV2 ? -1 * DirectX::XM_PIDIV2 : xRot);

		//transform.SetRotation(xRot, transform.GetPitchYawRoll().y, transform.GetPitchYawRoll().z);
	}

	UpdateViewMatrix();
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClipDistance, farClipDistance));
}

void Camera::SetAmbientColor(DirectX::XMFLOAT3 ambientColor)
{
	cameraAmbientColor = ambientColor;
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMFLOAT3 pos = transform.GetPosition();
	DirectX::XMFLOAT3 fwd = transform.GetForward();

	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&fwd);
	DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0, 1, 0, 0);

	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookToLH(position, forward, worldUp));
}


