#include "stdafx.h"
#include "BMesh.h"

vec4 Face::GetNormal(const BMesh& rkMesh)
{
	//auto it = m_apkVerticesMap.begin();

	vec4 kEdge1 = rkMesh.m_akVertices[m_apkVerticesMap[1]] - rkMesh.m_akVertices[m_apkVerticesMap[0]];
	vec4 kEdge2 = rkMesh.m_akVertices[m_apkVerticesMap[2]] - rkMesh.m_akVertices[m_apkVerticesMap[0]];

	return normalize(cross(normalize(kEdge1), normalize(kEdge2)));
}

mat4 Face::GetTransform(const BMesh & rkMesh)
{
	vec4 kNormal = GetNormal(rkMesh);

	//get center
	vec4 kCenter = GetCenter(rkMesh);

	//get edge1
	vec4 kEdge1 = normalize(rkMesh.m_akVertices[m_apkVerticesMap[1]] - rkMesh.m_akVertices[m_apkVerticesMap[0]]);

	//get last vector
	vec4 kLastVector = normalize(cross(kEdge1, kNormal));

	mat4 kTransform(kLastVector, kEdge1, kNormal, kCenter);

	return kTransform;
}

vec4 Face::GetCenter(const BMesh & rkMesh)
{
	//find center
	vec4 center = vec4(0,0,0,1);
	for (auto it = m_apkVerticesMap.begin(); it != m_apkVerticesMap.end(); ++it)
	{
		center += rkMesh.m_akVertices[*it];
	}
	center /= center[3];
	return center;
}

F32 Face::GetAspectRatio(const BMesh & rkMesh)
{
	vec4 kEdge1 = rkMesh.m_akVertices[m_apkVerticesMap[1]] - rkMesh.m_akVertices[m_apkVerticesMap[0]];
	vec4 kEdge2 = rkMesh.m_akVertices[m_apkVerticesMap[3]] - rkMesh.m_akVertices[m_apkVerticesMap[0]];

	F32 fRet = length(kEdge1) / length(kEdge2);
	if (fRet < 1)
	{
		fRet = 1 / fRet;
	}

	return fRet;
}


void ExtrudeFace::Apply(BMesh & rkMesh)
{
	//get face
	Face kFace = rkMesh.m_akFaces[m_iFaceID];

	//erase it :-) but keep the edges and the vertices
	//rkMesh.m_akFaces.erase(m_iFaceID);

	//get face normal
	vec4 kNormal = kFace.GetNormal(rkMesh);

	//create new vertices
	I32 iFirstNewIndex = rkMesh.m_akVertices.size();
	auto it = kFace.m_apkVerticesMap.begin();
	UI32 uiNbVertices = kFace.m_apkVerticesMap.size();
	UI32 uiLastFaceID = rkMesh.m_akFaces.rbegin()->first;

	for (UI32 i = 0; i < uiNbVertices; ++i)
	{
		rkMesh.m_akVertices.push_back(rkMesh.m_akVertices[*(it + i)] + m_fScaleFactor * kNormal);//vec4(kNormal,0));
	}
	
	//create additional faces
	for (UI32 i = 0; i < uiNbVertices; ++i)
	{
		Face kNewFace;
		it = kFace.m_apkVerticesMap.begin();
		kNewFace.m_apkVerticesMap.push_back(*(it + i));
		kNewFace.m_apkVerticesMap.push_back(*(it + (i + 1) % uiNbVertices));
		kNewFace.m_apkVerticesMap.push_back(iFirstNewIndex + (i + 1) % uiNbVertices);
		kNewFace.m_apkVerticesMap.push_back(iFirstNewIndex + i);

		kNewFace.m_kColor = kFace.m_kColor;

		rkMesh.m_akFaces[uiLastFaceID + i + 1] = kNewFace;
	}

	//modify kFace and reinsert it
	kFace.m_apkVerticesMap.clear();
	for (UI32 i = 0; i < uiNbVertices; ++i)
	{
		kFace.m_apkVerticesMap.push_back(iFirstNewIndex + i);
	}
	rkMesh.m_akFaces[m_iFaceID] = kFace;

	//create new faces
	/*//face 1
	it = kFace.m_apkVerticesMap.begin();
	auto itEnd = rkMesh.m_akFaces.rbegin();
	Face kNewFace;
	kNewFace.m_apkVerticesMap.push_back(iFirstNewIndex);
	kNewFace.m_apkVerticesMap.push_back(*it);
	kNewFace.m_apkVerticesMap.push_back(*++it);
	kNewFace.m_apkVerticesMap.push_back(iFirstNewIndex + 1);

	//add it back into the faces
	rkMesh.m_akFaces[itEnd->first + 1] = kNewFace;

	//face 2
	//it = pkFace.m_apkVerticesMap.begin();
	//auto itEnd = rkMesh.m_akFaces.end()--;
	Face kNewFace2;
	kNewFace2.m_apkVerticesMap.push_back(iFirstNewIndex + 1);
	kNewFace2.m_apkVerticesMap.push_back(*it);
	kNewFace2.m_apkVerticesMap.push_back(*++it);
	kNewFace2.m_apkVerticesMap.push_back(iFirstNewIndex + 2);

	//add it back into the faces
	rkMesh.m_akFaces[itEnd->first + 1] = kNewFace2;

	//face 3
	//it = pkFace.m_apkVerticesMap.begin();
	//auto itEnd = rkMesh.m_akFaces.end()--;
	Face kNewFace3;
	kNewFace3.m_apkVerticesMap.push_back(iFirstNewIndex + 2);
	kNewFace3.m_apkVerticesMap.push_back(*it);
	kNewFace3.m_apkVerticesMap.push_back(*++it);
	kNewFace3.m_apkVerticesMap.push_back(iFirstNewIndex + 3);

	//add it back into the faces
	rkMesh.m_akFaces[itEnd->first + 1] = kNewFace3;

	//face 4
	//it = pkFace.m_apkVerticesMap.begin();
	//auto itEnd = rkMesh.m_akFaces.end()--;
	Face kNewFace4;
	kNewFace4.m_apkVerticesMap.push_back(iFirstNewIndex + 3);
	kNewFace4.m_apkVerticesMap.push_back(*it);
	kNewFace4.m_apkVerticesMap.push_back(*kFace.m_apkVerticesMap.begin());
	kNewFace4.m_apkVerticesMap.push_back(iFirstNewIndex);

	//add it back into the faces
	rkMesh.m_akFaces[itEnd->first + 1] = kNewFace;

	//modify exisiting face
	kFace.m_apkVerticesMap.clear();
	kFace.m_apkVerticesMap.push_back(iFirstNewIndex);
	kFace.m_apkVerticesMap.push_back(iFirstNewIndex + 1);
	kFace.m_apkVerticesMap.push_back(iFirstNewIndex + 2);
	kFace.m_apkVerticesMap.push_back(iFirstNewIndex + 3);
	rkMesh.m_akFaces[m_iFaceID] = kFace;*/
}


void ScaleFace::Apply(BMesh & rkMesh)
{
	//get face
	Face kFace = rkMesh.m_akFaces[m_iFaceID];

	/*//find center
	Vertice center = Vertice(0);
	for (auto it = kFace.m_apkVerticesMap.begin(); it != kFace.m_apkVerticesMap.end(); ++it)
	{
		center += rkMesh.m_akVertices[*it];
	}
	center /= (F32) kFace.m_apkVerticesMap.size();*/

	mat4 kScaleMatrix = scale(vec3(m_fScaleFactorX, m_fScaleFactorY, 1));
	mat4 kFaceMatrix = kFace.GetTransform(rkMesh);
	mat4 kFaceMatrixInv = invert(kFaceMatrix);
	mat4 kFinalMatrix = kFaceMatrix * kScaleMatrix * kFaceMatrixInv;

	//scale towards center
	for (auto it = kFace.m_apkVerticesMap.begin(); it != kFace.m_apkVerticesMap.end(); ++it)
	{
		//vec3 dir = normalize(center - rkMesh.m_akVertices[*it]);
		vec4 kHomoGeneousVertice = kFinalMatrix * vec4(rkMesh.m_akVertices[*it][0], rkMesh.m_akVertices[*it][1], rkMesh.m_akVertices[*it][2],1);
		rkMesh.m_akVertices[*it] = kHomoGeneousVertice;// vec3(kHomoGeneousVertice[0], kHomoGeneousVertice[1], kHomoGeneousVertice[2], 1);
	}

	rkMesh.m_akFaces[m_iFaceID] = kFace;
}

void RibExtrusion::Apply(BMesh & rkMesh)
{
	ExtrudeFace kExtrusion_0_25(m_iFaceID, 0.25f);
	ExtrudeFace kExtrusion_0_5(m_iFaceID, 0.5f);
	ExtrudeFace kExtrusion_0(m_iFaceID, 0.0f);
	ScaleFace kScale_0_8(m_iFaceID, 0.8f, 0.8f);
	ScaleFace kScale_1_25(m_iFaceID, 1.25f, 1.25f);

	for (I32 i = 0; i < m_iNbRibs; ++i)
	{
		kExtrusion_0_25.Apply(rkMesh);
		kExtrusion_0.Apply(rkMesh);
		kScale_0_8.Apply(rkMesh);
		kExtrusion_0_5.Apply(rkMesh);
		kExtrusion_0.Apply(rkMesh);
		kScale_1_25.Apply(rkMesh);
		kExtrusion_0_25.Apply(rkMesh);
	}
}


void SubdivideFace::Apply(BMesh & rkMesh)
{
	std::list<I32> akLocalFaces;
	//get face
	Face kFace = rkMesh.m_akFaces[m_iFaceID];

	//check is this id is in
	bool bIsIn = false;
	for (auto it = m_apkSubdivFacesID->begin(); it != m_apkSubdivFacesID->end(); ++it)
	{
		if(*it == m_iFaceID)
		{
			bIsIn = true;
			break;
		}
	}

	akLocalFaces.push_back(m_iFaceID);

	if (!bIsIn)
	{
		m_apkSubdivFacesID->push_back(m_iFaceID);
	}

	//subdivide by finding the center of each edge and the center of the face
	I32 iNewCenter = rkMesh.m_akVertices.size();
	I32 iNewEdgeVertices = iNewCenter + 1;
	UI32 uiVerticesNb = kFace.m_apkVerticesMap.size();
	rkMesh.m_akVertices.push_back(kFace.GetCenter(rkMesh));
	if (m_apkNonFixedVertices != nullptr)
	{
		m_apkNonFixedVertices->insert(rkMesh.m_akVertices.size() - 1); // center is never fixed
	}

	for (UI32 i = 0; i < uiVerticesNb; ++i) // edges
	{
		rkMesh.m_akVertices.push_back(0.5f * (rkMesh.m_akVertices[kFace.m_apkVerticesMap[i]] + rkMesh.m_akVertices[kFace.m_apkVerticesMap[(i + 1) % uiVerticesNb]]));//e1

		if (m_apkNonFixedVertices != nullptr)
		{
			auto itI = m_apkNonFixedVertices->find(kFace.m_apkVerticesMap[i]);
			auto itIp1 = m_apkNonFixedVertices->find(kFace.m_apkVerticesMap[(i + 1) % uiVerticesNb]);

			if (itI != m_apkNonFixedVertices->end() || itIp1 != m_apkNonFixedVertices->end())
			{
				m_apkNonFixedVertices->insert(rkMesh.m_akVertices.size() - 1);
			}
		}
	}

	//start with lower left corner, and stamp it into kFace
	Face kNewFace;
	kNewFace.m_apkVerticesMap.push_back(kFace.m_apkVerticesMap[0]);
	kNewFace.m_apkVerticesMap.push_back(iNewEdgeVertices);
	kNewFace.m_apkVerticesMap.push_back(iNewCenter);
	kNewFace.m_apkVerticesMap.push_back(iNewEdgeVertices + uiVerticesNb - 1);
	kNewFace.m_kColor = kFace.m_kColor;
	rkMesh.m_akFaces[m_iFaceID] = kNewFace;

	

	//do the remaining faces
	for (UI32 i = 1; i < uiVerticesNb; ++i) // edges
	{
		Face kNewFace;
		kNewFace.m_apkVerticesMap.push_back(kFace.m_apkVerticesMap[i]);
		kNewFace.m_apkVerticesMap.push_back(iNewEdgeVertices + i);
		kNewFace.m_apkVerticesMap.push_back(iNewCenter);
		kNewFace.m_apkVerticesMap.push_back(iNewEdgeVertices + (i - 1) % uiVerticesNb);
		kNewFace.m_kColor = kFace.m_kColor;

		//shift the vertices i times to the left circularly
		for (UI32 j = 0; j < i; ++j)
		{
			I32 iLastOne = kNewFace.m_apkVerticesMap[uiVerticesNb - 1];
			for (UI32 k = uiVerticesNb - 1; k > 0; --k)
			{
				kNewFace.m_apkVerticesMap[k] = kNewFace.m_apkVerticesMap[k - 1];
			}
			kNewFace.m_apkVerticesMap[0] = iLastOne;
		}

		UI32 uiNewID = rkMesh.m_akFaces.size();
		rkMesh.m_akFaces[uiNewID] = kNewFace;
		m_apkSubdivFacesID->push_back(uiNewID);
		akLocalFaces.push_back(uiNewID);
	}

	
	//check if subdiv is still needed
	if (m_iSubDivLevel < m_iNbSubdiv)
	{
		for (auto it = akLocalFaces.begin(); it != akLocalFaces.end(); ++it)
		{
			SubdivideFace kSubdiv(*it, m_iNbSubdiv, m_apkSubdivFacesID, m_apkNonFixedVertices, m_iSubDivLevel + 1);
			kSubdiv.Apply(rkMesh);
		}
	}
}


void MakeCube::Apply(BMesh & rkMesh)
{
	//make vertices
	mat4 kScaleMatrix = m_kPositionMatrix * scale(m_kScale);

	I32 iStart = rkMesh.m_akVertices.size();
	rkMesh.m_akVertices.push_back(Vertice(-1, -1, -1, 1));//0
	rkMesh.m_akVertices.push_back(Vertice(-1, 1, -1, 1));//1
	rkMesh.m_akVertices.push_back(Vertice(1, 1, -1, 1));//2
	rkMesh.m_akVertices.push_back(Vertice(1, -1, -1, 1));//3

	rkMesh.m_akVertices.push_back(Vertice(-1, -1, 1, 1));//4
	rkMesh.m_akVertices.push_back(Vertice(-1, 1, 1, 1));//5
	rkMesh.m_akVertices.push_back(Vertice(1, 1, 1, 1));//6
	rkMesh.m_akVertices.push_back(Vertice(1, -1, 1, 1));//7

	for (UI32 i = iStart; i < rkMesh.m_akVertices.size(); ++i)
	{
		vec4 scaledVertice = kScaleMatrix * rkMesh.m_akVertices[i];
		rkMesh.m_akVertices[i] = scaledVertice;// Vertice(scaledVertice[0], scaledVertice[1], scaledVertice[2]);
	}

	//make edges
	/*rkMesh.m_akEdges[0] = Edge( &rkMesh.m_akVertices[0], &rkMesh.m_akVertices[1] );
	rkMesh.m_akEdges[1] = Edge(&rkMesh.m_akVertices[0], &rkMesh.m_akVertices[3]);
	rkMesh.m_akEdges[2] = Edge(&rkMesh.m_akVertices[0], &rkMesh.m_akVertices[4]);
	rkMesh.m_akEdges[3] = Edge(&rkMesh.m_akVertices[5], &rkMesh.m_akVertices[1]);
	rkMesh.m_akEdges[4] = Edge(&rkMesh.m_akVertices[2], &rkMesh.m_akVertices[1]);
	rkMesh.m_akEdges[5] = Edge(&rkMesh.m_akVertices[3], &rkMesh.m_akVertices[2]);
	rkMesh.m_akEdges[6] = Edge(&rkMesh.m_akVertices[2], &rkMesh.m_akVertices[6]);
	rkMesh.m_akEdges[7] = Edge(&rkMesh.m_akVertices[3], &rkMesh.m_akVertices[7]);
	rkMesh.m_akEdges[8] = Edge(&rkMesh.m_akVertices[4], &rkMesh.m_akVertices[7]);
	rkMesh.m_akEdges[9] = Edge(&rkMesh.m_akVertices[6], &rkMesh.m_akVertices[7]);
	rkMesh.m_akEdges[10] = Edge(&rkMesh.m_akVertices[6], &rkMesh.m_akVertices[5]);
	rkMesh.m_akEdges[11] = Edge(&rkMesh.m_akVertices[4], &rkMesh.m_akVertices[5]);*/

	//make faces
	rkMesh.m_akFaces[0] = Face();
	rkMesh.m_akFaces[1] = Face();
	rkMesh.m_akFaces[2] = Face();
	rkMesh.m_akFaces[3] = Face();
	rkMesh.m_akFaces[4] = Face();
	rkMesh.m_akFaces[5] = Face();

	rkMesh.m_akFaces[0].m_apkVerticesMap = std::vector<I32>{ 4,7,6,5 };
	rkMesh.m_akFaces[1].m_apkVerticesMap = std::vector<I32>{ 7,3,2,6 };
	rkMesh.m_akFaces[2].m_apkVerticesMap = std::vector<I32>{ 5,6,2,1 };
	rkMesh.m_akFaces[3].m_apkVerticesMap = std::vector<I32>{ 3,0,1,2 };
	rkMesh.m_akFaces[4].m_apkVerticesMap = std::vector<I32>{ 0,4,5,1 };
	rkMesh.m_akFaces[5].m_apkVerticesMap = std::vector<I32>{ 0,3,7,4 };

}

void MakeSpaceShip::Apply(BMesh & rkMesh)
{
	std::uniform_real_distribution<F32> kCubeScale(0.3f, 2.0f);
	vec3 kScaleVector(kCubeScale(m_kRng), kCubeScale(m_kRng), kCubeScale(m_kRng));
	std::cout << "cube scale " << kScaleVector[0] << " " << kScaleVector[1] << " "  << kScaleVector[2] << std::endl;
	MakeCube kCube(kScaleVector);
	kCube.Apply(rkMesh);

	//apply a random color to each faces
	vec4 kColor(m_kDist(m_kRng), m_kDist(m_kRng), m_kDist(m_kRng), 1);
	for (auto it = rkMesh.m_akFaces.begin(); it != rkMesh.m_akFaces.end(); ++it)
	{
		it->second.m_kColor = kColor;
	}

	/*ExtrudeFace kExtrude(0, 1);
	ScaleFace kScale(0, 0.5f);
	ExtrudeFace kExtrude1(1, 0.5f);
	RibExtrusion kRib(4, 5, 3);

	kExtrude.Apply(rkMesh);
	kScale.Apply(rkMesh);
	kExtrude1.Apply(rkMesh);
	kRib.Apply(rkMesh);*/

	//auto itEnd = rkMesh.m_akFaces.rbegin();
	I32 iFaceEnd = rkMesh.m_akFaces.size();
	for (I32 iFaceID = 0; iFaceID < iFaceEnd; ++iFaceID)
	{
		//I32 iFaceID = it->first;
		auto it = rkMesh.m_akFaces.find(iFaceID);
		//Get the normal and extrude only if z axis
		vec4 kNormal = it->second.GetNormal(rkMesh);
		if (std::abs(kNormal[2]) > 0.5f)
		{
			//ok so compute the number of extrusions needed and the length of extrusions
			std::uniform_real_distribution<F32> kDistHL(0.3f, 1.0f);
			F32 hullSegmentLength = kDistHL(m_kRng);

			std::uniform_int_distribution<I32> kDistHN(m_iMinHullExtrusions, m_iMaxHullExtrusions);
			I32 iNumExtrusions = kDistHN(m_kRng);

			for (I32 i = 0; i < iNumExtrusions; ++i)
			{
				bool bIsLast = i == iNumExtrusions - 1;

				//only extrude if random >0.1f else do rib extrusion
				if (m_kDist(m_kRng) > 0.1f)
				{
					std::cout << "Extruding " << it->first << " " << hullSegmentLength << std::endl;
					ExtrudeFace kExtrude(it->first, hullSegmentLength);
					kExtrude.Apply(rkMesh);
					it = rkMesh.m_akFaces.find(iFaceID);
					//sometimes add a tiny bit
					if (m_kDist(m_kRng) > 0.75f)
					{
						std::cout << "Extruding " << it->first << " " << hullSegmentLength * 0.25f << std::endl;
						ExtrudeFace kExtrudeTiny(it->first, hullSegmentLength * 0.25f);
						kExtrudeTiny.Apply(rkMesh);
						it = rkMesh.m_akFaces.find(iFaceID);
					}

					//maybe scale a bit
					if (m_kDist(m_kRng) > 0.5f)
					{
						//get scale factor and apply scale
						std::uniform_real_distribution<F32> kDistScale(1.2f, 1.5f);
						F32 fScale = kDistScale(m_kRng);

						if (m_kDist(m_kRng) > 0.5f || bIsLast)
						{
							fScale = 1 / fScale;
						}

						std::cout << "Scaling " << it->first << " " << fScale << std::endl;

						ScaleFace kScale(it->first, fScale, fScale);
						kScale.Apply(rkMesh);	
						it = rkMesh.m_akFaces.find(iFaceID);
					}

					//translate?
					if (m_kDist(m_kRng) > 0.5f)
					{
						//get translate factor and apply scale
						std::uniform_real_distribution<F32> kDistScale(0.1f, 0.4f);
						vec4 kTranslate = vec4(0, kDistScale(m_kRng) * kScaleVector[1] * hullSegmentLength, 0, 1);

						std::cout << "Translating " << it->first << " " << kTranslate << std::endl;
						TranslateFace kScale(it->first, kTranslate);
						kScale.Apply(rkMesh);
						it = rkMesh.m_akFaces.find(iFaceID);
					}

					//rotate?
					if (m_kDist(m_kRng) > 0.8f)
					{
						//get translate factor and apply scale
						std::uniform_real_distribution<F32> kDistScale(-0.5f, 0.5f);
						//F32 kRotateX = kDistScale(m_kRng);
						F32 kRotateY = kDistScale(m_kRng);

						std::cout << "Rotating " << it->first << " " << 0 << " " << kRotateY << std::endl;
						RotateFace kRotate(it->first, 0, kRotateY, 0);
						kRotate.Apply(rkMesh);
						it = rkMesh.m_akFaces.find(iFaceID);
					}
				}
				else
				{
					

					//get scale factor and apply scale
					std::uniform_real_distribution<F32> kDistScale(1.2f, 1.5f);
					F32 fScale = kDistScale(m_kRng);

					if (m_kDist(m_kRng) > 0.5f || bIsLast)
					{
						fScale = 1 / fScale;
					}

					//get number of ribs
					std::uniform_int_distribution<I32> kDistHN(1, 3);
					I32 iNumExtrusions = kDistHN(m_kRng);

					std::cout << "Ribbing " << it->first << " " << iNumExtrusions << " " << fScale << std::endl;
					RibExtrusion kRib(it->first,fScale, iNumExtrusions);
					kRib.Apply(rkMesh);
					it = rkMesh.m_akFaces.find(iFaceID);
				}
			}

		}
	}

	//add assymetric faces
	iFaceEnd = rkMesh.m_akFaces.size();
	for (I32 iFaceID = 0; iFaceID < iFaceEnd; ++iFaceID)
	{
		auto it = rkMesh.m_akFaces.find(iFaceID);
		// Skip any long thin faces as it'll probably look stupid
		if (it->second.GetAspectRatio(rkMesh) > 4)
		{
			continue;
		}

		if (m_kDist(m_kRng) > 0.85f)
		{
			std::uniform_real_distribution<F32> kDistHullPieceLength(0.1f, 0.4f);
			std::uniform_int_distribution<I32> kNbPieces(m_iMinAsymetrySegmentsExtrusions, 
														m_iMaxAsymetrySegmentsExtrusions);
			F32 fHullPieceLength = kDistHullPieceLength(m_kRng);
			I32 iHullExtrusions = kNbPieces(m_kRng);
			ExtrudeFace kExtrude(iFaceID, fHullPieceLength);
			for (I32 i = 0; i < iHullExtrusions; ++i)
			{
				kExtrude.Apply(rkMesh);
				
				//maybe some scaling?
				if (m_kDist(m_kRng) > 0.25f)
				{
					std::uniform_real_distribution<F32> kScaleDist(1.1f, 1.5f);
					F32 fScale = 1/ kScaleDist(m_kRng);
					ScaleFace kScale(iFaceID, fScale, fScale);
					kScale.Apply(rkMesh);
				}				
			}
		}
	}


	//add details
	iFaceEnd = rkMesh.m_akFaces.size();
	std::list<I32> kEngineFaces;
	std::list<I32> kGridFaces;
	std::list<I32> kSphereFaces;
	for (I32 iFaceID = 0; iFaceID < iFaceEnd; ++iFaceID)
	{
		//I32 iFaceID = it->first;
		auto it = rkMesh.m_akFaces.find(iFaceID);
		//Get the normal and extrude only if z axis
		vec4 kNormal = it->second.GetNormal(rkMesh);
		
		//skip thin long faces
		if (rkMesh.m_akFaces[iFaceID].GetAspectRatio(rkMesh) > 4)
		{
			continue;
		}

		//exhaut pipe
		//check if back facing
		F32 value = m_kDist(m_kRng);
		if (kNormal[2] < -0.95f)
		{
			if (value > 0.75f || kEngineFaces.empty())
			{
				kEngineFaces.push_back(iFaceID);
				it->second.m_kColor *= 0.25f;
				it->second.m_kColor[3] = 1;
			}
			else if (value > 0.25f)
			{
				kGridFaces.push_back(iFaceID);
			}
		}
		else if (kNormal[2] > 0.9f)
		{
			if(value > 0.7f){}
			else if (value > 0.4f)
			{
				kGridFaces.push_back(iFaceID);
			}
		}
		else if (kNormal[1] > 0.9f) // top
		{
			if (value > 0.7f) {}
			else if (value > 0.4f)
			{
				kGridFaces.push_back(iFaceID);
			}
		}
		else if (kNormal[1] < -0.9f)// bottom
		{
			if (value > 0.75f) {}
			else if (value > 0.5f)
			{
				kGridFaces.push_back(iFaceID);
			}
		}
		else if (abs(kNormal[0]) > 0.9f) // side
		{
			if (value > 0.75f) {}
			else if (value > 0.6f)
			{
				kGridFaces.push_back(iFaceID);
			}
			else if (value > 0.4f)
			{
				if (rkMesh.m_akFaces[iFaceID].GetAspectRatio(rkMesh) > 2)
				{
					kSphereFaces.push_back(iFaceID);
				}
			}
		}
		
	
	}

	//build Engine faces
	MakeEngines(rkMesh, kEngineFaces);

	//build grid faces
	MakeGrids(rkMesh, kGridFaces);

	//build the sphere faces
	MakeSpheres(rkMesh, kSphereFaces);
}

void MakeSpaceShip::MakeEngines(BMesh & rkMesh, const std::list<I32>& akEngineFaces)
{
	//compute the shaping of exhaust before hand for coherence
	F32 fTiny, fInside, fExtrude;
	I32 iSubdiv;
	
	for (auto it = akEngineFaces.begin(); it != akEngineFaces.end(); ++it)
	{
		//parameters
		std::uniform_real_distribution<F32> kDistScaleT(1.3f, 1.6f);
		std::uniform_real_distribution<F32> kDistScaleI(1.05f, 1.1f);
		std::uniform_real_distribution<F32> kExtrude(0.1f, 0.2f);
		std::uniform_int_distribution<I32> kSubdivDist(0, 4 - (I32)rkMesh.m_akFaces[*it].GetAspectRatio(rkMesh));
		iSubdiv = kSubdivDist(m_kRng);
		fTiny = 1 / kDistScaleT(m_kRng);
		fInside = 1 / kDistScaleI(m_kRng);
		fExtrude = kExtrude(m_kRng);

		//subdiv
		std::list<I32> subDivFaces;
		subDivFaces.push_back(*it);
		
		if (iSubdiv > 0)
		{			
			SubdivideFace kSubdiv(*it, iSubdiv, &subDivFaces);
			kSubdiv.Apply(rkMesh);
		}

		for (auto its = subDivFaces.begin(); its != subDivFaces.end(); ++its)
		{
			ExhaustPipe kExhaust(*its, fTiny, fInside, fExtrude);
			kExhaust.Apply(rkMesh);
		}
	}
}

void MakeSpaceShip::MakeGrids(BMesh & rkMesh, const std::list<I32>& akGridFaces)
{
	//compute the shaping of exhaust before hand for coherence
	F32 fExtrude;
	I32 iSubdiv;
	F32 fScale = 0.8f;
	
	for (auto it = akGridFaces.begin(); it != akGridFaces.end(); ++it)
	{
		std::uniform_real_distribution<F32> kExtrude(0.025f, 0.15f);
		std::uniform_int_distribution<I32> kSubdivDist(0, 1);
		iSubdiv = kSubdivDist(m_kRng);
		fExtrude = kExtrude(m_kRng);

		std::list<I32> subDivFaces;
		subDivFaces.push_back(*it);
		
		if (iSubdiv > 0)
		{
			SubdivideFace kSubdiv(*it, iSubdiv, &subDivFaces);
			kSubdiv.Apply(rkMesh);
		}

		for (auto its = subDivFaces.begin(); its != subDivFaces.end(); ++its)
		{
			ExtrudeFace kExtrudeAction(*its, fExtrude);
			ScaleFace kScaleAction(*its, fScale, fScale);
			kExtrudeAction.Apply(rkMesh);
			kScaleAction.Apply(rkMesh);			
		}
	}
}

void MakeSpaceShip::MakeSpheres(BMesh & rkMesh, const std::list<I32>& akGridFaces)
{
	//compute the shaping of exhaust before hand for coherence
	F32 fCenter;
	F32 fRadii;

	for (auto it = akGridFaces.begin(); it != akGridFaces.end(); ++it)
	{
		std::uniform_real_distribution<F32> kCenterDist(-0.5f, 0);
		std::uniform_real_distribution<F32> kRadDist(0.1f, 0.2f);
		fCenter = kCenterDist(m_kRng);
		fRadii = kRadDist(m_kRng);

		std::list<I32> subDivFaces;
		subDivFaces.push_back(*it);

		std::cout << "spherify " << *it << " " << fCenter << " " << fRadii << std::endl;
		SpherifyFace kSubdiv(*it, fCenter,fRadii, 4);
		kSubdiv.Apply(rkMesh);
	}
}

void TranslateFace::Apply(BMesh & rkMesh)
{
	//get face
	Face kFace = rkMesh.m_akFaces[m_iFaceID];

	//translate
	for (UI32 i = 0; i < kFace.m_apkVerticesMap.size(); ++i)
	{
		rkMesh.m_akVertices[kFace.m_apkVerticesMap[i]] += m_kScaleFactor;
	}

}

void RotateFace::Apply(BMesh & rkMesh)
{
	//get face
	Face kFace = rkMesh.m_akFaces[m_iFaceID];

	/*//find center
	Vertice center = Vertice(0);
	for (auto it = kFace.m_apkVerticesMap.begin(); it != kFace.m_apkVerticesMap.end(); ++it)
	{
	center += rkMesh.m_akVertices[*it];
	}
	center /= (F32) kFace.m_apkVerticesMap.size();*/

	mat4 kScaleMatrix = vmath::rotate(m_fRotationX, m_fRotationX, m_fRotationZ);
	//mat4 kFaceMatrix = kFace.GetTransform(rkMesh);
	//mat4 kFaceMatrixInv = invert(kFaceMatrix);
	mat4 kFinalMatrix = kScaleMatrix;// kFaceMatrix * kScaleMatrix * kFaceMatrixInv;

	//scale towards center
	for (auto it = kFace.m_apkVerticesMap.begin(); it != kFace.m_apkVerticesMap.end(); ++it)
	{
		//vec3 dir = normalize(center - rkMesh.m_akVertices[*it]);
		vec4 kHomoGeneousVertice = kFinalMatrix * rkMesh.m_akVertices[*it];
		rkMesh.m_akVertices[*it] = kHomoGeneousVertice;// vec3(kHomoGeneousVertice[0], kHomoGeneousVertice[1], kHomoGeneousVertice[2]);
	}

	rkMesh.m_akFaces[m_iFaceID] = kFace;
}

void SpherifyFace::Apply(BMesh & rkMesh)
{
	//first get the face, the normal, and the center position
	Face kFace = rkMesh.m_akFaces[m_iFaceID];
	vec4 kNormal = kFace.GetNormal(rkMesh);
	vec4 kCenter = kFace.GetCenter(rkMesh);
	mat4 kFaceMatrix = kFace.GetTransform(rkMesh);
	mat4 kFaceMatrixInverted = invert(kFaceMatrix);
	F32 fAspectRatio = kFace.GetAspectRatio(rkMesh);
	if (m_fSphereCenter == 0) m_fSphereCenter -= 0.001f; // small deviation to avoid 0 distance in distance computation
	vec4 kSphereCenter = (m_fSphereCenter) * kNormal + kCenter;
	vec4 kLocalSphereCenter = kFaceMatrixInverted * kSphereCenter;


	//then subdivide but keep non fixed vertices at hand
	std::set<I32> akNonFixedVertices;
	std::list<I32> akFaces;

	SubdivideFace kSubdiv(m_iFaceID, m_iSubdivSize, &akFaces, &akNonFixedVertices);
	kSubdiv.Apply(rkMesh);


	//loop through all the set elements and push them away from center along the radii
	for (auto it = akNonFixedVertices.begin(); it != akNonFixedVertices.end(); ++it)
	{
		//sphere distance
		vec4 kDistToCenter = rkMesh.m_akVertices[*it] - kSphereCenter;

		if (length(kDistToCenter) > m_fSphereRadius*fAspectRatio)
			continue;

		//compute the vertice in face direction
		vec4 kLocalVertice = kFaceMatrixInverted * vec4(rkMesh.m_akVertices[*it]);

		//get the intended distance along the normal while keeping the x and y values
		kLocalVertice[2] = sqrt(m_fSphereRadius * fAspectRatio * fAspectRatio * m_fSphereRadius -
											((kLocalVertice[0]-kLocalSphereCenter[0]) * (kLocalVertice[0] - kLocalSphereCenter[0]) +
											(kLocalVertice[1] - kLocalSphereCenter[1]) * (kLocalVertice[1] - kLocalSphereCenter[1]))) 
							+ kLocalSphereCenter[2];

		//invert it back
		kLocalVertice = kFaceMatrix * kLocalVertice;

		//push the element in this direction
		rkMesh.m_akVertices[*it] = kLocalVertice;// vec3(kLocalVertice[0], kLocalVertice[1], kLocalVertice[2]);
	}


}


void ExhaustPipe::Apply(BMesh & rkMesh)
{
	//extrude the face
	ExtrudeFace kExtrude(m_iFaceID, m_fExtrudeLength);
	ScaleFace kTinyScaling(m_iFaceID, m_fTinyScaling, m_fTinyScaling);
	ExtrudeFace kZeroExtrude(m_iFaceID, 0);
	ScaleFace kInsideScaling(m_iFaceID, m_fScaleFactorIn, m_fScaleFactorIn);
	ExtrudeFace kInsideExtrude(m_iFaceID, -m_fExtrudeLength);

	kExtrude.Apply(rkMesh);
	kTinyScaling.Apply(rkMesh);
	kZeroExtrude.Apply(rkMesh);
	kInsideScaling.Apply(rkMesh);
	kInsideExtrude.Apply(rkMesh);

	rkMesh.m_akFaces[m_iFaceID].m_kColor = vec4(176, 176, 255, 1);

}


void BMesh::BuildMesh(Mesh & rkMesh)
{
	rkMesh.m_akVertices = m_akVertices;
	rkMesh.m_akNormals = std::vector<vec3>(m_akVertices.size(), vec3(0));
	rkMesh.m_akColor = std::vector<vec4>(m_akVertices.size(), vec4(1));
	rkMesh.m_akUVs = std::vector<vec2>(m_akVertices.size(), vec2(0));
	std::vector<I32> kNbAdded = std::vector<I32>(m_akVertices.size(), 0);
	/*std::mt19937 kRng(100);
	std::uniform_real_distribution<F32> kDist(0, 1);*/
	

	for (auto it = m_akFaces.begin(); it != m_akFaces.end(); ++it) // go through the faces
	{
		//build the triangles
		for (UI32 i = 0; i < it->second.m_apkVerticesMap.size() - 2; ++i)
		{
			rkMesh.m_aiIndices.push_back(it->second.m_apkVerticesMap[0]);
			rkMesh.m_aiIndices.push_back(it->second.m_apkVerticesMap[i+1]);
			rkMesh.m_aiIndices.push_back(it->second.m_apkVerticesMap[i+2]);
		}

		//compute random color
		
		//vec4 kColor(kDist(kRng), kDist(kRng), kDist(kRng), 1);

		//get the normals
		vec4 kNormal = it->second.GetNormal(*this);
		for (UI32 i = 0; i < it->second.m_apkVerticesMap.size(); ++i)
		{
			/*if (rkMesh.m_akNormals[it->second.m_apkVerticesMap[i]][0] == 0 &&
				rkMesh.m_akNormals[it->second.m_apkVerticesMap[i]][1] == 0 && 
				rkMesh.m_akNormals[it->second.m_apkVerticesMap[i]][2] == 0)
			{*/
				rkMesh.m_akNormals[it->second.m_apkVerticesMap[i]] += vec3(kNormal[0], kNormal[1], kNormal[2]);
				rkMesh.m_akColor[it->second.m_apkVerticesMap[i]] += it->second.m_kColor;
				++kNbAdded[it->second.m_apkVerticesMap[i]];
			//}
		}
	}

	for (UI32 i = 0; i < m_akVertices.size(); ++i)
	{
		rkMesh.m_akNormals[i] = normalize(rkMesh.m_akNormals[i]);
		rkMesh.m_akColor[i] /= (F32)kNbAdded[i];
	}
}

void MakeSpaceShip2::Apply(BMesh & rkMesh)
{
	MakeCube kCube;

	kCube.Apply(rkMesh);

	I32 iFront, iTop, iBottom, iLeft, iRight, iBack;

	for (UI32 i = 0; i < rkMesh.m_akFaces.size(); ++i)
	{
		vec4 kNormal = rkMesh.m_akFaces[i].GetNormal(rkMesh);

		if (kNormal[2] > 0)
		{
			iBack = i;
		}
		if (kNormal[2] < 0)
		{
			iFront = i;
		}
		if (kNormal[0] > 0)
		{
			iRight = i;
		}
		if (kNormal[0] < 0)
		{
			iLeft = i;
		}
		if (kNormal[1] > 0)
		{
			iTop = i;
		}
		if (kNormal[1] < 0)
		{
			iBottom = i;
		}
	}

	//make front
	ExtrudeFace kFront1(iFront, 1);
	ExtrudeFace kFront2(iFront, 0.5f);
	ScaleFace kFrontScale1(iFront, 0.7f, 0.7f);
	TranslateFace kTranslate1(iFront, vec4(0, -0.3f, 0, 1));
	TranslateFace kTranslate2(iFront, vec4(0, -0.25f, 0, 1));


	kFront1.Apply(rkMesh);
	kFrontScale1.Apply(rkMesh);
	kTranslate1.Apply(rkMesh);
	kFront2.Apply(rkMesh);
	kFrontScale1.Apply(rkMesh);
	kTranslate2.Apply(rkMesh);

	//make back
	ExhaustPipe kPipe(iBack, 0.9f, 0.8f, 0.2f);

	kPipe.Apply(rkMesh);
	

	//make top
	ExtrudeFace kTop(iTop, 0.25f);
	ScaleFace kTopScale(iTop, 0.5f, 0.5f);
	kTop.Apply(rkMesh);
	kTopScale.Apply(rkMesh);

	//make Left Wing
	ExtrudeFace kLeftWing1(iLeft, 0.1f);
	ScaleFace kLeftWingScale1(iLeft, 0.2f, 0.9f);
	TranslateFace kLeftWingTranslate1(iLeft, vec4(0, 0.7f, 0, 1));

	ExtrudeFace kLeftWing2(iLeft, 0.1f);
	ScaleFace kLeftWingScale2(iLeft, 1, 0.8f);
	TranslateFace kLeftWingTranslate2(iLeft, vec4(0, 0.2f, 0, 1));

	ExtrudeFace kLeftWing3(iLeft, 2);


	kLeftWing1.Apply(rkMesh);
	kLeftWingScale1.Apply(rkMesh);
	kLeftWingTranslate1.Apply(rkMesh);
	kLeftWing2.Apply(rkMesh);
	kLeftWingScale2.Apply(rkMesh);
	kLeftWingTranslate2.Apply(rkMesh);
	kLeftWing3.Apply(rkMesh);

	//make Right Wing
	ExtrudeFace kRightWing1(iRight, 0.1f);
	ScaleFace kRightWingScale1(iRight, 0.2f, 0.9f);
	TranslateFace kRightWingTranslate1(iRight, vec4(0, 0.7f, 0, 1));

	ExtrudeFace kRightWing2(iRight, 0.1f);
	ScaleFace kRightWingScale2(iRight, 1, 0.8f);
	TranslateFace kRightWingTranslate2(iRight, vec4(0, 0.2f, 0, 1));

	ExtrudeFace kRightWing3(iRight, 2);


	kRightWing1.Apply(rkMesh);
	kRightWingScale1.Apply(rkMesh);
	kRightWingTranslate1.Apply(rkMesh);
	kRightWing2.Apply(rkMesh);
	kRightWingScale2.Apply(rkMesh);
	kRightWingTranslate2.Apply(rkMesh);
	kRightWing3.Apply(rkMesh);

}
