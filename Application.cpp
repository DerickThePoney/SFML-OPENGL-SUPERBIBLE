#include "stdafx.h"
#include "Application.h"


Application::Application()
	: m_bOnEscapeQuit (true)
{
}


Application::~Application()
{
}

void Application::OnResize(unsigned int width, unsigned int height)
{
	glViewport(0, 0, width, height);
}

void Application::HandleMessages()
{
	// handle events
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);
		InterpretMessage(event);
	}
}

void Application::InterpretMessage(sf::Event event)
{
	if (event.type == sf::Event::Closed)
	{
		// end the program
		m_bRunning = false;
	}
	else if (event.type == sf::Event::Resized)
	{
		// adjust the viewport when the window is resized
		OnResize(event.size.width, event.size.height);
	}
	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		m_bRunning = false;
	}
}

void Application::MainLoop()
{
	sf::ContextSettings context(24, 8, 16, 4, 5, 0U);

	
	// create the window
	m_window.create(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, context);
	m_window.setVerticalSyncEnabled(true);

	// load resources, initialize the OpenGL states, ...
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	//intialise stuff
	Initialise();

	//init imgui
	ImGui::SFML::SetRenderTarget(m_window);
	ImGui::SFML::InitImGuiRendering();
	ImGui::SFML::SetWindow(m_window);
	ImGui::SFML::InitImGuiEvents();


	//actual main loop
	std::chrono::time_point<std::chrono::high_resolution_clock> startTimeTotal, startTimeFrame;
	startTimeTotal = std::chrono::high_resolution_clock::now();
	startTimeFrame = startTimeTotal;
	auto elapsedFrame = startTimeFrame - startTimeTotal;
	auto elaspedTotal = std::chrono::high_resolution_clock::now() - startTimeTotal;
	m_bRunning = true;
	while (m_bRunning)
	{
		ImGui::SFML::UpdateImGui();
		ImGui::SFML::UpdateImGuiRendering();

		bool tmp = true;
		ImGui::Begin("Time Information: ", &tmp);
		float tmpF = 1.0 / TIME_SEC_DOUBLE(elapsedFrame);
		ImGui::InputFloat("fps", &tmpF);
		ImGui::End();

		HandleMessages();
		Update(TIME_MSEC_DOUBLE(elapsedFrame));
		Render(TIME_MSEC_DOUBLE(elaspedTotal));

		auto now = std::chrono::high_resolution_clock::now();
		elapsedFrame = now - startTimeFrame;
		elaspedTotal = now - startTimeTotal;
		startTimeFrame = now;
	}

	//termination stuff
	Terminate();
}
