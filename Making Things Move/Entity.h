#pragma once

#include "Transform.h"
#include "Mesh.h"
#include<memory>

class Entity
{
public:
	Entity(std::shared_ptr<Mesh> mesh);
	~Entity();

	//Getters
	std::shared_ptr<Mesh> GetMesh();
	Transform* GetTransform();
	//Draw (option 2 for now)
	void Draw();
private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
};

