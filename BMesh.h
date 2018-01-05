#pragma once
#include "Utility.h"
//#include "vmath.h"
#include <map>
#include <vector>
#include <list>
#include <set>
#include "Mesh.h"
#include <random>
//using namespace vmath;

typedef vec4 Vertice;
typedef std::vector<Vertice*> VerticesPVector;
typedef std::vector<Vertice> VerticesVector;
typedef std::map<I32, Vertice> VerticesMap;
typedef std::map<I32, Vertice*> VerticesPMap;
class BMesh;

class Edge
{
public:
	Edge(Vertice* pkV1, Vertice* pkV2):m_pkVertice1(pkV1), m_pkVertice2(pkV2) {}
	Vertice* m_pkVertice1;
	Vertice* m_pkVertice2;
};

typedef std::map<I32, Edge> EdgesMap;
typedef std::map<I32, Edge*> EdgesPMap;

class Face
{
public:
	vec4 GetNormal(const BMesh& rkMesh);
	mat4 GetTransform(const BMesh& rkMesh);
	vec4 GetCenter(const BMesh& rkMesh);
	F32 GetAspectRatio(const BMesh& rkMesh);
	std::vector<I32> m_apkVerticesMap;
	EdgesPMap m_apkEdgesMap;
	vec4 m_kColor;
};

typedef std::map<I32, Face> FacesMap;
typedef std::map<I32, Face*> FacesPMap;

class BMesh
{
public:
	VerticesVector m_akVertices;
	//EdgesMap m_akEdges;
	FacesMap m_akFaces;

	void BuildMesh(Mesh& rkMesh);
};

class BMeshOperation
{
public:
	virtual void Apply(BMesh& rkMesh) = 0;
};

class ExtrudeFace : public BMeshOperation
{
public:
	ExtrudeFace(I32 iFaceID, F32 fScaleFactor) : m_iFaceID(iFaceID), m_fScaleFactor(fScaleFactor) {}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkBMesh) override;

private:
	I32 m_iFaceID;
	F32 m_fScaleFactor;
};

class ScaleFace : public BMeshOperation
{
public:
	ScaleFace(I32 iFaceID, F32 fScaleFactorX, F32 fScaleFactorY) : m_iFaceID(iFaceID), m_fScaleFactorX(fScaleFactorX), m_fScaleFactorY(fScaleFactorY) {}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkMesh) override;

private:
	I32 m_iFaceID;
	F32 m_fScaleFactorX;
	F32 m_fScaleFactorY;
};

class RotateFace : public BMeshOperation
{
public:
	RotateFace(I32 iFaceID, F32 fRotationX, F32 fRotationY, F32 fRotationZ) : m_iFaceID(iFaceID), m_fRotationX(fRotationX), m_fRotationY(fRotationY), m_fRotationZ(fRotationZ) {}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkMesh) override;

private:
	I32 m_iFaceID;
	F32 m_fRotationX;
	F32 m_fRotationY;
	F32 m_fRotationZ;
};

class RibExtrusion : public BMeshOperation
{
public:
	RibExtrusion(I32 iFaceID, F32 fScaleFactor, I32 iNbRibs) : m_iFaceID(iFaceID), m_fScaleFactor(fScaleFactor), m_iNbRibs(iNbRibs) {}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkMesh) override;

private:
	I32 m_iFaceID;
	F32 m_fScaleFactor;
	I32 m_iNbRibs;
};

class SubdivideFace : public BMeshOperation
{
public:
	SubdivideFace(I32 iFaceID, I32 iNbSubDiv, std::list<I32>* apkSubdivFacesID, std::set<I32>* apkNonFixedVertices = nullptr, I32 iSubDivLevel = 0)
				: m_iFaceID(iFaceID), m_iNbSubdiv(iNbSubDiv), m_apkNonFixedVertices(apkNonFixedVertices),
				m_apkSubdivFacesID(apkSubdivFacesID), m_iSubDivLevel(iSubDivLevel)
	{}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkMesh) override;

private:
	I32 m_iFaceID;
	I32 m_iNbSubdiv;
	I32 m_iSubDivLevel;
	std::set<I32>* m_apkNonFixedVertices;
	std::list<I32>* m_apkSubdivFacesID;
};

class SpherifyFace : public BMeshOperation
{
public:
	SpherifyFace(I32 iFaceID, F32 fSphereCenter, F32 fSphereRadius, I32 iSubdivSize)
		: m_iFaceID(iFaceID), m_fSphereCenter(fSphereCenter), m_fSphereRadius(fSphereRadius),
		m_iSubdivSize(iSubdivSize)
	{}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkMesh) override;

private:
	I32 m_iFaceID;
	F32 m_fSphereCenter;
	F32 m_fSphereRadius;
	I32 m_iSubdivSize;
};


class ExhaustPipe : public BMeshOperation
{
public:
	ExhaustPipe(I32 iFaceID, F32 fTinyScaling, F32 fScaleFactorIn, F32 fExtrudeLength)
		: m_iFaceID(iFaceID), m_fTinyScaling(fTinyScaling), m_fScaleFactorIn(fScaleFactorIn),
		m_fExtrudeLength(fExtrudeLength)
	{}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkMesh) override;

private:
	I32 m_iFaceID;
	F32 m_fTinyScaling;
	F32 m_fScaleFactorIn;
	F32 m_fExtrudeLength;
};


class TranslateFace : public BMeshOperation
{
public:
	TranslateFace(I32 iFaceID, vec4 kScaleFactor) : m_iFaceID(iFaceID), m_kScaleFactor(kScaleFactor) {}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkMesh) override;

private:
	I32 m_iFaceID;
	vec4 m_kScaleFactor;
};

class MakeCube : public BMeshOperation
{
public:
	MakeCube(vec3 kScale = vec3(1), mat4 kPositionMatrix = mat4::identity()) 
		: m_kScale(kScale), m_kPositionMatrix(kPositionMatrix) {}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkMesh) override;

private:
	vec3 m_kScale;
	mat4 m_kPositionMatrix;
};


class MakeSpaceShip : public BMeshOperation
{
public:
	MakeSpaceShip(I32 seed = 523648936, const I32& iMinHullExtrusions = 3, const I32& iMaxHullExtrusions = 6, const I32& iMinAsymetrySegments = 1, const I32& iMaxAsymetrySegments = 5)
		: m_iSeed(seed) , m_iMinHullExtrusions(iMinHullExtrusions), m_iMaxHullExtrusions(iMaxHullExtrusions),
		m_iMinAsymetrySegmentsExtrusions(iMaxAsymetrySegments), m_iMaxAsymetrySegmentsExtrusions(iMaxAsymetrySegments)
	{
		m_kRng.seed(m_iSeed);
	}

	// Hérité via BMeshOperation
	virtual void Apply(BMesh & rkMesh) override;

private:
	void MakeEngines(BMesh& rkMesh, const std::list<I32>& akEngineFaces);
	void MakeGrids(BMesh& rkMesh, const std::list<I32>& akGridFaces);
	void MakeSpheres(BMesh& rkMesh, const std::list<I32>& akGridFaces);
private:
	I32 m_iSeed;
	std::mt19937 m_kRng;
	std::uniform_real_distribution<F32> m_kDist;
	
	I32 m_iMinHullExtrusions;
	I32 m_iMaxHullExtrusions;
	I32 m_iMinAsymetrySegmentsExtrusions;
	I32 m_iMaxAsymetrySegmentsExtrusions;
};

class MakeSpaceShip2 : public BMeshOperation
{
public:
	virtual void Apply(BMesh & rkMesh) override;
};