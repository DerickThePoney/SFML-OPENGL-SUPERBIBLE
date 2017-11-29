#pragma once
class OGLTexture
{
public:
	OGLTexture();
	~OGLTexture();

	void Initialise();

	void InitialiseStorage(GLenum eTextureTarget, I32 iWidth, I32 iSize, GLenum eSamplerFormat);


	void Delete();

private:
	GLuint m_hiTexture;
	
	//texture size
	I32 m_iWidth;
	I32 m_iSize;
	
	//
	GLenum m_eTextureType;
	GLenum m_eSamplerFormat;
};

