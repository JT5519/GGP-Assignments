#include "Transform.h"

using namespace DirectX;

Transform::Transform()
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
	
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);

	isWorldMatrixDirty = true; 
}

Transform::~Transform() {}

void Transform::SetPosition(float x, float y, float z)
{
	isWorldMatrixDirty = true;
	position = XMFLOAT3(x, y, z);
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
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

void Transform::MoveAbsolute(float x, float y, float z)
{
	isWorldMatrixDirty = true;
	XMVECTOR pos = XMLoadFloat3(&position);
	pos = XMVectorAdd(pos, XMVectorSet(x, y, z, 0.0f));
	XMStoreFloat3(&position, pos);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
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
