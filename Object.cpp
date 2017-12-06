#include "stdafx.h"
#include "Object.h"
#include "MeshManager.h"


Object::Object()
	: m_kPosition(0, 0, 0), m_kRotation(0, 0, 0)
{
}


Object::~Object()
{
}

void Object::Initialise()
{
	BMesh kBMesh;
	MakeSpaceShip kSpaceShip(GetSeedForBuilding());
	MakeCube kCube;
	ExtrudeFace kExtrude(1, 0.5);
	TranslateFace kTranslate(1, vec3(0,0.5f,0));
	ScaleFace kScale(1, 1.5f,0.5f);
	RotateFace kRotate(1, 0, 20, 0);
	RibExtrusion kRib(1, 2, 3);
	std::list<I32> m_apkAddedFace;
	SubdivideFace kSub(4, 2, &m_apkAddedFace);
	SpherifyFace kSpherify(4, -0.1f, 1, 4);
	MakeSpaceShip2 kSpaceShip2;

	//kCube.Apply(kBMesh);
	//kSpherify.Apply(kBMesh);
	//kSub.Apply(kBMesh);

	/*for (auto it = m_apkAddedFace.begin(); it != m_apkAddedFace.end(); ++it)
	{
		ExhaustPipe kExhaust(*it, 0.95f, 0.9f, 0.1f);
		kExhaust.Apply(kBMesh);
	}*/
	//kRib.Apply(kBMesh);
	//kExtrude.Apply(kBMesh);
	//kTranslate.Apply(kBMesh);
	//kScale.Apply(kBMesh);
	//kExtrude.Apply(kBMesh);
	//kRotate.Apply(kBMesh);
	//kSpaceShip.Apply(kBMesh);
	kSpaceShip2.Apply(kBMesh);

	m_pkMesh = MeshManager::Instance()->Instantiate();

	kBMesh.BuildMesh(*m_pkMesh);
	//create vao
/*	glGenVertexArrays(1, &m_hiVao);
	glBindVertexArray(m_hiVao);*/

	/*//create the z curve
	std::vector<vmath::vec3> kControlPoints;
	kControlPoints.push_back(vmath::vec3(0.0f, 0.0f, 0.0f));
	kControlPoints.push_back(vmath::vec3(0.0f, 0.0f, 1));
	kControlPoints.push_back(vmath::vec3(0.0f, 0.0f, 3));
	kControlPoints.push_back(vmath::vec3(0.0f, 0.0f, 5));
	BezierCurve3D kCurve(kControlPoints);

	//create the XScale
	std::vector<F32> kXScaleControlPoints;
	kXScaleControlPoints.push_back(2);
	kXScaleControlPoints.push_back(1.8f);
	kXScaleControlPoints.push_back(1.3f);
	kXScaleControlPoints.push_back(0);
	BezierCurve<F32> kXCurve(kXScaleControlPoints);

	//create the YScale
	std::vector<F32> kYScaleControlPoints;
	kYScaleControlPoints.push_back(1);
	kYScaleControlPoints.push_back(0.66f);
	kYScaleControlPoints.push_back(0.33f);
	kYScaleControlPoints.push_back(0);
	BezierCurve<F32> kYCurve(kYScaleControlPoints);*/

	//create the z curve
	/*std::vector<vmath::vec3> kControlPoints;
	kControlPoints.push_back(vmath::vec3(0.0f, 0.0f, 0.0f));
	//kControlPoints.push_back(vmath::vec3(0.25f, 0.0f, -0.5f));
	//kControlPoints.push_back(vmath::vec3(0.33f, 0.0f, 0));
	//kControlPoints.push_back(vmath::vec3(0.25f, 0.0f, 0.5f));
	kControlPoints.push_back(vmath::vec3(0.0f, 0.0f, 1.0f));
	BezierCurve3D kCurve(kControlPoints);

	//create the Scales
	std::vector<F32> afStarts, afEnds;
	std::vector<F32> kXScaleControlPoints1;
	kXScaleControlPoints1.push_back(0.1f);
	kXScaleControlPoints1.push_back(0.1f);
	BezierCurve<F32> kXCurve1(kXScaleControlPoints1);
	afStarts.push_back(0);
	afEnds.push_back(0.2f);

	std::vector<F32> kXScaleControlPoints2;
	kXScaleControlPoints2.push_back(0.08f);
	kXScaleControlPoints2.push_back(0.08f);
	BezierCurve<F32> kXCurve2(kXScaleControlPoints2);
	afStarts.push_back(0.2f);
	afEnds.push_back(0.25f);

	std::vector<F32> kXScaleControlPoints3;
	kXScaleControlPoints3.push_back(0.1f);
	kXScaleControlPoints3.push_back(0.1f);
	BezierCurve<F32> kXCurve3(kXScaleControlPoints3);
	afStarts.push_back(0.25f);
	afEnds.push_back(0.4f);

	std::vector<F32> kXScaleControlPoints4;
	kXScaleControlPoints4.push_back(0.08f);
	kXScaleControlPoints4.push_back(0.08f);
	BezierCurve<F32> kXCurve4(kXScaleControlPoints3);
	afStarts.push_back(0.4f);
	afEnds.push_back(0.45f);

	std::vector<F32> kXScaleControlPoints5;
	kXScaleControlPoints5.push_back(0.1f);
	kXScaleControlPoints5.push_back(0.1f);
	BezierCurve<F32> kXCurve5(kXScaleControlPoints5);
	afStarts.push_back(0.45f);
	afEnds.push_back(0.6f);

	std::vector<F32> kXScaleControlPoints6;
	kXScaleControlPoints6.push_back(0.08f);
	kXScaleControlPoints6.push_back(0.08f);
	BezierCurve<F32> kXCurve6(kXScaleControlPoints6);
	afStarts.push_back(0.6f);
	afEnds.push_back(0.65f);

	std::vector<F32> kXScaleControlPoints7;
	kXScaleControlPoints7.push_back(0.1f);
	kXScaleControlPoints7.push_back(0.1f);
	kXScaleControlPoints7.push_back(0.09f);
	kXScaleControlPoints7.push_back(0.08f);
	kXScaleControlPoints7.push_back(0.07f);
	kXScaleControlPoints7.push_back(0.0f);
	BezierCurve<F32> kXCurve7(kXScaleControlPoints7);
	afStarts.push_back(0.65f);
	afEnds.push_back(1.0f);

	//create the Spline
	std::vector<BezierCurve<F32>> kCurves;
	kCurves.push_back(kXCurve1);
	kCurves.push_back(kXCurve2);
	kCurves.push_back(kXCurve3);
	kCurves.push_back(kXCurve4);
	kCurves.push_back(kXCurve5);
	kCurves.push_back(kXCurve6);
	kCurves.push_back(kXCurve7);
	BezierSpline<F32> kSpline(afStarts, afEnds, kCurves);


	//create the mesh
	SplinedRevolutionMesh(5, 100, *m_pkMesh, kCurve, kSpline, kSpline);*/
	m_pkMesh->LoadBuffersOnGraphicsCard();

	//create the data
	/*vmath::vec3* pkData;
	vmath::vec3* pkNormals;
	vmath::vec2* pkUVs;
	
	I32 iNbVertices;
	std::vector<UI32> aiIndices;
	CreateCircleColumn(5, 100, pkData, pkNormals, pkUVs, aiIndices, iNbVertices, kCurve, kSpline, kSpline);

	/*I32 iIntersectQuantity;
	F32 fTValues[2];
	//apkHit = new F32[iNbVertices];
	
	std::vector<F32> apkHit(iNbVertices, 0);
	m_kLine = Line(vmath::vec3(0, 0.01f, 0), vmath::normalize(vmath::vec3(0, 0.01f, 1)));
	Intersect::Find(m_kLine, pkData, (UI32)iNbVertices, aiIndices.data(), aiIndices.size() / 3, iIntersectQuantity, fTValues, apkHit);*/


	/*//create the buffer
	glGenBuffers(1, &m_hiDataBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_hiDataBuffer);
	glBufferData(GL_ARRAY_BUFFER, iNbVertices*sizeof(vmath::vec3), (unsigned char*)pkData, GL_STATIC_DRAW);

	//set it as attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//create Normals the buffer
	glGenBuffers(1, &m_hiNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_hiNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, iNbVertices * sizeof(vmath::vec3), (unsigned char*)pkNormals, GL_STATIC_DRAW);

	//set it as attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	//create the buffer
	glGenBuffers(1, &m_hiHitBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_hiHitBuffer);
	glBufferData(GL_ARRAY_BUFFER, iNbVertices * sizeof(F32), apkHit.data(), GL_STATIC_DRAW);

	//set it as attribute
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(5);

	//create the buffer
	glGenBuffers(1, &m_hiUVsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_hiUVsBuffer);
	glBufferData(GL_ARRAY_BUFFER, iNbVertices * sizeof(vmath::vec2), (unsigned char*)pkUVs, GL_STATIC_DRAW);

	//set it as attribute
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(4);

	//create index buffer
	glGenBuffers(1, &m_hiIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hiIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, aiIndices.size() * sizeof(UI32), aiIndices.data(), GL_STATIC_DRAW);
	m_uiSize = aiIndices.size();

	

	delete[] pkData;
	delete[] pkNormals;
	delete[] pkUVs;
//	delete[] apkHit;
	aiIndices.clear();*/
}

void Object::Update(double deltaTime)
{
}

void Object::Render()
{
	m_pkMesh->BindForDrawing();
	//glPointSize(10);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	//draw call
	glDrawElements(GL_TRIANGLES, m_pkMesh->m_aiIndices.size(),GL_UNSIGNED_INT, 0);

	/*glBindTexture(GL_TEXTURE_2D, m_hiBlackTexture);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	glDrawElements(GL_TRIANGLES, m_uiSize, GL_UNSIGNED_INT, 0);*/

	/*glUseProgram(m_kLineProgram);
	glUniformMatrix4fv(m_kProjMatLocation, 1, GL_FALSE, m_kProjMat);

	m_kLine.Draw(50);*/
	
}

void Object::Terminate()
{
	MeshManager::Delete();// m_pkMesh;
	/*glDeleteVertexArrays(1, &m_hiVao);
	glDeleteBuffers(1, &m_hiDataBuffer);
	glDeleteBuffers(1, &m_hiNormalBuffer);
	glDeleteBuffers(1, &m_hiUVsBuffer);
	glDeleteBuffers(1, &m_hiHitBuffer);*/
}

void Object::Translate(const vmath::vec3 & kMovement)
{
	m_kPosition += kMovement;
}

void Object::AddRotation(const vmath::vec3 & kAddRotation)
{
	m_kRotation += kAddRotation;
}

vmath::mat4 Object::GetMoveMatrix()
{
	return vmath::translate(m_kPosition) * vmath::rotate(m_kRotation[0], m_kRotation[1], m_kRotation[2]);
}

void CreateCircleColumn(const I32 & riNbSubdiv, const I32 & riNbStages, vmath::vec3 *& rpkBufferData, vmath::vec3 *& rpkNormalData,
						vmath::vec2 *& rpkUVS, std::vector<UI32>& raiIndices, I32 & iNbVertices, const BezierCurve3D& rkCurve, 
						const BezierSpline<F32>& rkXScale, const BezierSpline<F32>& rkYScale)
{
	//compute nb vertices
	iNbVertices = riNbSubdiv*riNbStages + 2;

	//create the buffer
	rpkBufferData = new vmath::vec3[iNbVertices];
	rpkUVS = new vmath::vec2[iNbVertices];
	rpkNormalData = new vmath::vec3[iNbVertices];

	//create the first vertice and the last one
	vmath::mat3 kMat;
	rkCurve.EvaluatePositionAndSpace(1, rpkBufferData[iNbVertices - 1], kMat);
	rpkNormalData[iNbVertices - 1] = kMat[2];

	rkCurve.EvaluatePositionAndSpace(0, rpkBufferData[0], kMat);
	rpkNormalData[0] = -kMat[2];

	//uvs for the first two vertices
	rpkUVS[0] = vmath::vec2(0.5,0);
	rpkUVS[iNbVertices - 1] = vmath::vec2(0.5, 1);

	//compute the angle between each vertices on each stage
	F32 fAngleRadians = vmath::radians(360 / (F32)riNbSubdiv);

	//compute the space between each stages
	F32 fSpaceBetweenStages = 1 / (F32)(riNbStages - 1);

	//compute the first stage vertices
	I32 iVerticeIndex = 1;
	F32 fCurrentAngle = 0;
	F32 fCurrentZ = 0;
	F32 fLength = 0;
	F32 fXScale, fYScale;
	rkXScale.EvaluatePosition(fCurrentZ, fXScale);
	rkYScale.EvaluatePosition(fCurrentZ, fYScale);

	for(I32 i = 0; i < riNbSubdiv; ++i, ++iVerticeIndex, fCurrentAngle += fAngleRadians)
	{
		//create vertice
		rpkBufferData[iVerticeIndex] = vmath::vec3(cos(fCurrentAngle), sin(fCurrentAngle), 0);
		rpkNormalData[iVerticeIndex] = vmath::normalize(rpkBufferData[iVerticeIndex] * kMat);
		rpkBufferData[iVerticeIndex][0] *= fXScale;
		rpkBufferData[iVerticeIndex][1] *= fYScale;
		rpkBufferData[iVerticeIndex] = rpkBufferData[iVerticeIndex] * kMat + rpkBufferData[0];

		//rpkBufferData[iVerticeIndex] = vmath::vec3(XScale*randX * cos(fCurrentAngle), YScale*randY * sin(fCurrentAngle), fZScale*fCurrentZ);
		rpkUVS[iVerticeIndex] = vmath::vec2((F32)i / (F32)(riNbSubdiv - 1), 0);

		//create triangle is i > 1
		if (i >= 1)
		{
			raiIndices.push_back(0);
			raiIndices.push_back(iVerticeIndex-1);
			raiIndices.push_back(iVerticeIndex);
		}
	}

	//create LastTriangle is i > 1
	raiIndices.push_back(0);
	raiIndices.push_back(iVerticeIndex - 1);
	raiIndices.push_back(iVerticeIndex - riNbSubdiv);


	//Create the remaing stages
	vmath::vec3 kOriginP = rpkBufferData[0];
	fCurrentZ += fSpaceBetweenStages;
	for (I32 j = 0; j < riNbStages - 1; ++j, fCurrentZ+=fSpaceBetweenStages)
	{
		if (fCurrentZ > 1)
		{
			fCurrentZ = 1;
		}

		fCurrentAngle = 0;
		vmath::vec3 kOrigin;
		vmath::mat3 kMat;
		rkCurve.EvaluatePositionAndSpace(fCurrentZ, kOrigin, kMat);

		fLength += vmath::length(kOrigin - kOriginP);
		kOriginP = kOrigin;

		rkXScale.EvaluatePosition(fCurrentZ, fXScale);
		rkYScale.EvaluatePosition(fCurrentZ, fYScale);
		for (I32 i = 0; i < riNbSubdiv; ++i, ++iVerticeIndex, fCurrentAngle += fAngleRadians)
		{
			//create vertice
			rpkBufferData[iVerticeIndex] = vmath::vec3(cos(fCurrentAngle), sin(fCurrentAngle), 0);
			rpkNormalData[iVerticeIndex] = vmath::normalize(rpkBufferData[iVerticeIndex] * kMat);
			rpkBufferData[iVerticeIndex][0] *= fXScale;
			rpkBufferData[iVerticeIndex][1] *= fYScale;
			rpkBufferData[iVerticeIndex] = rpkBufferData[iVerticeIndex] * kMat + kOrigin;
			

			//rpkBufferData[iVerticeIndex] = vmath::vec3(XScale * randX * cos(fCurrentAngle), YScale * randY * sin(fCurrentAngle), fZScale*fCurrentZ);
			rpkUVS[iVerticeIndex] = vmath::vec2((F32)i / (F32)(riNbSubdiv - 1), fCurrentZ);
			//rpkNormalData[iVerticeIndex] = vmath::normalize(rpkBufferData[iVerticeIndex] - kOrigin);

			//create triangle 1
			raiIndices.push_back(iVerticeIndex - riNbSubdiv);
			raiIndices.push_back(iVerticeIndex);
			raiIndices.push_back(iVerticeIndex - riNbSubdiv + 1);

			//create triangle 2 only if i > 1
			if (i >= 1)
			{
				raiIndices.push_back(iVerticeIndex - riNbSubdiv);
				raiIndices.push_back(iVerticeIndex - 1);
				raiIndices.push_back(iVerticeIndex);
			}
		}

		//create LastTriangle is i > 1
		raiIndices.push_back(iVerticeIndex - 2*riNbSubdiv);
		raiIndices.push_back(iVerticeIndex - riNbSubdiv - 1);
		raiIndices.push_back(iVerticeIndex - riNbSubdiv);
	}
	
	//create the last surface
	for (I32 i = 1; i < riNbSubdiv; ++i)
	{
		
		raiIndices.push_back(iNbVertices - i - 2);
		raiIndices.push_back(iNbVertices - 1);
		raiIndices.push_back(iNbVertices - i - 1);
	}

	//create LastTriangle is i > 1
	raiIndices.push_back(iNbVertices - 2);
	raiIndices.push_back(iNbVertices- 1);
	raiIndices.push_back(iNbVertices - riNbSubdiv - 1);
	
}

void SplinedRevolutionMesh(const I32 & riNbSubdiv, const I32 & riNbStages, Mesh & rkMesh, const BezierCurve3D & rkZCurve, const BezierSpline<F32>& rkXScale, const BezierSpline<F32>& rkYScale)
{
	//compute nb vertices
	UI32 iNbVertices = riNbSubdiv*riNbStages + 2;

	//create the buffer
	rkMesh.m_akVertices.resize(iNbVertices);
	rkMesh.m_akNormals.resize(iNbVertices);
	rkMesh.m_akColor.resize(iNbVertices, vec4(1,1,1,1));
	rkMesh.m_akUVs.resize(iNbVertices);

	//create the first vertice and the last one
	vmath::mat3 kMat;
	rkZCurve.EvaluatePositionAndSpace(1, rkMesh.m_akVertices[iNbVertices - 1], kMat);
	rkMesh.m_akNormals[iNbVertices - 1] = kMat[2];

	rkZCurve.EvaluatePositionAndSpace(0, rkMesh.m_akVertices[0], kMat);
	rkMesh.m_akNormals[0] = -kMat[2];

	//uvs for the first two vertices
	rkMesh.m_akUVs[0] = vmath::vec2(0.5, 0);
	rkMesh.m_akUVs[iNbVertices - 1] = vmath::vec2(0.5, 1);

	//compute the angle between each vertices on each stage
	F32 fAngleRadians = vmath::radians(360 / (F32)riNbSubdiv);

	//compute the space between each stages
	F32 fSpaceBetweenStages = 1 / (F32)(riNbStages - 1);

	//compute the first stage vertices
	I32 iVerticeIndex = 1;
	F32 fCurrentAngle = 0;
	F32 fCurrentZ = 0;
	F32 fLength = 0;
	F32 fXScale, fYScale;
	rkXScale.EvaluatePosition(fCurrentZ, fXScale);
	rkYScale.EvaluatePosition(fCurrentZ, fYScale);

	for (I32 i = 0; i < riNbSubdiv; ++i, ++iVerticeIndex, fCurrentAngle += fAngleRadians)
	{
		//create vertice
		rkMesh.m_akVertices[iVerticeIndex] = vmath::vec3(cos(fCurrentAngle), sin(fCurrentAngle), 0);
		rkMesh.m_akNormals[iVerticeIndex] = vmath::normalize(rkMesh.m_akVertices[iVerticeIndex] * kMat);
		rkMesh.m_akVertices[iVerticeIndex][0] *= fXScale;
		rkMesh.m_akVertices[iVerticeIndex][1] *= fYScale;
		rkMesh.m_akVertices[iVerticeIndex] = rkMesh.m_akVertices[iVerticeIndex] * kMat + rkMesh.m_akVertices[0];

		//rpkBufferData[iVerticeIndex] = vmath::vec3(XScale*randX * cos(fCurrentAngle), YScale*randY * sin(fCurrentAngle), fZScale*fCurrentZ);
		rkMesh.m_akUVs[iVerticeIndex] = vmath::vec2((F32)i / (F32)(riNbSubdiv - 1), 0);

		//create triangle is i > 1
		if (i >= 1)
		{
			rkMesh.m_aiIndices.push_back(0);
			rkMesh.m_aiIndices.push_back(iVerticeIndex - 1);
			rkMesh.m_aiIndices.push_back(iVerticeIndex);
		}
	}

	//create LastTriangle is i > 1
	rkMesh.m_aiIndices.push_back(0);
	rkMesh.m_aiIndices.push_back(iVerticeIndex - 1);
	rkMesh.m_aiIndices.push_back(iVerticeIndex - riNbSubdiv);


	//Create the remaing stages
	vmath::vec3 kOriginP = rkMesh.m_akVertices[0];
	fCurrentZ += fSpaceBetweenStages;
	for (I32 j = 0; j < riNbStages - 1; ++j, fCurrentZ += fSpaceBetweenStages)
	{
		if (fCurrentZ > 1)
		{
			fCurrentZ = 1;
		}

		fCurrentAngle = 0;
		vmath::vec3 kOrigin;
		vmath::mat3 kMat;
		rkZCurve.EvaluatePositionAndSpace(fCurrentZ, kOrigin, kMat);

		fLength += vmath::length(kOrigin - kOriginP);
		kOriginP = kOrigin;

		rkXScale.EvaluatePosition(fCurrentZ, fXScale);
		rkYScale.EvaluatePosition(fCurrentZ, fYScale);
		for (I32 i = 0; i < riNbSubdiv; ++i, ++iVerticeIndex, fCurrentAngle += fAngleRadians)
		{
			//create vertice
			rkMesh.m_akVertices[iVerticeIndex] = vmath::vec3(cos(fCurrentAngle), sin(fCurrentAngle), 0);
			rkMesh.m_akNormals[iVerticeIndex] = vmath::normalize(rkMesh.m_akVertices[iVerticeIndex] * kMat);
			rkMesh.m_akVertices[iVerticeIndex][0] *= fXScale;
			rkMesh.m_akVertices[iVerticeIndex][1] *= fYScale;
			rkMesh.m_akVertices[iVerticeIndex] = rkMesh.m_akVertices[iVerticeIndex] * kMat + kOrigin;


			//rpkBufferData[iVerticeIndex] = vmath::vec3(XScale * randX * cos(fCurrentAngle), YScale * randY * sin(fCurrentAngle), fZScale*fCurrentZ);
			rkMesh.m_akUVs[iVerticeIndex] = vmath::vec2((F32)i / (F32)(riNbSubdiv - 1), fCurrentZ);
			//rpkNormalData[iVerticeIndex] = vmath::normalize(rpkBufferData[iVerticeIndex] - kOrigin);

			//create triangle 1
			rkMesh.m_aiIndices.push_back(iVerticeIndex - riNbSubdiv);
			rkMesh.m_aiIndices.push_back(iVerticeIndex);
			rkMesh.m_aiIndices.push_back(iVerticeIndex - riNbSubdiv + 1);

			//create triangle 2 only if i > 1
			if (i >= 1)
			{
				rkMesh.m_aiIndices.push_back(iVerticeIndex - riNbSubdiv);
				rkMesh.m_aiIndices.push_back(iVerticeIndex - 1);
				rkMesh.m_aiIndices.push_back(iVerticeIndex);
			}
		}

		//create LastTriangle is i > 1
		rkMesh.m_aiIndices.push_back(iVerticeIndex - 2 * riNbSubdiv);
		rkMesh.m_aiIndices.push_back(iVerticeIndex - riNbSubdiv - 1);
		rkMesh.m_aiIndices.push_back(iVerticeIndex - riNbSubdiv);
	}

	//create the last surface
	for (I32 i = 1; i < riNbSubdiv; ++i)
	{

		rkMesh.m_aiIndices.push_back(iNbVertices - i - 2);
		rkMesh.m_aiIndices.push_back(iNbVertices - 1);
		rkMesh.m_aiIndices.push_back(iNbVertices - i - 1);
	}

	//create LastTriangle is i > 1
	rkMesh.m_aiIndices.push_back(iNbVertices - 2);
	rkMesh.m_aiIndices.push_back(iNbVertices - 1);
	rkMesh.m_aiIndices.push_back(iNbVertices - riNbSubdiv - 1);
}

I32 GetSeedForBuilding()
{
	static std::default_random_engine skRNG(0);
	return skRNG();	
}
