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
	//prep material for drawing
	material->PrepareMaterialForDraw(&transform, camera);

	//Draw mesh after all shader variables have been set
	mesh->Draw();
}