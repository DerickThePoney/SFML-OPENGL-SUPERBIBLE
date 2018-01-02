#pragma once

class Line
{
public:
	Line() :m_kOrigin(vmath::vec3(0)), m_kDirection(vmath::vec3(1, 1, 1)), m_bInit(false) {}
	Line(const vmath::vec3& rkOrigin, const vmath::vec3& rkLine) : m_kOrigin(rkOrigin), m_kDirection(rkLine), m_bInit(false)
	{}

	void InitDraw()
	{
		//create vao
		glGenVertexArrays(1, &m_hiVao);
		glBindVertexArray(m_hiVao);

		glGenBuffers(1, &m_hiBufferData);
		m_bInit = true;
	}

	void Draw(float fExtent)
	{
		if (!m_bInit) InitDraw();
		glBindVertexArray(m_hiVao);

		const vmath::vec3 kArray[] = { m_kOrigin - fExtent * m_kDirection, m_kOrigin + fExtent * m_kDirection };
		glBindBuffer(GL_ARRAY_BUFFER, m_hiBufferData);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vmath::vec3), kArray, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_LINES, 0, 2);
	}

	void SetOrigin(const vec3& kOrigin) { m_kOrigin = kOrigin; };
	void SetDirection(const vec3& kDirection) { m_kDirection = kDirection; };

	const vec3& GetOrigin() const { return m_kOrigin; };
	const vec3& GetDirection() const { return m_kDirection; };

private:
	GLuint m_hiVao;
	GLuint m_hiBufferData;
	GLuint m_hiProgram;
	bool m_bInit;
	vmath::vec3 m_kOrigin;
	vmath::vec3 m_kDirection;
};
