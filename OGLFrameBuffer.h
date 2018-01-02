#pragma once

class OGLFrameBuffer
{
public:
	OGLFrameBuffer();
	~OGLFrameBuffer();

	void Init();
	void Delete();
	void Bind();
	void UnBind();

private:
	GLuint m_hiFrameBuffer;
};

