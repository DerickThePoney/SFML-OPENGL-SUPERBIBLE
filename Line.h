#pragma once
#include "Camera.h"
class Line
{
public:
	Line() :m_kOrigin(vmath::vec4(0,0,0,1)), m_kDirection(vmath::vec4(1, 1, 1, 0)), m_bInit(false) {}
	Line(const vmath::vec4& rkOrigin, const vmath::vec4& rkLine) : m_kOrigin(rkOrigin), m_kDirection(rkLine), m_bInit(false)
	{}

	~Line() 
	{
		m_kVertices.Delete();
		m_kIndices.Delete();
		m_hkVao.Delete();
	}

	void InitDraw()
	{
		//create vao
		m_hkVao.Init();
		m_hkVao.Bind();

		m_kVertices.Init(GL_ARRAY_BUFFER, 2 * sizeof(vec4), NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
		m_hkVao.SetAttribute(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		
		//UI32 indices[2] = { 0,1 };
		//m_kIndices.Init(GL_ELEMENT_ARRAY_BUFFER, 2 * sizeof(UI32), indices, 0);
		//m_bInit = true;
	}

	void Draw(float fExtent, Camera& camera)
	{
		if (!m_bInit) InitDraw();
		m_hkVao.Bind();

		//mat4 lookAt = camera.GetLookAt();
		//mat4 prj = camera.GetProjection();
		vec4 start = m_kOrigin;
		//start = prj * start;
		//start /= start[3];
		vec4 end = m_kOrigin + m_kDirection;
		//end = prj * end;
		//end /= end[3];
		//end /= end[3];

		vmath::vec4 kArray[2] = { start, end };
		m_kVertices.UpdateData(GL_ARRAY_BUFFER, 2 * sizeof(vec4), kArray, GL_WRITE_ONLY);
		//glBindBuffer(GL_ARRAY_BUFFER, m_hiBufferData1);
		/*vec4* kArray = (vec4*)m_kVertices.Map(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		kArray[0] = start;
		kArray[1] = end;
		m_kVertices.Unmap(GL_ARRAY_BUFFER);*/
		//glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(vmath::vec4), kArray);

		//glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_LINES, 0, 2);
	}

	void SetOrigin(const vec4& kOrigin) { m_kOrigin = kOrigin; };
	void SetDirection(const vec4& kDirection) { m_kDirection = kDirection; };

	const vec4& GetOrigin() const { return m_kOrigin; };
	const vec4& GetDirection() const { return m_kDirection; };

private:
	OGLVertexArray m_hkVao;
	OGLBuffer m_kVertices;
	OGLBuffer m_kIndices;
	bool m_bInit;
	vmath::vec4 m_kOrigin;
	vmath::vec4 m_kDirection;
};
