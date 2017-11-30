#pragma once
#include "Material.h"
#include "Mesh.h"

//TODO make an IComponent and have transform and meshrenderer part of it
class MeshRendererComponent 
{
public:
	MeshRendererComponent() : m_pkMesh(nullptr), m_pkMaterial(nullptr) {}

	void Init(Mesh* kMesh, Material* kMaterial);

	void Inspect();
	
	Mesh* m_pkMesh;
	Material* m_pkMaterial;
};