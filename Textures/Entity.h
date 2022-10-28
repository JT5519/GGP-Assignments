#pragma once

#include "Transform.h"
#include "Mesh.h"
#include<memory>
#include "Material.h"
#include "Camera.h"
class Entity
{
public:
	Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
	~Entity();

	//Getters
	std::shared_ptr<Mesh> GetMesh();
	Transform* GetTransform();
	std::shared_ptr<Material> GetMaterial();
	
	//Draw (option 2 for now)
	void Draw(std::shared_ptr<Camera> camera);
private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

