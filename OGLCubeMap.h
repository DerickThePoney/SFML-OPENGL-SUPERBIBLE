#pragma once

class OGLCubeMap
{
public:
	OGLCubeMap();
	~OGLCubeMap();

	void Initialise();
	void InitialiseFromRessource(const std::string& pathName);
	void Bind();

	void Delete();

private:
	std::string GetFilenameFromIndex(I32 index);


	GLuint m_hiTexture;

	//texture size
	I32 m_iWidth;
	I32 m_iHeight;
	I32 m_iNbLevels;

	bool m_bIsStorageInitialised;
};