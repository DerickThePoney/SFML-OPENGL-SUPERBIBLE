#include "stdafx.h"
#include "MeshRendererComponent.h"

void MeshRendererComponent::Init(Mesh kMesh, Material kMaterial)
{
	m_kMaterial = kMaterial;
	m_kMesh = kMesh;
}

