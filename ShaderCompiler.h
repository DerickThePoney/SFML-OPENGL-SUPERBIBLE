#pragma once

struct ShaderInformation
{
	std::string m_kShaderCode;
	GLenum m_eShaderType;
};

class ShaderCompiler
{
public:
	static ShaderCompiler instance;
	
	bool RetrieveShaders(std::vector<ShaderInformation>& akShaderInformation, const std::string& kFilename);

private:
	ShaderCompiler();
	~ShaderCompiler();

	char Advance();
	char Peek();
	void AdvanceToNext(char c);
	bool IsAtEnd();
	std::string NextWord();
	void Rewind(int size);

	bool ParseNextShader(ShaderInformation& kInfo);

	int m_iCurrent;
	std::string m_kFileSource;
};

