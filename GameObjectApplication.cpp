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

	LoadAndCompileProgram();

	/*m_kLookAtMatrix = vmath::lookat(vmath::vec3(0, 0, 20), vmath::vec3(0, 0, 0), vmath::vec3(0, 1, 0));
	m_kProjectionMatrice = vmath::perspective(50.0f,
		(float)m_window.getSize().x / (float)m_window.getSize().y,
		0.1f,
		1000.0f);*/

	m_kCamera.Init(50, (float)m_window.getSize().x / (float)m_window.getSize().y,
		0.1f,
		1000.0f);

	m_kCamera.UpdateAllTransformsInHierarchy();
	m_kCamera.AddChild(&kGameObjects[0]);
	kGameObjects[0].SetName("BaseObject");
	kGameObjects[1].SetName("Middle");
	kGameObjects[2].SetName("LeafObject");

	kGameObjects[0].AddChild(&kGameObjects[1]);
	kGameObjects[1].AddChild(&kGameObjects[2]);

	kGameObjects[0].m_kTransform.SetLocalPosition(vec3(0, 0, 20));
	kGameObjects[1].m_kTransform.SetLocalPosition(vec3(1, 0, 0));
	kGameObjects[2].m_kTransform.SetLocalPosition(vec3(1, 0, 0));

	kGameObjects[0].m_kTransform.SetLocalOrientation(quaternion(-1, 0, 0, 0));

	m_kCamera.UpdateAllTransformsInHierarchy();



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
	
	//m_kCamera.UpdateAllTransformsInHierarchy();
	/*kGameObjects[0].m_kTransform.SetLocalOrientation(normalize(q));
	kGameObjects[1].m_kTransform.SetLocalOrientation(normalize(q2));
	kGameObjects[2].m_kTransform.SetLocalOrientation(normalize(q3));*/
	m_kCamera.UpdateAllTransformsInHierarchy();
	elapsed += deltaTime * 0.001 / fLoopTime;
//	++tick;
	/*std::cout << "---------------------------------------------------------------\n";
	std::cout << "UPDATE LOOP: delta: " << deltaTime << "\n";
	std::cout << "q: " << q[0] << "\t" << q[1] << "\t" << q[2] << "\t" << q[3] << "\n";
	for (int i = 0; i < 3; ++i)
	{
		std::cout << "Game object " << i << "\n";
		kGameObjects[i].m_kTransform.WriteIntoStream(std::cout);
	}*/

	ImGuiIO &io = ImGui::GetIO();
	

	ImGui::Begin("Hierarchy");
	static GameObject* node_clicked;// = nullptr;
	//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
	m_kCamera.ImGUIHierarchy(node_clicked);
	//kGameObjects[0].ImGUIHierarchy(node_clicked);
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
	m_kProgram.UseProgram();
	m_kMesh.BindForDrawing();

	m_kCamera.LoadProjectionOnGraphics(m_hiProjMatrixUniformLocation);

	for (int i = 0; i < 3; ++i)
	{
		//set modelview and proj matrix
		glUniformMatrix4fv(m_hiMoveMatrixUniformLocation, 1, GL_FALSE, m_kCamera.GetLookAt() * kGameObjects[i].m_kTransform.GetWorldSpaceTransform());

		//draw call
		glDrawElements(GL_TRIANGLES, m_kMesh.m_aiIndices.size(), GL_UNSIGNED_INT, 0);
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
	m_kCamera.OnResize(width, height);
}

void GameObjectApplication::LoadAndCompileProgram()
{
	OGLShader akShaders[2];// hiVertexShader, hiFragmentShader;

	akShaders[0].InitFromFile("media/shaders/StandardMeshShader.vert.glsl", GL_VERTEX_SHADER);
	akShaders[1].InitFromFile("media/shaders/StandardMeshShader.frag.glsl", GL_FRAGMENT_SHADER);

	m_kProgram.LinkProgram(akShaders, 2, false);

	m_hiMoveMatrixUniformLocation = glGetUniformLocation(m_kProgram, "mv_matrix");
	m_hiProjMatrixUniformLocation = glGetUniformLocation(m_kProgram, "proj_matrix");
}
