#pragma once

class OGLCubeMap
{
public:
	OGLCubeMap();
	~OGLCubeMap();

	void Initialise();
	void InitialiseFromRessource(const std::vector<std::string>& files);
	void Bind();

	void Delete();


	static std::string GetFilenameFromIndex(I32 index);

private:
	GLuint m_hiTexture;

	//texture size
	I32 m_iWidth;
	I32 m_iHeight;
	I32 m_iNbLevels;

	bool m_bIsStorageInitialised;
};