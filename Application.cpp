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
	sf::Joystick::Identification idJoystick;
	switch (event.type)
	{
	case sf::Event::Closed:
		// end the program
		m_bRunning = false;
		break;
	case sf::Event::Resized:
		// adjust the viewport when the window is resized
		OnResize(event.size.width, event.size.height);
		break;
	case sf::Event::EventType::JoystickButtonPressed:  ///< A joystick button was pressed (data in event.joystickButton)
		std::cout << "JoystickButtonPressed: id = " << event.joystickButton.joystickId << " button = " << event.joystickButton.button << std::endl;
		break;
	case sf::Event::EventType::JoystickButtonReleased: ///< A joystick button was released (data in event.joystickButton)
		std::cout << "JoystickButtonReleased: id = " << event.joystickButton.joystickId << " button = " << event.joystickButton.button << std::endl;
		break;
	case sf::Event::EventType::JoystickMoved:          ///< The joystick moved along an axis (data in event.joystickMove)
		std::cout << "JoystickMoved: id = " << event.joystickMove.joystickId << " axis = " << event.joystickMove.axis << " position = " << event.joystickMove.position << std::endl;
		break;
	case sf::Event::EventType::JoystickConnected:      ///< A joystick was connected (data in event.joystickConnect)
		std::cout << "JoystickConnected: id = " << event.joystickConnect.joystickId << std::endl;
		idJoystick = sf::Joystick::getIdentification(event.joystickConnect.joystickId);
		std::cout << "\tname= " << (std::string)idJoystick.name << " vendor id = " << idJoystick.vendorId << " product id = " << idJoystick.productId << std::endl;
		break;
	case sf::Event::EventType::JoystickDisconnected:  ///< A joystick was disconnected (data in event.joystickConnect)
		std::cout << "JoystickDisconnected: id = " << event.joystickConnect.joystickId << std::endl;
		break;

	case sf::Event::EventType::KeyPressed:
		InputManager::Instance()->HandleKeyboardMessages(event);
		if (event.key.code == sf::Keyboard::Escape)
		{
			m_bRunning = false;
		}
		break;
	case sf::Event::EventType::KeyReleased:
		InputManager::Instance()->HandleKeyboardMessages(event);
		break;
	default:
		break;
	}
	
}

void Application::MainLoop()
{
	sf::ContextSettings context(24, 8, 16, 4, 5, sf::ContextSettings::Attribute::Debug);

	
	// create the window
	m_window.create(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, context);
	m_window.setVerticalSyncEnabled(false);

	// load resources, initialize the OpenGL states, ...
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	glDebugMessageCallback(&OGLUtilities::DebugCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

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

		//Do the timing IMGUI stuffs
		bool tmp = true;
		const int NbFramesSaved = 1000;
		static float frameTimeValues[NbFramesSaved] = { 0 };
		static int values_offset = 0;
		static int frameNb = 0;
		static float average = 0;
		static float maxValue = 0;
		ImGui::Begin("Time Information: ", &tmp);
		
		float tmpF = TIME_MSEC_FLOAT(elapsedFrame);

		if (tmpF > maxValue) maxValue = tmpF;
		
		int multiplier = vmath::min(frameNb, NbFramesSaved);
		int divider = vmath::min(++frameNb, NbFramesSaved);
		if (TIME_MSEC_FLOAT(elapsedFrame) != 0 && !isinf(frameTimeValues[values_offset]))
		{
			average = average * (multiplier) - frameTimeValues[values_offset] + tmpF;

			average /= divider;
		}
		frameTimeValues[values_offset] = tmpF;
		
		values_offset = (values_offset + 1) % NbFramesSaved;

		std::stringstream sstr; sstr << "avg " << average;

		ImGui::InputFloat("frame time", &tmpF);
		ImGui::PlotLines("Frame time", frameTimeValues, NbFramesSaved, values_offset, sstr.str().c_str(), 0, vmath::min(average*2, maxValue), ImVec2(0, 80));// , "", 0, 1.0f, ImVec2(0, 80));

		ImGui::End();

		// Do the actual main loop stuffs

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
