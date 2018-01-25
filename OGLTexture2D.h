#pragma once


class OGLTexture2D
{
public:
	OGLTexture2D();
	~OGLTexture2D();

	void Initialise();
	void InitialiseStorage(I32 iWidth, I32 iHeight, I32 iLevels, GLenum eSamplerFormat, bool bIsMultisample = false);
	void SetData(I32 iLevel, I32 iXOffset, I32 iYOffset, 
		I32 iWidth, I32 iHeight, 
		GLenum eDataFormat, GLenum eDataType,
		void * data,
		bool bGenerateMipMaps);

	void InitialiseFromRessource(const std::string& kFilename);

	void Bind(GLint iUnit = 0);

	void Delete();

	operator GLuint() const { return m_hiTexture; }

	ivec2 GetTextureSize() const { return ivec2(m_iWidth, m_iHeight); }

	void Inspect();

	void SetSamplingParameters();

private:
	GLuint m_hiTexture;
	
	//texture size
	I32 m_iWidth;
	I32 m_iHeight;
	I32 m_iNbLevels;

	bool m_bIsStorageInitialised;
	bool m_bIsMultisample;
	
	GLenum m_eSamplerFormat;

	GLenum m_eTextureFilter;
	GLenum m_eWrapMode;
	//GLenum m_eMinFilter;
};

