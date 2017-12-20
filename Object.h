#pragma once

//#include "vmath.h"
#include "Utility.h"
#include "Intersection.h"
#include "BezierSpline.h"
#include "Mesh.h"
#include "BMesh.h"


class Object
{
public:
	Object();
	~Object();

	void Initialise();
	void Update(double deltaTime);
	void Render();
	void Terminate();

	void Translate(const vmath::vec3& kMovement);
	void AddRotation(const vmath::vec3& kAddRotation);
	vmath::mat4 GetMoveMatrix();

	GLuint m_hiBlackTexture;
	GLuint m_kLineProgram;
	GLuint m_kProjMatLocation;
	vmath::mat4 m_kProjMat;

private:
	/*GLuint m_hiVao;
	GLuint m_hiDataBuffer;
	GLuint m_hiNormalBuffer;
	GLuint m_hiHitBuffer;
	GLuint m_hiIndexBuffer;
	GLuint m_hiUVsBuffer;*/

	Mesh* m_pkMesh;
	
	UI32 m_uiSize;
	

	vmath::vec3 m_kRotation;
	vmath::vec3 m_kPosition;

	Line m_kLine;
//	GLuint m_kLineProgram;

};


static void CreateCircleColumn(const I32 & riNbSubdiv, 
	const I32 & riNbStages, 
	vmath::vec3 *& rpkBufferData, 
	vmath::vec3 *& rpkNormalData,
	vmath::vec2 *& rpkUVS,
	std::vector<UI32>& raiIndices, 
	I32 & iNbVertices,
	const BezierCurve3D& rkZCurve,
	const BezierSpline<F32>& rkXScale,
	const BezierSpline<F32>& rkYScale);

static void SplinedRevolutionMesh(const I32 & riNbSubdiv,
	const I32 & riNbStages,
	Mesh& rkMesh,
	const BezierCurve3D& rkZCurve,
	const BezierSpline<F32>& rkXScale,
	const BezierSpline<F32>& rkYScale);

I32 GetSeedForBuilding();


