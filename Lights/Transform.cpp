#include "Transform.h"

using namespace DirectX;

Transform::Transform()
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
	
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);

	right = XMFLOAT3(1, 0, 0);
	up = XMFLOAT3(0, 1, 0);
	forward = XMFLOAT3(0, 0, 1);

	isWorldMatrixDirty = false; 
	isRotated = false;
}

Transform::~Transform() {}

void Transform::SetPosition(float x, float y, float z)
{
	isWorldMatrixDirty = true;
	position = XMFLOAT3(x, y, z);
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	isRotated = true;
	isWorldMatrixDirty = true;
	rotation = XMFLOAT3(pitch, yaw, roll);
}

void Transform::SetScale(float x, float y, float z)
{
	isWorldMatrixDirty = true;
	scale = XMFLOAT3(x, y, z);
}

XMFLOAT3 Transform::GetPosition()
{
	return position;
}

XMFLOAT3 Transform::GetPitchYawRoll()
{
	return rotation;
}

XMFLOAT3 Transform::GetScale()
{
	return scale;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (isWorldMatrixDirty)
	{
		UpdateMatrices();
	}

	return worldMatrix;
}

XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	if (isWorldMatrixDirty)
	{
		UpdateMatrices();
	}

	return worldInverseTransposeMatrix;
}

XMFLOAT3 Transform::GetRight()
{
	if (isRotated)
	{
		UpdateOrientation();
	}
	return right;
}

XMFLOAT3 Transform::GetUp()
{
	if (isRotated)
	{
		UpdateOrientation();
	}
	return up;
}

XMFLOAT3 Transform::GetForward()
{
	if (isRotated)
	{
		UpdateOrientation();
	}
	return forward;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	isWorldMatrixDirty = true;
	XMVECTOR pos = XMLoadFloat3(&position);
	pos = XMVectorAdd(pos, XMVectorSet(x, y, z, 0.0f));
	XMStoreFloat3(&position, pos);
}

void Transform::MoveRelative(float x, float y, float z)
{
	//convert Euler Angles to Quaternion
	XMVECTOR rot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));

	//relative movement to add to position
	XMVECTOR relVec = XMVector3Rotate(XMVectorSet(x, y, z, 1), rot);
	
	//updating position
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), relVec));
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	isRotated = true;
	isWorldMatrixDirty = true;
	XMVECTOR rot = XMLoadFloat3(&rotation);
	rot = XMVectorAdd(rot, XMVectorSet(pitch, yaw, roll, 0.0f));
	XMStoreFloat3(&rotation, rot);
}

void Transform::Scale(float x, float y, float z)
{
	isWorldMatrixDirty = true;
	XMVECTOR sc = XMLoadFloat3(&scale);
	sc = XMVectorMultiply(sc, XMVectorSet(x, y, z, 0.0f));
	XMStoreFloat3(&scale, sc);
}

void Transform::UpdateMatrices()
{
	XMMATRIX transMat = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
	XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));
	XMMATRIX scaleMat = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

	XMMATRIX world = scaleMat * rotMat * transMat;

	XMStoreFloat4x4(&worldMatrix, world);
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
	
	isWorldMatrixDirty = false;
}

void Transform::UpdateOrientation()
{
	//convert Euler Angles to Quaternion
	XMVECTOR rot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));

	//Update orientation vectors
	XMStoreFloat3(&right, XMVector3Rotate(XMVectorSet(1, 0, 0,0), rot));
	XMStoreFloat3(&up, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rot));
	XMStoreFloat3(&forward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rot));

	isRotated = false;
}
