#pragma once
class OGLTexture2D
{
public:
	OGLTexture2D();
	~OGLTexture2D();

	void Initialise();
	void InitialiseStorage(I32 iWidth, I32 iHeight, I32 iLevels, GLenum eSamplerFormat);
	void SetData(I32 iLevel, I32 iXOffset, I32 iYOffset, 
		I32 iWidth, I32 iHeight, 
		GLenum eDataFormat, GLenum eDataType,
		void * data,
		bool bGenerateMipMaps);

	void Bind();

	void Delete();

	operator GLuint() const { return m_hiTexture; }

private:
	GLuint m_hiTexture;
	
	//texture size
	I32 m_iWidth;
	I32 m_iHeight;
	I32 m_iNbLevels;

	bool m_bIsStorageInitialised;
	
	GLenum m_eSamplerFormat;
};

