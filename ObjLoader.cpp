#include "stdafx.h"
#include "ObjLoader.h"

bool ObjLoader::LoadFromObjFile(Mesh* pkMeshObject, const std::string & kFilename)
{
	//open the file and load the source in memory
	//std::stringstream sstr;
	//std::istringstream iglobalsstr;
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
	std::istringstream icountsstr(kFileSource);
	std::istringstream iglobalsstr(kFileSource);
	char line[512];
	
	//first count the number of vs, vts, vns, and fs
	UI32 vnb = 0, vtnb = 0, vnnb = 0, fnb = 0;

	while (!icountsstr.eof())
	{
		icountsstr.getline(line, 512);
		if (line[0] == 'v')
		{
			switch (line[1])
			{
			case ' ': vnb++; break;
			case 'n': vnnb++; break;
			case 't': vtnb++; break;
			}
		}
		else if (line[0] == 'f')
		{
			fnb++;
		}	
	}

	//iglobalsstr.seekg(0, iglobalsstr.beg);
	//go through each lines...
	//char line[512];// = new char[512];
	pkMeshObject->m_akVertices.resize(vnb, vec4(1));
	std::vector<vec3> akNormals(vnnb);
	std::vector<vec2> akUVs(vtnb);

	std::vector<TriangleData> akTriangles(fnb);


	UI32 vidx = 0, vtidx = 0, vnidx = 0, fidx = 0;
	while (!iglobalsstr.eof())
	{
		iglobalsstr.getline(line, 512);
		std::stringstream isstr(line);
		
		std::string type;
		isstr >> type;

		if (type == "v")
		{
			//vec3 vertex;
			isstr >> pkMeshObject->m_akVertices[vidx][0];
			isstr >> pkMeshObject->m_akVertices[vidx][1];
			isstr >> pkMeshObject->m_akVertices[vidx++][2];
			//pkMeshObject->m_akVertices.push_back(vertex);
		}
		else if(type == "vt")
		{
			//vec2 uv;
			isstr >> akUVs[vtidx][0];
			isstr >> akUVs[vtidx++][1];
			//akUVs.push_back(uv);
		}
		else if (type == "vn")
		{
			//vec3 normal;
			isstr >> akNormals[vnidx][0];
			isstr >> akNormals[vnidx][1];
			isstr >> akNormals[vnidx++][2];
			//akNormals.push_back(normal);
		}
		else if (type == "f")
		{
			std::string t1, t2, t3;
			isstr >> t1 >> t2 >> t3;
			
			//TriangleData data;

			//first vertex
			ParseTriangle(akTriangles[fidx].t1, t1);

			//second vertex
			ParseTriangle(akTriangles[fidx].t2, t2);

			//third vertex
			ParseTriangle(akTriangles[fidx++].t3, t3);

			//akTriangles.push_back(data);
		}
	}

	//now process the vertex...
	pkMeshObject->m_akNormals.resize(pkMeshObject->m_akVertices.size());
	pkMeshObject->m_akColor.resize(pkMeshObject->m_akVertices.size());
	pkMeshObject->m_akUVs.resize(pkMeshObject->m_akVertices.size());
	pkMeshObject->m_aiIndices.resize(akTriangles.size()*3);
	
	for (UI32 i = 0; i < akTriangles.size(); ++i)
	{
		FillDataForTriangle(akTriangles[i].t1, i * 3, pkMeshObject, akNormals, akUVs);
		FillDataForTriangle(akTriangles[i].t2, i * 3 + 1, pkMeshObject, akNormals, akUVs);
		FillDataForTriangle(akTriangles[i].t3, i * 3 + 2, pkMeshObject, akNormals, akUVs);
	}
	
	return true;
}

void ObjLoader::ParseTriangle(TriangleIndex & idx, std::string t)
{
	std::string::size_type sz;
	UI32 dataUI;

	if (!FirstCharIsNumber(t)) 
	{
		Advance(t); 
	}
	else
	{
		dataUI = std::stoul(t, &sz);
		if (sz != 0)	idx.v = dataUI;
		t = t.substr(sz);
	}

	if (FirstCharIsChar(t, '/')) Advance(t);

	if (!FirstCharIsNumber(t))
	{
		Advance(t);
	}
	else
	{
		dataUI = std::stoul(t, &sz);
		if (sz != 0)	idx.vt = dataUI;
		t = t.substr(sz);
	}

	if (FirstCharIsChar(t, '/')) Advance(t);

	if (!FirstCharIsNumber(t))
	{
		Advance(t);
	}
	else
	{
		dataUI = std::stoul(t, &sz);
		if (sz != 0)	idx.vn = dataUI;
		t = t.substr(sz);
	}
}

void ObjLoader::FillDataForTriangle(const TriangleIndex & tri, const UI32 index, Mesh * pkMesh, std::vector<vec3>& akNormals, std::vector<vec2>& akUVs)
{
	pkMesh->m_aiIndices[index] = tri.v - 1;

	if (tri.vn != 0)
	{
		pkMesh->m_akNormals[tri.v - 1] = normalize(akNormals[tri.vn - 1]);
	}
	else
	{
		pkMesh->m_akNormals[tri.v - 1] = vec3(0, 0, -1); // face forward...
	}

	if (tri.vt != 0)
	{
		pkMesh->m_akUVs[tri.v - 1] = akUVs[tri.vt - 1];
	}
	else
	{
		pkMesh->m_akUVs[tri.v - 1] = vec2(0, 0);
	}

	pkMesh->m_akColor[tri.v - 1] = vec4(1);
}

bool ObjLoader::FirstCharIsNumber(const std::string & t)
{
	return t[0] >= '0' && t[0] <= '9';
}

bool ObjLoader::FirstCharIsChar(const std::string & t, char cmp)
{
	return t[0] == cmp;
}

void ObjLoader::Advance(std::string & t)
{
	t = t.substr(1);
}