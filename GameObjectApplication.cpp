#include "stdafx.h"
#include "GameObjectApplication.h"


GameObjectApplication::GameObjectApplication()
{
}


GameObjectApplication::~GameObjectApplication()
{
}

void GameObjectApplication::Initialise()
{
	//create the mesh
	BMesh kBMesh;
	MakeCube kMakeCube(vec3(0.2f));
	kMakeCube.Apply(kBMesh);
	kBMesh.BuildMesh(m_kMesh);
	m_kMesh.LoadOnGraphicsCard();

	BMesh kBMesh2;
	MakeSingleFace kSingleFace(vec3(1));// 0.1f, 0.3, 1));
	//TranslateFace kFaceTranslate(0, vec3(-0.7, 0.7,0));
	kSingleFace.Apply(kBMesh2);
	//kFaceTranslate.Apply(kBMesh2);
	kBMesh2.BuildMesh(m_kDirectionsX);
	m_kDirectionsX.LoadOnGraphicsCard();

	BMesh kBMesh3;
	MakeSingleFace kSingleFace2(vec3(1));// 0.3f, 0.1, 1));
	kSingleFace2.Apply(kBMesh3);
	//kFaceTranslate2.Apply(kBMesh3);
	kBMesh3.BuildMesh(m_kDirectionsY);
	m_kDirectionsY.LoadOnGraphicsCard();


	LoadAndCompileProgram();

	m_kLookAtMatrix = vmath::lookat(vmath::vec3(0, 0, 0), vmath::vec3(0, 0, 20), vmath::vec3(0, 1, 0));
	m_kProjectionMatrice = vmath::perspective(50.0f,
		(float)m_window.getSize().x / (float)m_window.getSize().y,
		0.1f,
		1000.0f);

	kGameObjects[0].SetName("BaseObject");
	kGameObjects[1].SetName("Middle");
	kGameObjects[2].SetName("LeafObject");

	kGameObjects[0].AddChild(&kGameObjects[1]);
	kGameObjects[1].AddChild(&kGameObjects[2]);

	kGameObjects[0].m_kTransform.SetLocalPosition(vec3(0, 0, 20));
	kGameObjects[1].m_kTransform.SetLocalPosition(vec3(1, 0, 0));
	kGameObjects[2].m_kTransform.SetLocalPosition(vec3(1, 0, 0));

	kGameObjects[0].UpdateAllTransformsInHierarchy();

	/*std::cout << "---------------------------------------------------------------\n";
	std::cout << "INIT LOOP\n";
	for (int i = 0; i < 3; ++i)
	{
		std::cout << "Game object " << i << "\n";
		kGameObjects[i].m_kTransform.WriteIntoStream(std::cout);
	}*/
}

void GameObjectApplication::Update(double deltaTime)
{
	const float angle = -90;
	//float sAngle = sin(radians(angle) / 2);
	//float cAngle = cos(radians(angle) / 2);
	quaternion q,q2,q3;

	const quaternion qStart(1, vec3(0, 0, 0));
	const quaternion qEnd(cos(radians(angle) / 2), vec3(0, 0, 1 * sin(radians(angle) / 2)));
	const quaternion qEnd2(cos(radians(-angle) / 2), vec3(0, 1*sin(radians(-angle) / 2), 0));
	const quaternion qEnd3(cos(radians(-angle) / 2), vec3(1 * sin(radians(-angle) / 2), 0, 0));
	const double fLoopTime = 3;
	static double elapsed = 0;
	
	float t = (float)(elapsed - (int)(elapsed));

	if ((int)(elapsed) & 1)
	{
		q = Slerp(qEnd, qStart, t);
		q2 = Slerp(qEnd2, qStart, t);
		q3 = Slerp(qEnd3, qStart, t);
		//q = (1 - t) * qEnd + t * qStart;
		//q = mix(qEnd, qStart, t);
	}
	else
	{
		q = Slerp(qStart, qEnd, t);
		q2 = Slerp(qStart, qEnd2, t);
		q3 = Slerp(qStart, qEnd3, t);
		//q = (1 - t) * qStart + t * qEnd;
		//q = mix(qStart, qEnd, t);
	}
	

	/*kGameObjects[0].m_kTransform.SetLocalOrientation(normalize(q));
	kGameObjects[1].m_kTransform.SetLocalOrientation(normalize(q2));
	kGameObjects[2].m_kTransform.SetLocalOrientation(normalize(q3));*/
	kGameObjects[0].UpdateAllTransformsInHierarchy();
	elapsed += deltaTime * 0.001 / fLoopTime;
//	++tick;
	std::cout << "---------------------------------------------------------------\n";
	std::cout << "UPDATE LOOP: delta: " << deltaTime << "\n";
	std::cout << "q: " << q[0] << "\t" << q[1] << "\t" << q[2] << "\t" << q[3] << "\n";
	for (int i = 0; i < 3; ++i)
	{
		std::cout << "Game object " << i << "\n";
		kGameObjects[i].m_kTransform.WriteIntoStream(std::cout);
	}

	ImGuiIO &io = ImGui::GetIO();
	

	ImGui::Begin("Hierarchy");
	static GameObject* node_clicked;// = nullptr;
	//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
	kGameObjects[0].ImGUIHierarchy(node_clicked);
	//ImGui::PopStyleVar();
	//ImGui::TreePop();
	ImGui::End();


	ImGui::Begin("Inspector");

	if (node_clicked != nullptr)
	{
		node_clicked->Inspector();
	}

	ImGui::End();


}



void GameObjectApplication::Render(double currentTime)
{
	//draw

    static const GLfloat one = 1.0f;

	//clear buffer
	glClearBufferfv(GL_COLOR, 0, vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, &one);

	//use program
	glUseProgram(m_hiProgram);
	m_kMesh.BindForDrawing();

	glUniformMatrix4fv(m_hiProjMatrixUniformLocation, 1, GL_FALSE, m_kProjectionMatrice);

	for (int i = 0; i < 3; ++i)
	{
		vmath::mat4 moveMat = m_kLookAtMatrix * kGameObjects[i].m_kTransform.GetWorldSpaceTransform();
		glUseProgram(m_hiProgram);
		m_kMesh.BindForDrawing();

		glUniformMatrix4fv(m_hiProjMatrixUniformLocation, 1, GL_FALSE, m_kProjectionMatrice);
		//set modelview and proj matrix
		glUniformMatrix4fv(m_hiMoveMatrixUniformLocation, 1, GL_FALSE, moveMat);

		//draw call
		glDrawElements(GL_TRIANGLES, m_kMesh.m_aiIndices.size(), GL_UNSIGNED_INT, 0);

		//set modelview and proj matrix
		const vec4 red = vec4(1, 0, 0, 1);
		const vec4 green = vec4(0, 1, 0, 1);
		const vec4 blue = vec4(1, 0, 1, 1);

		glUseProgram(m_hiProgramArrows);
		m_kDirectionsX.BindForDrawing();
		
		glUniform4fv(m_hiColor, 1, red.GetData());
		glUniformMatrix4fv(m_hiMovColor, 1, GL_FALSE, moveMat);
		glUniformMatrix4fv(m_hiProjColor, 1, GL_FALSE, m_kProjectionMatrice);
		glDrawElements(GL_TRIANGLES, m_kDirectionsX.m_aiIndices.size(), GL_UNSIGNED_INT, 0);


		m_kDirectionsY.BindForDrawing();
		glUniform4fv(m_hiColor, 1, green.GetData());
		glUniformMatrix4fv(m_hiMovColor, 1, GL_FALSE, moveMat);
		//glUniformMatrix4fv(m_hiProjColor, 1, GL_FALSE, m_kProjectionMatrice);
		glDrawElements(GL_TRIANGLES, m_kDirectionsY.m_aiIndices.size(), GL_UNSIGNED_INT, 0);
	}

	ImGui::Render();

	m_window.display();
}

void GameObjectApplication::Terminate()
{
	ImGui::SFML::Shutdown();
}

void GameObjectApplication::OnResize(unsigned int width, unsigned int height)
{
	Application::OnResize(width, height);
	m_kProjectionMatrice = vmath::perspective(50.0f,
		(float)width / (float)height,
		0.1f,
		1000.0f);
}

void GameObjectApplication::LoadAndCompileProgram()
{
	GLuint ahiShaders[2];// hiVertexShader, hiFragmentShader;

	ahiShaders[0] = LoadShader("media/shaders/SpinnyCube.vert.glsl", GL_VERTEX_SHADER, true);
	ahiShaders[1] = LoadShader("media/shaders/SpinnyCube.frag.glsl", GL_FRAGMENT_SHADER, true);


	m_hiProgram = LinkProgram(ahiShaders, 2, true, true);// glCreateProgram();
														 /*glAttachShader(m_hiProgram, hiVertexShader);
														 glAttachShader(m_hiProgram, hiFragmentShader);
														 glLinkProgram(m_hiProgram);*/

														 //delete vertex and fragment shaders
														 //glDeleteShader(hiVertexShader);
														 //glDeleteShader(hiFragmentShader);

														 //get the unforms
	m_hiMoveMatrixUniformLocation = glGetUniformLocation(m_hiProgram, "mv_matrix");
	m_hiProjMatrixUniformLocation = glGetUniformLocation(m_hiProgram, "proj_matrix");

//	GLuint ahiShaders[2];// hiVertexShader, hiFragmentShader;

	ahiShaders[0] = LoadShader("media/shaders/Arrows.vert.glsl", GL_VERTEX_SHADER, true);
	ahiShaders[1] = LoadShader("media/shaders/Arrows.frag.glsl", GL_FRAGMENT_SHADER, true);


	m_hiProgramArrows = LinkProgram(ahiShaders, 2, true, true);// glCreateProgram();
														 /*glAttachShader(m_hiProgram, hiVertexShader);
														 glAttachShader(m_hiProgram, hiFragmentShader);
														 glLinkProgram(m_hiProgram);*/

														 //delete vertex and fragment shaders
														 //glDeleteShader(hiVertexShader);
														 //glDeleteShader(hiFragmentShader);

														 //get the unforms
	m_hiColor = glGetUniformLocation(m_hiProgramArrows, "color");
	m_hiProjColor = glGetUniformLocation(m_hiProgramArrows, "mv_matrix");
	m_hiMovColor = glGetUniformLocation(m_hiProgramArrows, "proj_matrix");
}
