#include "stdafx.h"
#include "PLYLoader.h"

bool PLYLoader::LoadFromPLYFile(Mesh * pkMeshObject, const std::string & kFilename)
{
	std::string kFileSource;

	{
		std::ifstream ifstr(kFilename);
		if (!ifstr.good())
		{
			std::cout << "Unable to open " << kFilename << std::endl;
			return false;
		}

		std::stringstream sstr;
		sstr << ifstr.rdbuf();

		kFileSource = sstr.str();
	}
	std::istringstream iglobalsstr(kFileSource);
	char line[512];


	//read the header
	std::string temp;

	//1- get PLY
	iglobalsstr >> temp;

	if (temp != "ply")
		return false;

	//1- get PLY
	iglobalsstr >> temp;

	if (temp != "format")
		return false;

	iglobalsstr.getline(line, 512);

	//get to the element line
	while (1)
	{
		iglobalsstr >> temp;
		if (temp != "element")
		{
			iglobalsstr.getline(line, 512);
		}
		else
		{
			break;
		}
	}

	//read the number of vertices
	UI32 nbVerts;
	iglobalsstr >> temp >> nbVerts;

	if (temp != "vertex")
		return false;

	bool bHasPos = false;
	bool bHasNormal = false;
	bool bHasUV = false;
	bool bHasColor = false;

	//read the props
	while (1)
	{
		iglobalsstr >> temp;
		if (temp != "property")
		{
			break;
		}
		else
		{
			iglobalsstr >> temp;
			iglobalsstr >> temp;

			char fChar = temp[0];
			if (fChar == 'x' || fChar == 'y' || fChar == 'z')
			{
				bHasPos = true;
			}
			else if (fChar == 'n')
			{ 
				bHasNormal = true;
			}
			else if (fChar == 'r' || fChar == 'g' || fChar == 'b')
			{
				bHasColor = true;
			}
			else if (fChar == 's'|| fChar == 't') 
			{
				bHasUV = true;
			}
		}
	}

	if (temp != "element")
		return false;

	//read the number of vertices
	UI32 nbTris;
	iglobalsstr >> temp >> nbTris;


	//go to the end of the header
	while (1)
	{
		iglobalsstr >> temp;
		if (temp != "end_header")
		{
			iglobalsstr.getline(line, 512);
		}
		else
		{
			break;
		}
	}

	// set the memory
	pkMeshObject->m_akVertices.resize(nbVerts, vec4(1));
	pkMeshObject->m_akNormals.resize(nbVerts, vec3(0,0,-1));
	pkMeshObject->m_akColor.resize(nbVerts, vec4(1));
	pkMeshObject->m_akUVs.resize(nbVerts, vec2(0));

	pkMeshObject->m_aiIndices.resize(nbTris * 3);

	//read the vertex datas
	for (UI32 i = 0; i < nbVerts; ++i)
	{
		if (bHasPos)
		{
			iglobalsstr >> pkMeshObject->m_akVertices[i][0]
				>> pkMeshObject->m_akVertices[i][1]
				>> pkMeshObject->m_akVertices[i][2];
		}

		if (bHasNormal)
		{
			iglobalsstr >> pkMeshObject->m_akNormals[i][0]
				>> pkMeshObject->m_akNormals[i][1]
				>> pkMeshObject->m_akNormals[i][2];
		}

		if (bHasUV)
		{
			iglobalsstr >> pkMeshObject->m_akUVs[i][0]
				>> pkMeshObject->m_akUVs[i][1];
		}

		if (bHasColor)
		{
			float r, g, b;
			iglobalsstr >> r
				>> g
				>> b;

			pkMeshObject->m_akColor[i][0] = r / 255;
			pkMeshObject->m_akColor[i][1] = g / 255;
			pkMeshObject->m_akColor[i][2] = b / 255;
			pkMeshObject->m_akColor[i][3] = 1;
		}
	}
	
	//read the tris datas
	for (UI32 i = 0; i < nbTris; ++i)
	{
		UI32 tempI;

		iglobalsstr >> tempI;
		
		if (tempI == 3)
		{
			iglobalsstr >> pkMeshObject->m_aiIndices[i * 3]
				>> pkMeshObject->m_aiIndices[i * 3 + 1]
				>> pkMeshObject->m_aiIndices[i * 3 + 2];
		}
		else
		{
			std::cout << "wrong sized tris :-) : " << tempI << std::endl;
			return false;
		}

		
	}
	return true;
}
