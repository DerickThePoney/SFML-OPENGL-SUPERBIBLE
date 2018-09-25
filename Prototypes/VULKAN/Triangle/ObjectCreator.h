#pragma once

#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/RawVolume.h"
//using namespace PolyVox;

#include "VulkanHelpers.h"

#include "SignedDistanceField.h"

struct MeshData;
//class VulkanBuffer;
class ObjectCreator
{

public:
	MeshData Execute(glm::vec3 minPos, glm::vec3 maxPos, float cellSize);

private:
	void BuildSDFObject();
	void SDF(PolyVox::RawVolume<float>& volData, glm::vec3 minPos, glm::vec3 maxPos, float fRadius, float cellSize);

	TreeSpeciesDNA GenerateTreeSpeciesDNA(unsigned long seed);
	TreeInstanceDNA GenerateTreeInstanceDNA(const TreeSpeciesDNA& species);
	//SignedDistanceField SDFObject;
	SignedDistanceTree SDFObject;

	float halfheight;
	float size;
};