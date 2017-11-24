#pragma once


class OGLBuffer
{
public:
	OGLBuffer(): m_iSize(0) {}

	//Data handling
	void Init(GLenum eTarget, I32 iSize, void* pData, GLbitfield eFlags);
	void UpdateData(GLenum eTarget, I32 iSize, void* pData, GLbitfield eFlags);
	void Delete();

	//other calls
	void Bind(GLenum eTarget);

	operator GLuint() { return m_hiBuffer; }
	const UI32& GetSize() const { return m_iSize; }
private:
	GLuint m_hiBuffer;
	UI32 m_iSize;
	GLbitfield m_eFlags;
};
