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
		glDeleteBuffers(2, m_hiBufferData);
	}

	void InitDraw()
	{
		//create vao
		glGenVertexArrays(1, &m_hiVao);
		glBindVertexArray(m_hiVao);

		glGenBuffers(2, m_hiBufferData);
		m_bInit = true;
	}

	void Draw(float fExtent, Camera& camera)
	{
		if (!m_bInit) InitDraw();
		glBindVertexArray(m_hiVao);

		mat4 lookAt = camera.GetLookAt();
		mat4 prj = camera.GetProjection();
		vec4 start = m_kOrigin;
		//start = prj * start;
		//start /= start[3];
		vec4 end = m_kOrigin + m_kDirection;
		//end = prj * end;
		//end /= end[3];
		//end /= end[3];

		vmath::vec4 kArray[] = { start, end };
		UI32 indices[] = { 0,1 };
		glBindBuffer(GL_ARRAY_BUFFER, m_hiBufferData[0]);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vmath::vec4), kArray, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hiBufferData[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * sizeof(UI32), indices, GL_DYNAMIC_DRAW);

		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	}

	void SetOrigin(const vec4& kOrigin) { m_kOrigin = kOrigin; };
	void SetDirection(const vec4& kDirection) { m_kDirection = kDirection; };

	const vec4& GetOrigin() const { return m_kOrigin; };
	const vec4& GetDirection() const { return m_kDirection; };

private:
	GLuint m_hiVao;
	GLuint m_hiBufferData[2];
	GLuint m_hiProgram;
	bool m_bInit;
	vmath::vec4 m_kOrigin;
	vmath::vec4 m_kDirection;
};
