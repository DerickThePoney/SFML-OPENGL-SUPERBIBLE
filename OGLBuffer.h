#pragma once


class OGLBuffer
{
public:
	OGLBuffer(): m_iSize(0) {}
	OGLBuffer(GLuint hiBuffer);

	//Data handling
	void Init(GLenum eTarget, I32 iSize, void* pData, GLbitfield eFlags);
	void UpdateData(GLenum eTarget, I32 iSize, void* pData, GLbitfield eFlagsNormal, GLbitfield eFlagsReset = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
	void Delete();

	//other calls
	void Bind(GLenum eTarget);
	void BindToUniformBindingPoint(GLint iBindingIndex);
	void* Map(GLenum eTarget, GLenum eFlags);
	void Unmap(GLenum eTarget);

	operator GLuint() { return m_hiBuffer; }
	const UI32& GetSize() const { return m_iSize; }

private:
	void InitFromHandle();
private:
	GLuint m_hiBuffer;
	UI32 m_iSize;
	GLbitfield m_eFlags;
};
