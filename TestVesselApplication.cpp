#include "stdafx.h"
#include "TestVesselApplication.h"


TestVesselApplication::TestVesselApplication()
	: m_bDragMode(false)
{
}


TestVesselApplication::~TestVesselApplication()
{
}

void TestVesselApplication::Render(double currentTime)
{
	vmath::mat4 moveMatrix = vmath::mat4::identity();
	float f = (float)currentTime * 0.0001f;
	/*moveMatrix =
		vmath::translate(0.0f, 0.0f, -6.0f) *
		vmath::rotate((float)currentTime * 45.0f*0.001f, 0.0f, 1.0f, 0.0f) *
		vmath::rotate((float)currentTime * 21.0f*0.001f, 1.0f, 0.0f, 0.0f) *
		vmath::translate(sinf(2.1f * f) * 2.0f,
			cosf(1.7f * f) * 2.0f,
			sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);*/

	moveMatrix = m_kLookAtMatrix * moveMatrix *m_kTest.GetMoveMatrix();

	static const GLfloat one = 1.0f;
	//draw
	//clear buffer
	glClearBufferfv(GL_COLOR, 0, vmath::vec4(0.0f, 0, 0.0f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, &one);

	glBindTexture(GL_TEXTURE_2D, m_hiTextureObject[0]);


	//use program
	glUseProgram(m_hiProgram);

	//set modelview and proj matrix
	glUniformMatrix4fv(m_hiMoveMatrixUniformLocation, 1, GL_FALSE, moveMatrix);
	glUniformMatrix4fv(m_hiProjMatrixUniformLocation, 1, GL_FALSE, m_kProjectionMatrice);

	m_kTest.m_kLineProgram = m_hiProgramLine;
	m_kTest.m_kProjMatLocation = m_hiProjMatrixUniformLocationLine;
	m_kTest.m_kProjMat = m_kProjectionMatrice * moveMatrix;
	m_kTest.Render();

	//draw call
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	Application::Render(currentTime);
}

void TestVesselApplication::Initialise()
{

	//m_kLookAtMatrix = vmath::mat4::identity();
	m_kLookAtMatrix = vmath::lookat(vmath::vec3(-5, 5, -5), vmath::vec3(0, 0, 2.5f), vmath::vec3(0, 1, 0));
	//long w, h;
	//m_hiTextureObject = LoadBMPImageAsTexture("media/texture/test.bmp", w, h);
	GenerateTexture();
	glBindTexture(GL_TEXTURE_2D, m_hiTextureObject[0]);
	//load program
	LoadAndCompileProgram();

	m_kTest.Initialise();
	m_kTest.m_hiBlackTexture = m_hiTextureObject[1];

	/*//create vao
	glGenVertexArrays(1, &m_hiVao);
	glBindVertexArray(m_hiVao);

	//create vertex

	static Cube c;
	/*c.bf = 1;
	c.bb = 2;

	c.tf = 0.4f;
	c.tb = 0.8f;
	
	c.hf = 1;
	c.hb = 1.5f;

	c.lt = 3;
	c.lb = 3.3f;

	c.bf = 0;
	c.bb = 2;

	c.tf = 0;
	c.tb = 0.8f;

	c.hf = 0;
	c.hb = 1.5f;

	c.lt = 3;
	c.lb = 3.3f;


	static const GLfloat vertex_positions[] =
	{
		//back 1
		c.tb*-0.25f,  c.hb*0.25f, c.lt*-0.25f,
		c.bb*-0.25f, c.hb*-0.25f, c.lb*-0.25f,
		c.bb*0.25f, c.hb*-0.25f, c.lb*-0.25f,

		//back 2
		c.bb*0.25f, c.hb*-0.25f, c.lb*-0.25f,
		c.tb*0.25f,  c.hb*0.25f, c.lt*-0.25f,
		c.tb*-0.25f,  c.hb*0.25f, c.lt*-0.25f,

		//right 1
		c.bb*0.25f, c.hb*-0.25f, c.lb*-0.25f,
		c.bf*0.25f, c.hf*-0.25f,c.lb* 0.25f,
		c.tb*0.25f,  c.hb*0.25f,c.lt*-0.25f,

		//right 2
		c.bf*0.25f, c.hf*-0.25f,  c.lb*0.25f,
		c.tf*0.25f, c.hf* 0.25f,  c.lt*0.25f,
		c.tb*0.25f, c.hb* 0.25f, c.lt*-0.25f,

		//front 1
		c.bf*0.25f, c.hf*-0.25f, c.lb* 0.25f,
		c.bf*-0.25f,c.hf* -0.25f, c.lb* 0.25f,
		c.tf*0.25f,  c.hf*0.25f,  c.lt*0.25f,

		//front 2
		c.bf*-0.25f, c.hf*-0.25f, c.lb* 0.25f,
		c.tf*-0.25f, c.hf* 0.25f, c.lt* 0.25f,
		c.tf*0.25f, c.hf* 0.25f, c.lt* 0.25f,

		//left 1
		c.bf*-0.25f, c.hf*-0.25f, c.lb* 0.25f,
		c.bb*-0.25f, c.hb*-0.25f, c.lb*-0.25f,
		c.tf*-0.25f, c.hf* 0.25f, c.lt* 0.25f,

		//left 2
		c.bb*-0.25f, c.hb*-0.25f, c.lb*-0.25f,
		c.tb*-0.25f,  c.hb*0.25f, c.lt*-0.25f,
		c.tf*-0.25f, c.hf* 0.25f, c.lt* 0.25f,

		//bottom 1 
		c.bf*-0.25f, c.hf*-0.25f,  c.lb*0.25f,
		c.bf*0.25f, c.hf*-0.25f,  c.lb*0.25f,
		c.bb*0.25f, c.hb*-0.25f, c.lb*-0.25f,

		//bottom 2
		c.bb*0.25f, c.hb*-0.25f, c.lb*-0.25f,
		c.bb*-0.25f, c.hb*-0.25f, c.lb*-0.25f,
		c.bf*-0.25f, c.hf*-0.25f,  c.lb*0.25f,

		//top 1
		c.tb*-0.25f,  c.hb*0.25f, c.lt*-0.25f,
		c.tb*0.25f,  c.hb*0.25f, c.lt*-0.25f,
		c.tf*0.25f,  c.hf*0.25f,  c.lt*0.25f,

		//top 2
		c.tf*0.25f,  c.hf*0.25f,  c.lt*0.25f,
		c.tf*-0.25f,  c.hf*0.25f,  c.lt*0.25f,
		c.tb*-0.25f,  c.hb*0.25f, c.lt*-0.25f
	};

	static const GLfloat texture_uvs[] =
	{
		//back 1
		1,1,
		1,0,
		0,0,

		//back 2
		0,0,
		0,1,
		1,1,

		//right 1
		1,0,
		0,0,
		1,1,

		//right 2
		0,0,
		0,1,
		1,1,

		//front 1
		1,0,
		0,0,
		1,1,

		//front 2
		0,0,
		0,1,
		1,1,

		//left 1
		1,0,
		0,0,
		1,1,

		//left 2
		0,0,
		0,1,
		1,1,

		//bottom 1 
		0,1,
		1,1,
		1,0,

		//bottom 2
		1,0,
		0,0,
		0,1,

		//top 1
		0,1,
		1,1,
		1,0,

		//top 2
		1,0,
		0,0,
		0,1
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

	//data buffer
	glGenBuffers(1, &m_hiTextureUVS);
	glBindBuffer(GL_ARRAY_BUFFER, m_hiTextureUVS);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(texture_uvs),
		texture_uvs,
		GL_STATIC_DRAW);

	//set it as attribute
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(4);*/


	//viewport and initial viewmatrix
	OnResize(m_window.getSize().x, m_window.getSize().y);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void TestVesselApplication::Terminate()
{
	glDeleteVertexArrays(1, &m_hiVao);
	glDeleteProgram(m_hiProgram);
	glDeleteTextures(2,m_hiTextureObject);
	m_kTest.Terminate();
}

void TestVesselApplication::OnResize(unsigned int width, unsigned int height)
{
	Application::OnResize(width, height);
	m_kProjectionMatrice = vmath::perspective(50.0f,
		(float)width / (float)height,
		0.1f,
		1000.0f);
}

void TestVesselApplication::HandleMessages()
{
	// handle events
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
		{
			m_kTest.Terminate();
			m_kTest.Initialise();
		}
		if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Up||
													event.key.code == sf::Keyboard::Left|| 
													event.key.code == sf::Keyboard::Right|| 
													event.key.code == sf::Keyboard::Down))
		{
			vmath::vec4 kMove4(0);
			vmath::vec3 kMove3(0);
			kMove4[3] = 1;
			if (event.key.code == sf::Keyboard::Up)
			{
				kMove4[1] = 1;
			}
			else if (event.key.code == sf::Keyboard::Down)
			{
				kMove4[1] = -1;
			}

			if (event.key.code == sf::Keyboard::Left)
			{
				kMove4[0] = -1;
			}
			else if (event.key.code == sf::Keyboard::Right)
			{
				kMove4[0] = 1;
			}

			kMove4 = kMove4 * m_kLookAtMatrix;
			kMove3[0] = kMove4[0];
			kMove3[1] = kMove4[1];
			kMove3[2] = kMove4[2];

			m_kTest.Translate(kMove3);
		}
		if (event.type == sf::Event::MouseWheelScrolled)
		{
			if (event.mouseWheel.x < 0)
			{
				m_kLookAtMatrix = m_kLookAtMatrix * vmath::translate(-vmath::normalize(m_kLookAtMatrix[0]) * 1);
			}
			else
			{
				m_kLookAtMatrix = m_kLookAtMatrix * vmath::translate(m_kLookAtMatrix[0] * 1);
			}
			std::cout << event.mouseWheel.delta << "\t" << std::hex << event.mouseWheel.x << "\t" << event.mouseWheel.y << std::endl;
		}
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle)
		{
			m_bDragMode = true;
		}
		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle)
		{
			m_bDragMode = false;
		}
		if (event.type == sf::Event::MouseMoved && m_bDragMode)
		{
			vmath::vec3 kRot4((float)event.mouseMove.x/800.0f*10, (float)event.mouseMove.y/600.0f *10, 0);
			m_kTest.AddRotation(kRot4);
			
		}
		else
		{
			Application::InterpretMessage(event);
		}
	}
}

void TestVesselApplication::LoadAndCompileProgram()
{
	//program 1
	GLuint ahiShaders[2];// hiVertexShader, hiFragmentShader;

	ahiShaders[0] = LoadShader("media/shaders/StandardMeshShader.vert.glsl", GL_VERTEX_SHADER, true);
	ahiShaders[1] = LoadShader("media/shaders/StandardMeshShader.frag.glsl", GL_FRAGMENT_SHADER, true);


	m_hiProgram = LinkProgram(ahiShaders, 2, true, true);// glCreateProgram();
														 /*glAttachShader(m_hiProgram, hiVertexShader);
														 glAttachShader(m_hiProgram, hiFragmentShader);
														 glLinkProgram(m_hiProgram);*/


	//get the unforms
	m_hiMoveMatrixUniformLocation = glGetUniformLocation(m_hiProgram, "moveMatrix");
	m_hiProjMatrixUniformLocation = glGetUniformLocation(m_hiProgram, "projectionMatrix");

	//program 2
	/*ahiShaders[0] = LoadShader("media/shaders/SimpleGreen.vert.glsl", GL_VERTEX_SHADER, true);
	ahiShaders[1] = LoadShader("media/shaders/SimpleGreen.frag.glsl", GL_FRAGMENT_SHADER, true);


	m_hiProgramLine = LinkProgram(ahiShaders, 2, true, true);
	
	//get the unforms
	//m_hiMoveMatrixUniformLocationLine = glGetUniformLocation(m_hiProgramLine, "mv_matrix");
	m_hiProjMatrixUniformLocationLine = glGetUniformLocation(m_hiProgramLine, "proj_matrix");*/

}

void TestVesselApplication::GenerateTexture()
{
	const UI32 size = 1024;
	UC8* data = new UC8[size * size * 4];
	UC8* data1 = new UC8[size * size * 4];
	
	UC8 baseGrey = 150;

	for (UI32 i = 0; i < size *size *4; i+=4)
	{
		UC8 baseR, baseG, baseB;
		F32 val = (F32)(i / (size * 4)) / (F32)size;

		if (( val < 0.2f) ||
			(( val > 0.25f) && (val < 0.4f)) ||
			((val > 0.45f) && (val < 0.6f)))
		{
			baseR = baseGrey;
			baseB = baseGrey;
			baseG = baseGrey;
		}
		else if (
			((val >= 0.2f) && (val <= 0.25f)) ||
			((val >= 0.4f) && (val <= 0.45f)) ||
			((val >= 0.6f) && (val <= 0.65f)))
		{
			baseR = 0;
			baseB = 0;
			baseG = 0;
		}
		else
		{
			baseR = 255;
			baseB = 0;
			baseG = 0;
		}

		data[i] = baseR;
		data[i + 1] = baseG;
		data[i + 2] = baseB;
		data[i + 3] = 255;

		data1[i] = 0;
		data1[i + 1] = 0;
		data1[i + 2] = 0;
		data1[i + 3] = 255;
	}

	glGenTextures(2, m_hiTextureObject);
	glBindTexture(GL_TEXTURE_2D, m_hiTextureObject[0]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, size, size);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, m_hiTextureObject[1]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, size, size);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_UNSIGNED_BYTE, data1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*glCreateTextures(GL_TEXTURE_2D, 1, &m_hiTextureObject);             // Generate a texture
	glBindTexture(GL_TEXTURE_2D, m_hiTextureObject); // Bind that texture temporarily
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, 256, 256);



	GLint mode = GL_RGB;                   // Set the mode

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Create the texture. We get the offsets from the image, then we use it with the image's
	// pixel data to create it.
	glTextureSubImage2D(m_hiTextureObject, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Unbind the texture
	//glBindTexture(GL_TEXTURE_2D, NULL);

	// Output a successful message
	//std::cout << "Texture \"" << rkFilename << "\" successfully loaded.\n";

	// Delete the two buffers.
	delete[] data;*/
}
