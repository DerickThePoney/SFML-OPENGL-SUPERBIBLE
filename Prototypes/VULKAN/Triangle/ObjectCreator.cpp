#include "stdafx.h"
#include "ObjectCreator.h"
//#include "VulkanBuffer.h"

MeshData ObjectCreator::Execute(glm::vec3 minPos, glm::vec3 maxPos, float cellSize)
{
	glm::vec3 size = maxPos - minPos;
	float voxelPrecision = .1f;
	PolyVox::RawVolume<float> volData
	(
		PolyVox::Region(PolyVox::Vector3DInt32(0,0,0), 
		PolyVox::Vector3DInt32((int32_t)ceil(size.x / cellSize), (int32_t)ceil(size.y / cellSize), (int32_t)ceil(size.z / cellSize)))
	);
	//PolyVox::RawVolume<float> volData(PolyVox::Region(PolyVox::Vector3DInt32(minPos.x, minPos.y, minPos.z), PolyVox::Vector3DInt32(maxPos.x, maxPos.y, maxPos.z)));
	auto start = std::chrono::high_resolution_clock::now();
	BuildSDFObject();
	auto end1 = std::chrono::high_resolution_clock::now();

	SDF(volData, minPos, maxPos, 5.0f, cellSize);
	auto end2 = std::chrono::high_resolution_clock::now();

	//PolyVox::Mesh<PolyVox::Vertex<float>> mesh;
	PolyVox::DefaultMarchingCubesController<float> ct;
	ct.setThreshold(0);
	PolyVox::Mesh<PolyVox::MarchingCubesVertex<float> > mesh = PolyVox::extractMarchingCubesMesh(&volData, volData.getEnclosingRegion(), ct); // Mesh< SimpleVolume<float> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh, DefaultMarchingCubesController<float>(0));
	auto end3 = std::chrono::high_resolution_clock::now();
	//surfaceExtractor.execute();


	//Convienient access to the vertices and indices
	MeshData ret;
	ret.indices = std::vector<uint32_t>(mesh.getNoOfIndices());
	const uint32_t* indices = mesh.getRawIndexData();
	for (size_t i = 0; i < mesh.getNoOfIndices(); ++i)
	{
		ret.indices[i] = indices[i];
	}

	/*std::vector<glm::vec4> vert(mesh.getNoOfVertices(), glm::vec4(0,0,0,0));

	for (size_t i = 0; i < ret.indices.size(); i+=3)
	{
		glm::vec4 v1(0, 0, 0, 3);// v2(0, 0, 0, 1), v3(0, 0, 0, 1);
		PolyVox::Vertex<float> vmc1 = PolyVox::decodeVertex(mesh.getVertex(ret.indices[i]));
		PolyVox::Vertex<float> vmc2 = PolyVox::decodeVertex(mesh.getVertex(ret.indices[i+1]));
		PolyVox::Vertex<float> vmc3 = PolyVox::decodeVertex(mesh.getVertex(ret.indices[i+2]));
		v1.x = vmc1.position.getX() * cellSize + vmc2.position.getX() * cellSize + vmc3.position.getX() * cellSize;// +minPos.x;
		v1.y = vmc1.position.getX() * cellSize + vmc2.position.getY() * cellSize + vmc3.position.getY() * cellSize;// + minPos.y;
		v1.z = vmc1.position.getX() * cellSize + vmc2.position.getZ() * cellSize + vmc3.position.getZ() * cellSize;// +minPos.z;
		//v1 += minPos;
		vert[ret.indices[i]] += v1;
		vert[ret.indices[i+1]] += v1;
		vert[ret.indices[i+2]] += v1;
	}*/

	auto end4 = std::chrono::high_resolution_clock::now();

	ret.vertices.resize(mesh.getNoOfVertices());
	for (uint32_t i = 0; i < ret.vertices.size(); ++i)
	{
		PolyVox::Vertex<float> vmc1 = PolyVox::decodeVertex(mesh.getVertex(i));
		glm::vec3 v(vmc1.position.getX()* cellSize, vmc1.position.getY()* cellSize, vmc1.position.getZ()* cellSize);
		//glm::vec4 t = vert[i] / vert[i].w;
		//float ti = 0.0f;
		ret.vertices[i].pos = v;// (1 - ti) * v + ti * (glm::vec3(t.x, t.y, t.z));
	}

	auto end5 = std::chrono::high_resolution_clock::now();
	
	for (size_t i = 0; i < ret.indices.size(); i += 3)
	{
		glm::vec3 v1, v2;
		v1 = ret.vertices[ret.indices[i + 1]].pos - ret.vertices[ret.indices[i]].pos;
		v2 = ret.vertices[ret.indices[i + 2]].pos - ret.vertices[ret.indices[i]].pos;
		glm::vec3 n = glm::normalize(
			glm::cross(
				glm::normalize(v1),
				glm::normalize(v2)
			)
		);
		ret.vertices[ret.indices[i]].normal += n;
		ret.vertices[ret.indices[i+1]].normal += n;
		ret.vertices[ret.indices[i+2]].normal += n;
	}
	auto end6 = std::chrono::high_resolution_clock::now();

	for (uint32_t i = 0; i < ret.vertices.size(); ++i)
	{
		ret.vertices[i].pos += minPos;
		ret.vertices[i].normal = glm::normalize(ret.vertices[i].normal);
		ret.vertices[i].color = (ret.vertices[i].pos.y < halfheight) ? glm::vec3(1.0f, 0.0f, 0.0f) : ((ret.vertices[i].pos.y > 2 * halfheight) ? glm::vec3(0.0f, 1.0f, 0.0f) : ((ret.vertices[i].pos.y > (2 * halfheight + this->size)) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 1.0f, 1.0f)));
	}

	auto end7 = std::chrono::high_resolution_clock::now();
	/*for (uint32_t i = 0; i < mesh.getNoOfVertices(); ++i)
	{
		PolyVox::Vertex<float> vmc = PolyVox::decodeVertex(mesh.getVertex(i));
		VertexData v;
		v.pos = vert[i];
		/*v.pos.x = vmc.position.getX() * cellSize;
		v.pos.y = vmc.position.getY()* cellSize;
		v.pos.z = vmc.position.getZ()* cellSize;
		v.pos += minPos;

		
		v.color = (v.pos.y < halfheight) ? glm::vec3(1.0f, 0.0f, 0.0f) : ((v.pos.y > 2*halfheight) ? glm::vec3( 0.0f, 1.0f, 0.0f) : ((v.pos.y > (2 * halfheight + this->size)) ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 1.0f, 1.0f)));

		v.normal.x = vmc.normal.getX();
		v.normal.y = vmc.normal.getY();
		v.normal.z = vmc.normal.getZ();
		//v.normal *= -1;

		ret.vertices.push_back(v);
	}*/

	std::cout << "Computing marching cube mesh : \n";
	std::cout << "\tCreating the sdf : " << (float)(end1 - start).count() / 1000000.0f << "ms\n";
	std::cout << "\tComputing the sdf : " << (float)(end2 - end1).count() / 1000000.0f << "ms\n";
	std::cout << "\tComputing marching cube : " << (float)(end3 - end1).count() / 1000000.0f << "ms\n";
	std::cout << "\tExtracting mesh data (LaplacianSmooth) \n";
	std::cout << "\t\tAveraging vertex: " << (float)(end4 - end3).count() / 1000000.0f << "ms\n";
	std::cout << "\t\tSmoothing vertex: " << (float)(end5 - end4).count() / 1000000.0f << "ms\n";
	std::cout << "\t\tReconstructing normals: " << (float)(end6 - end5).count() / 1000000.0f << "ms\n";
	std::cout << "\t\tFinal data: " << (float)(end7 - end6).count() / 1000000.0f << "ms\n";

	return ret;
}

void ObjectCreator::BuildSDFObject()
{
	/*SignedDistanceCapsule* caps1 = new SignedDistanceCapsule();
	caps1->start = glm::vec3(0.0f);
	caps1->end = glm::vec3(0.0f, 2.0f, 0.0f);
	caps1->size = .3f;
	SDFObject.AddUnionDistance(caps1);

	caps1 = new SignedDistanceCapsule();
	caps1->start = glm::vec3(0.0f, 2.0f, 0.0f);
	caps1->end = glm::vec3(0.5f, 2.2f, 0.0f);
	caps1->size = .1f;
	SDFObject.AddUnionDistance(caps1);*/

	/*SignedDistanceCylinder*  distToTheXAxis = new SignedDistanceCylinder();
	distToTheXAxis->center = glm::vec3(0.0f);
	distToTheXAxis->direction = normalize(glm::vec3(1.0f, 0.0f, 1.0f));
	distToTheXAxis->size = 0.1f;

	SignedDistanceMonster* monster = new SignedDistanceMonster();*/

	TreeSpeciesDNA species = GenerateTreeSpeciesDNA(rand());
	TreeInstanceDNA dna = GenerateTreeInstanceDNA(species);
	halfheight = dna.height / 2;
	this->size = dna.width;
	/*SignedDistanceTree* tree = new SignedDistanceTree(dna);
	

	SDFObject.AddUnionDistance(tree);*/
	SDFObject.SetDNA(dna);

}

void ObjectCreator::SDF(PolyVox::RawVolume<float>& volData, glm::vec3 minPos, glm::vec3 maxPos, float fRadius, float cellSize)
{
	//This vector hold the position of the center of the volume
	PolyVox::Region r = volData.getEnclosingRegion();

	PolyVox::Vector3DInt32 v3dVolCenterI = (r.getLowerCorner() + r.getUpperCorner()) / 2;
	glm::vec3 lowerCorner(r.getLowerCorner().getX(), r.getLowerCorner().getY(), r.getLowerCorner().getZ());

	glm::vec3 v3dVolCenter = glm::vec3(0);// .5f*(maxPos + minPos);
	glm::vec3 box(fRadius, fRadius / 3.0f, fRadius);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				//Store our current position as a vector...
				glm::vec3 v3dCurrentPos = minPos + glm::vec3(x * cellSize, y* cellSize, z* cellSize);
				/*glm::vec3 v3dCurrentPosAbs = glm::abs(v3dCurrentPos);// (abs(v3dCurrentPos.x), abs(v3dCurrentPos.y), abs(v3dCurrentPos.z));

				//Sphere
				float fValueS = length(v3dCurrentPos - v3dVolCenter) - fRadius; 
				
				//Box
				glm::vec3 d = v3dCurrentPosAbs - box;

				float v1 = std::min(std::max(d.x, std::max(d.y, d.z)), 0.0f);
				
				
				d.x = (std::max(d.x, 0.0f));
				d.y = (std::max(d.y, 0.0f));
				d.z = (std::max(d.z, 0.0f));

				float v2 = (float)d.length();

				float fValueB = -(v1 + length(max(d, 0.0f)));
				

				//Capusule
				glm::vec3 a(0, 5, 0);
				glm::vec3 b(0, 14, 0);
				float r = 5.0f;
				glm::vec3 pa = v3dCurrentPos - a, ba = b - a;
				float h = glm::clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
				float fValueCaps = r - length(pa - ba * h);
				*/
				//Union
				float fValueFinal = SDFObject.Evaluate(v3dCurrentPos);

				//Wrte the voxel value into the volume
				volData.setVoxel(x, y, z, -fValueFinal);
			}
		}
	}
}

TreeSpeciesDNA ObjectCreator::GenerateTreeSpeciesDNA(unsigned long seed)
{
	TreeSpeciesDNA species;
	species.seed = 0;

	//Trunk
	//sections
	species.minNumberOfSections = 3;
	species.maxNumberOfSections = 6;

	//height of the trunk
	species.minHeight = 2.0f;
	species.maxHeight = 3.2f;

	//width of the trunk
	species.minWidth = 0.7f;
	species.maxWidth = 0.8f;

	//Top width Scaling
	species.maxTopWidth = 0.7f;
	species.minTopWidth = 0.5f;

	//X&Z noise scaling as function of height
	species.minXYNoiseScale = 1.0f;
	species.maxXYNoiseScale = 3.0f;

	//X Noise
	species.minXOffset = -1000.0f;
	species.maxXOffset = 1000.0f;
	species.minXFrequency = 0.5f;
	species.maxXFrequency = 1.5f;
	species.minXStrength = 0.0f;
	species.maxXStrength = 0.5f;
	species.minXOctave = 2;
	species.maxXOctave = 10;

	//Z Noise
	species.minZOffset = -1000.0f;
	species.maxZOffset = 1000.0f;
	species.minZFrequency = 0.5f;
	species.maxZFrequency = 1.5f;
	species.minZStrength = 0.0f;
	species.maxZStrength = 0.5f;
	species.minZOctave = 2;
	species.maxZOctave = 10;

	//W Noise
	species.minWOffset = -1000.0f;
	species.maxWOffset = 1000.0f;
	species.minWFrequency = 1.0f;
	species.maxWFrequency = 2.0f;
	species.minWStrength = 1.0f;
	species.maxWStrength = 3.0f;
	species.minWOctave = 1;
	species.maxWOctave = 6;


	return species;
}

TreeInstanceDNA ObjectCreator::GenerateTreeInstanceDNA(const TreeSpeciesDNA & species)
{
	TreeInstanceDNA instance;
	instance.dna = &species;

	//init the random number
	srand(time(NULL));
	instance.seed = rand();
	std::mt19937 gen(instance.seed);

	//trunk sections
	std::uniform_int_distribution<unsigned int> distrUIntU;
	instance.trunkSectionsSeed = distrUIntU(gen);

	std::uniform_int_distribution<unsigned int> distrUInt(species.minNumberOfSections, species.maxNumberOfSections);
	instance.numberOfSections = distrUInt(gen);

	//dimensions
	std::uniform_real_distribution<float> distrFloat(species.minHeight, species.maxHeight);
	instance.height = distrFloat(gen);
	distrFloat = std::uniform_real_distribution<float>(species.minWidth, species.maxWidth);
	instance.width = distrFloat(gen);
	distrFloat = std::uniform_real_distribution<float>(species.minTopWidth, species.maxTopWidth);
	instance.topWidth = distrFloat(gen);

	//XYNoiseScale
	distrFloat = std::uniform_real_distribution<float>(species.minXYNoiseScale, species.maxXYNoiseScale);
	instance.XYNoiseScale = distrFloat(gen);

	//X Noise
	distrFloat = std::uniform_real_distribution<float>(species.minXOffset, species.maxXOffset);
	instance.XOffset = distrFloat(gen);
	distrFloat = std::uniform_real_distribution<float>(species.minXFrequency, species.maxXFrequency);
	instance.XFrequency = distrFloat(gen);
	distrFloat = std::uniform_real_distribution<float>(species.minXStrength, species.maxXStrength);
	instance.XStrength = distrFloat(gen);

	instance.XSeed = distrUIntU(gen);
	distrUInt = std::uniform_int_distribution<unsigned int>(species.minXOctave, species.maxXOctave);
	instance.XOctave = distrUInt(gen);

	//Z Noise
	distrFloat = std::uniform_real_distribution<float>(species.minZOffset, species.maxZOffset);
	instance.ZOffset = distrFloat(gen);
	distrFloat = std::uniform_real_distribution<float>(species.minZFrequency, species.maxZFrequency);
	instance.ZFrequency = distrFloat(gen);
	distrFloat = std::uniform_real_distribution<float>(species.minZStrength, species.maxZStrength);
	instance.ZStrength = distrFloat(gen);

	instance.ZSeed = distrUIntU(gen);
	distrUInt = std::uniform_int_distribution<unsigned int>(species.minZOctave, species.maxZOctave);
	instance.ZOctave = distrUInt(gen);

	//W Noise
	distrFloat = std::uniform_real_distribution<float>(species.minWOffset, species.maxWOffset);
	instance.WOffset = distrFloat(gen);
	distrFloat = std::uniform_real_distribution<float>(species.minWFrequency, species.maxWFrequency);
	instance.WFrequency = distrFloat(gen);
	distrFloat = std::uniform_real_distribution<float>(species.minWStrength, species.maxWStrength);
	instance.WStrength = distrFloat(gen);

	instance.WSeed = distrUIntU(gen);
	distrUInt = std::uniform_int_distribution<unsigned int>(species.minWOctave, species.maxWOctave);
	instance.WOctave = distrUInt(gen);

	return instance;
}
