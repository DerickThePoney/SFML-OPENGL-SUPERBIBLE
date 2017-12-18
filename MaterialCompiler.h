#pragma once

struct MaterialInformation
{
	std::string m_kShaderCode;
	GLenum m_eShaderType;
};

class MaterialCompiler
{
public:
	static MaterialCompiler instance;
	
	bool RetrieveShaders(std::vector<MaterialInformation>& akShaderInformation, const std::string& kFilename);

private:
	MaterialCompiler();
	~MaterialCompiler();

	char Advance();
	char Peek();
	void AdvanceToNext(char c);
	bool IsAtEnd();

	bool ParseNextShader(MaterialInformation& kInfo);

	int m_iCurrent;
	std::string m_kFileSource;
};

