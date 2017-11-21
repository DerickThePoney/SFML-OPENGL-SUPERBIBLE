#include "stdafx.h"
#include "CEGUITutorial.h"

using namespace CEGUI;

CEGUITutorial::CEGUITutorial()
	: Application()
{
}


CEGUITutorial::~CEGUITutorial()
{
}

void CEGUITutorial::Render(double currentTime)
{
	static const GLfloat one = 1.0f;
	glClearBufferfv(GL_COLOR, 0, vmath::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	glClearBufferfv(GL_DEPTH, 0, &one);
	//m_window.draw(m_kString);
	mGUIManager.Draw();
	m_window.display();
}

void CEGUITutorial::Initialise()
{
	m_window.setMouseCursorVisible(false);
	if (!mGUIManager.Initialize((sf::RenderWindow*)&m_window))
		return;

	m_kString.setPosition(300.0f, 400.0f);
	/***************** Create our dialog **********************/
	try
	{
		CEGUI::WindowManager* WindowMgr = mGUIManager.getWindowManager();
		CEGUI::Window* Dialog = WindowMgr->createWindow("WindowsLook/FrameWindow", "OurDialog");

		Dialog->setMinSize(CEGUI::USize(UDim(0.0f, 200), UDim(0.0f, 150)));
		Dialog->setSize(USize(UDim(0.0f, 400), UDim(0.0f, 300)));
		Dialog->setPosition(UVector2(UDim(0.25f, 0), UDim(0.1f, 0)));
		Dialog->setText("Window");
		Dialog->subscribeEvent(FrameWindow::EventCloseClicked, Event::Subscriber(&CEGUITutorial::onDialog_Closed, this));

		m_pkEditBox = WindowMgr->createWindow("WindowsLook/Editbox", "OurDialog_Editbox");
		m_pkEditBox->setMinSize(USize(UDim(0.0f, 100), UDim(0.0f, 20)));
		m_pkEditBox->setSize(USize(UDim(0.5f, 0), UDim(0.1f, 0)));
		m_pkEditBox->setPosition(UVector2(UDim(0.25f, 0), UDim(0.4f, 0)));
		m_pkEditBox->subscribeEvent(CEGUI::Window::EventTextChanged, Event::Subscriber(&CEGUITutorial::onEditbox_TextChanged,this));
		Dialog->addChild(m_pkEditBox);
		Dialog->setMousePassThroughEnabled(true);
		mGUIManager.setRootWindow(Dialog);
	}
	catch (CEGUI::Exception& e)
	{
		printf("CEGUI Exception: %s\n", e.getMessage().c_str());
		return;
	}
	return;

}

void CEGUITutorial::Update(double deltaTime)
{
	mGUIManager.Update((float)deltaTime/1000.0f);
	std::stringstream sstr;
	std::string s = m_kString.getString();
	sstr << s << " : " << 1 / (deltaTime / 1000.0f) << " fps";
	mGUIManager.getRootWindow()->setText(sstr.str());
}

void CEGUITutorial::Terminate()
{
	mGUIManager.Terminate();
}

void CEGUITutorial::OnResize(unsigned int width, unsigned int height)
{
	Application::OnResize(width, height);
}

void CEGUITutorial::HandleMessages()
{
	// handle events
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		//See if CEGUI should handle it
		if (mGUIManager.handleEvent(event))
			continue;

		Application::InterpretMessage(event);
	}
}

bool CEGUITutorial::onDialog_Closed(const CEGUI::EventArgs & e)
{
	// end the program
	m_bRunning = false; 
	return true;
}

bool CEGUITutorial::onEditbox_TextChanged(const CEGUI::EventArgs & e)
{
	m_kString.setString(m_pkEditBox->getText().c_str());
	return true;
}
