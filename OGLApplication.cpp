#include "stdafx.h"
#include "OGLApplication.h"


OGLApplication::OGLApplication()
{
}


OGLApplication::~OGLApplication()
{
}

void OGLApplication::Render(double currentTime)
{
	// clear the buffers
	GLfloat red[] = { 1.0f,
		0.0f,
		0.0f,
		1.0f };

	GLfloat green[] = { 0.0f,
		0.25f,
		0.0f,
		1.0f };
	/*GLfloat red[] = { (float)cos(currentTime) * 0.5f + 0.5f,
		(float)sin(currentTime) * 0.5f + 0.5f,
		0.0f,
		1.0f };*/

	glClearBufferfv(GL_COLOR, 0, green);

	glUseProgram(m_iProgram);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	Application::Render(currentTime);
}

void OGLApplication::Initialise()
{
	m_iProgram = CompileShaders();
	glCreateVertexArrays(1, &m_iVertexArrayObject);
	glBindVertexArray(m_iVertexArrayObject);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glPointSize(40.0f);
}

void OGLApplication::Terminate()
{
	glDeleteVertexArrays(1, &m_iVertexArrayObject);
	glDeleteProgram(m_iProgram);
}

GLuint OGLApplication::CompileShaders()
{
	GLuint iProgram;

	//source for vertex shader
	struct {
		GLuint vs;
		GLuint tcs;
		GLuint tes;
		GLuint fs;
	} shaders;

	shaders.vs = LoadShader("media/shaders/triangle.vert.glsl", GL_VERTEX_SHADER, true);
	//shaders.tcs = LoadShader("media/shaders/triangle.tcs.glsl", GL_TESS_CONTROL_SHADER, true);
	//shaders.tes = LoadShader("media/shaders/triangle.tes.glsl", GL_TESS_EVALUATION_SHADER, true);
	shaders.fs = LoadShader("media/shaders/triangle.frag.glsl", GL_FRAGMENT_SHADER, true);

	

	//create program
	iProgram = glCreateProgram();
	glAttachShader(iProgram, shaders.vs);
	//glAttachShader(iProgram, shaders.tcs);
	//glAttachShader(iProgram, shaders.tes);
	glAttachShader(iProgram, shaders.fs);
	glLinkProgram(iProgram);

	//delete vertex and fragment shaders
	glDeleteShader(shaders.vs);
	//glDeleteShader(shaders.tcs);
	//glDeleteShader(shaders.tes);
	glDeleteShader(shaders.fs);

	return iProgram;
}


GLuint Application::LoadShader(const char * pcFilename, GLenum eShaderType, bool bCheckErrors)
{
	GLuint result = 0;
	FILE * fp;
	size_t filesize;
	char * data;

	fp = fopen(pcFilename, "rb");

	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	data = new char[filesize + 1];

	if (!data)
		goto fail_data_alloc;

	fread(data, 1, filesize, fp);
	data[filesize] = 0;
	fclose(fp);

	result = glCreateShader(eShaderType);

	if (!result)
		goto fail_shader_alloc;

	glShaderSource(result, 1, &data, NULL);

	delete[] data;

	glCompileShader(result);

	if (bCheckErrors)
	{
		GLint status = 0;
		glGetShaderiv(result, GL_COMPILE_STATUS, &status);

		if (!status)
		{
			char buffer[4096];
			glGetShaderInfoLog(result, 4096, NULL, buffer);
#ifdef _WIN32
			OutputDebugStringA(pcFilename);
			OutputDebugStringA(":");
			OutputDebugStringA(buffer);
			OutputDebugStringA("\n");
#else
			fprintf(stderr, "%s: %s\n", pcFilename, buffer);
#endif
			goto fail_compile_shader;
		}
	}

	return result;

fail_compile_shader:
	glDeleteShader(result);

fail_shader_alloc:;
fail_data_alloc:
	return result;
}

GLuint Application::LinkProgram(const GLuint * uiShaders, int iShaderCount, bool bDeleteShaders, bool bCheckErrors)
{
	int i;

	GLuint uiProgram;

	uiProgram = glCreateProgram();

	for (i = 0; i < iShaderCount; i++)
	{
		glAttachShader(uiProgram, uiShaders[i]);
	}

	glLinkProgram(uiProgram);

	if (bCheckErrors)
	{
		GLint status;
		glGetProgramiv(uiProgram, GL_LINK_STATUS, &status);

		if (!status)
		{
			char buffer[4096];
			glGetProgramInfoLog(uiProgram, 4096, NULL, buffer);
#ifdef _WIN32
			OutputDebugStringA(buffer);
			OutputDebugStringA("\n");
#endif
			glDeleteProgram(uiProgram);
			return 0;
		}
	}

	if (bDeleteShaders)
	{
		for (i = 0; i < iShaderCount; i++)
		{
			glDeleteShader(uiShaders[i]);
		}
	}

	return uiProgram;
}
