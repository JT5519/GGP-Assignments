#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	this->mesh = mesh;
	this->material = material;
}

Entity::~Entity() {}

std::shared_ptr<Mesh> Entity::GetMesh()
{
	return mesh;
}

Transform* Entity::GetTransform()
{
	return &transform;
}

std::shared_ptr<Material> Entity::GetMaterial()
{
	return material;
}

void Entity::Draw(std::shared_ptr<Camera> camera)
{
	std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
	std::shared_ptr<SimplePixelShader> ps = material->GetPixelShader();

	//Set vertex shader constant buffer data
	{
		vs->SetMatrix4x4("world", transform.GetWorldMatrix());
		vs->SetMatrix4x4("view", camera->GetViewMatrix());
		vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	}
	vs->CopyAllBufferData();

	//Set pixel shader constant buffer data
	{
		ps->SetFloat4("colorTint", material->GetColorTint());
	}
	ps->CopyAllBufferData();

	//Set active shaders as the shaders of the material used by this entitty.
	vs->SetShader();
	ps->SetShader();

	mesh->Draw();
}