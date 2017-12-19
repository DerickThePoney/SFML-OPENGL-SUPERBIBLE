#pragma once
#include "OGLUtilities.h"
#include "OGLShader.h"

struct ActiveProgramInformations
{
	ActiveProgramInformations(): m_pcName(nullptr), m_bIsFromBlock(false) {}
	GLsizei m_iLength;
	GLint m_iSize;
	GLenum m_eType;
	GLchar* m_pcName;
	bool m_bIsFromBlock;
};


struct ActiveUniformBlockInformation
{
	ActiveUniformBlockInformation() : m_pcName(nullptr) {}
	GLsizei m_iLength;
	GLchar* m_pcName;
	GLint m_iNbUniform;
	GLint* m_piUniformsIndexes;
};


class OGLProgram
{
public:
	OGLProgram();
	~OGLProgram();

	OGLProgram(const OGLProgram& other) = delete;
	OGLProgram(OGLProgram&& other) = delete;
	
	void operator = (const OGLProgram& other) = delete;
	void operator = (OGLProgram&& other) = delete;

	bool LinkProgram(OGLShader * uiShaders, int iShaderCount, bool bDeleteShaders);

	void UseProgram();
	void DeleteProgram();

	operator GLuint() const { return m_hProgram; }

	const std::vector<ActiveProgramInformations>& GetVectorAttributesInformation();
	const std::vector<ActiveProgramInformations>& GetUniformsInformation();
	const std::vector<ActiveUniformBlockInformation>& GetUniformBlocksInformation();

	void InspectUniformProgramInformation(const ActiveProgramInformations& info, I32 index);

private:
	void ExtractInformation();

private:
	GLuint m_hProgram;
	std::vector<ActiveProgramInformations> m_akAttributesInfo;
	std::vector<ActiveProgramInformations> m_akUniformsInfo;
	std::vector<ActiveUniformBlockInformation> m_akUniformBlockInfo;
};


