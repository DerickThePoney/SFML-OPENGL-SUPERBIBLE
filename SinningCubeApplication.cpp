#include "stdafx.h"
#include "SinningCubeApplication.h"


SpinningCubeApplication::SpinningCubeApplication()
{
}


SpinningCubeApplication::~SpinningCubeApplication()
{
}

void SpinningCubeApplication::Initialise()
{
	//load program
	LoadAndCompileProgram();

	//create vao
	glGenVertexArrays(1, &m_hiVao);
	glBindVertexArray(m_hiVao);

	//data to put in buffer
	static const GLfloat vertex_positions[] =
	{
		-0.25f,  0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		0.25f, -0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f, -0.25f,

		0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,
		0.25f,  0.25f, -0.25f,

		0.25f, -0.25f,  0.25f,
		-0.25f, -0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
		0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f,  0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f, -0.25f,
		-0.25f,  0.25f, -0.25f,
		-0.25f,  0.25f,  0.25f,

		-0.25f, -0.25f,  0.25f,
		0.25f, -0.25f,  0.25f,
		0.25f, -0.25f, -0.25f,

		0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f, -0.25f,
		-0.25f, -0.25f,  0.25f,

		-0.25f,  0.25f, -0.25f,
		0.25f,  0.25f, -0.25f,
		0.25f,  0.25f,  0.25f,

		0.25f,  0.25f,  0.25f,
		-0.25f,  0.25f,  0.25f,
		-0.25f,  0.25f, -0.25f
	};

	//data buffer
	glGenBuffers(1, &m_hiBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_hiBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertex_positions),
		vertex_positions,
		GL_STATIC_DRAW);

	//set it as attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);


	//viewport and initial viewmatrix
	OnResize(m_window.getSize().x, m_window.getSize().y);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
}

void SpinningCubeApplication::Render(double currentTime)
{
	//compute matrix
	float f = (float)currentTime*0.0001f;// +(float)M_PI + 0.1f;
	vmath::mat4 moveMatrix =
		vmath::translate(0.0f, 0.0f, -6.0f) *
		vmath::rotate((float)currentTime * 45.0f*0.001f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate((float)currentTime * 21.0f*0.001f, 1.0f, 0.0f, 0.0f) *
		vmath::translate(sinf(2.1f * f) * 2.0f,
			cosf(1.7f * f) * 2.0f,
			sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);

	static const GLfloat one = 1.0f;
	//draw
	//clear buffer
	glClearBufferfv(GL_COLOR, 0, vmath::vec4(0.0f, 0.25f, 0.0f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, &one);

	//use program
	glUseProgram(m_hiProgram);

	//set modelview and proj matrix
	glUniformMatrix4fv(m_hiMoveMatrixUniformLocation, 1, GL_FALSE, moveMatrix);
	glUniformMatrix4fv(m_hiProjMatrixUniformLocation, 1, GL_FALSE, m_kProjectionMatrice);

	//draw call
	glDrawArrays(GL_TRIANGLES, 0, 36);

	for (int i = 1; i < 24; i++)
	{
		float f = (float)i + (float)currentTime * 0.0001f;
		vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -6.0f) *
			vmath::rotate((float)currentTime * 45.0f*0.001f, 0.0f, 1.0f, 0.0f) *
			vmath::rotate((float)currentTime * 21.0f*0.001f, 1.0f, 0.0f, 0.0f) *
			vmath::translate(sinf(2.1f * f) * 2.0f,
				cosf(1.7f * f) * 2.0f,
				sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);
		glUniformMatrix4fv(m_hiMoveMatrixUniformLocation, 1, GL_FALSE, mv_matrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	Application::Render(currentTime);
}



void SpinningCubeApplication::Terminate()
{

	glDeleteVertexArrays(1, &m_hiVao);
	glDeleteProgram(m_hiProgram);
}

void SpinningCubeApplication::OnResize(unsigned int width, unsigned int height)
{
	Application::OnResize(width, height);
	m_kProjectionMatrice = vmath::perspective(50.0f,
		(float)width / (float)height,
		0.1f,
		1000.0f);
}

void SpinningCubeApplication::LoadAndCompileProgram()
{
	GLuint ahiShaders[2];// hiVertexShader, hiFragmentShader;

	ahiShaders[0] = LoadShader("media/shaders/SpinnyCube.vert.glsl", GL_VERTEX_SHADER, true);
	ahiShaders[1] = LoadShader("media/shaders/SpinnyCube.frag.glsl", GL_FRAGMENT_SHADER, true);


	m_hiProgram = LinkProgram(ahiShaders,2,true, true);// glCreateProgram();
	/*glAttachShader(m_hiProgram, hiVertexShader);
	glAttachShader(m_hiProgram, hiFragmentShader);
	glLinkProgram(m_hiProgram);*/

	//delete vertex and fragment shaders
	//glDeleteShader(hiVertexShader);
	//glDeleteShader(hiFragmentShader);

	//get the unforms
	m_hiMoveMatrixUniformLocation = glGetUniformLocation(m_hiProgram, "mv_matrix");
	m_hiProjMatrixUniformLocation = glGetUniformLocation(m_hiProgram, "proj_matrix");
}
