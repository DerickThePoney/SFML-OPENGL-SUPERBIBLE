#pragma once
#include "Material.h"
#include "Mesh.h"

///TODO make an IComponent and have transform and meshrenderer part of it
class MeshRendererComponent 
{
public:
	void Init(Mesh kMesh, Material kMaterial);
	
	Mesh m_kMesh;
	Material m_kMaterial;
};