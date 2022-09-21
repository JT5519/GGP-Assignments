#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
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

void Entity::Draw()
{
	mesh->Draw();
}