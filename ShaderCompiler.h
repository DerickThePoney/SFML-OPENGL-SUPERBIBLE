#pragma once

struct ShaderInformation
{
	std::string m_kShaderCode;
	GLenum m_eShaderType;
};

struct InterpolatorInformation
{
	std::string m_kInterpolatorCode;
};

struct UniformsInformation
{
	std::string m_kUniformsCode;
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
	std::string NextLine();
	void Rewind(int size);

	I32 ParseNextShader(ShaderInformation& kShInfo, InterpolatorInformation& kIInfo, UniformsInformation& kUInfo);

	int m_iCurrent;
	std::string m_kFileSource;
};

