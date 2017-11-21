#include "stdafx.h"
#include "GUIManager.h"


GUIManager::GUIManager()
{
}


GUIManager::~GUIManager()
{
}

bool GUIManager::Initialize(sf::RenderWindow * Win)
{
	mWindow = Win;
	initializeMaps();
	try
	{
		mRenderer = &CEGUI::OpenGL3Renderer::bootstrapSystem();
		mSystem = CEGUI::System::getSingletonPtr();
		m_pkGuiContext = &mSystem->getDefaultGUIContext();
		
		CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(
			CEGUI::System::getSingleton().getResourceProvider());

		CEGUI::String dataPathPrefix = reinterpret_cast<const CEGUI::utf8*>("./media/");

		rp->setResourceGroupDirectory("schemes",
			dataPathPrefix + reinterpret_cast<const CEGUI::utf8*>("/schemes/"));
		rp->setResourceGroupDirectory("imagesets",
			dataPathPrefix + reinterpret_cast<const CEGUI::utf8*>("/imagesets/"));
		rp->setResourceGroupDirectory("fonts",
			dataPathPrefix + reinterpret_cast<const CEGUI::utf8*>("/fonts/"));
		rp->setResourceGroupDirectory("layouts",
			dataPathPrefix + reinterpret_cast<const CEGUI::utf8*>("/layouts/"));
		rp->setResourceGroupDirectory("looknfeels",
			dataPathPrefix + reinterpret_cast<const CEGUI::utf8*>("/looknfeel/"));
		rp->setResourceGroupDirectory("lua_scripts",
			dataPathPrefix + reinterpret_cast<const CEGUI::utf8*>("/lua_scripts/"));
		rp->setResourceGroupDirectory("schemas",
			dataPathPrefix + reinterpret_cast<const CEGUI::utf8*>("/xml_schemas/"));
		rp->setResourceGroupDirectory("animations",
			dataPathPrefix + reinterpret_cast<const CEGUI::utf8*>("/animations/"));

		/*rp->setResourceGroupDirectory("schemes", "./media/schemes/");
		rp->setResourceGroupDirectory("fonts", "./media/fonts/");
		rp->setResourceGroupDirectory("imagesets", "./media/imagesets/");
		rp->setResourceGroupDirectory("looknfeel", "./media/looknfeel/");*/

		CEGUI::SchemeManager::getSingleton().createFromFile("WindowsLook.scheme", "schemes");

		//load default font
		CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-10.font", "fonts");
		mSystem->getDefaultGUIContext().setDefaultFont("DejaVuSans-10");
		//set cursor
		mSystem->getDefaultGUIContext().getMouseCursor().setDefaultImage("WindowsLook/MouseArrow");

		mWindowManager = CEGUI::WindowManager::getSingletonPtr();
	}
	catch (CEGUI::Exception& e)
	{
		printf("CEGUI Error: %s\n", e.getMessage().c_str());
		return false;
	}
	return true;
}

void GUIManager::Update(float deltaSeconds)
{
	mSystem->injectTimePulse(deltaSeconds);
}

void GUIManager::Terminate()
{
	CEGUI::System::destroy();
	CEGUI::OpenGL3Renderer::destroy(*mRenderer);
}

bool GUIManager::handleEvent(sf::Event & Event)
{
	sf::Vector2i mouse_pos;
	switch (Event.type)
	{
	case sf::Event::TextEntered:
		return m_pkGuiContext->injectChar(Event.text.unicode);
	case sf::Event::KeyPressed:
		return m_pkGuiContext->injectKeyDown(toCEGUIKey(Event.key.code));
	case sf::Event::KeyReleased:
		return m_pkGuiContext->injectKeyUp(toCEGUIKey(Event.key.code));
	case sf::Event::MouseMoved:
		mouse_pos = sf::Mouse::getPosition(*(sf::Window*)mWindow);
		return m_pkGuiContext->injectMousePosition(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y));
	case sf::Event::MouseButtonPressed:
		return m_pkGuiContext->injectMouseButtonDown(toCEGUIMouseButton(Event.mouseButton.button));
	case sf::Event::MouseButtonReleased:
		return m_pkGuiContext->injectMouseButtonUp(toCEGUIMouseButton(Event.mouseButton.button));
	case sf::Event::MouseWheelMoved:
		return m_pkGuiContext->injectMouseWheelChange(static_cast<float>(Event.mouseWheel.delta));
	case sf::Event::Resized:
		CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Sizef((float)Event.size.width, (float)Event.size.height));
		return true;
	}
	return false;
}

void GUIManager::Draw()
{
	mSystem->renderAllGUIContexts();
}

void GUIManager::initializeMaps()
{
	m_mapSFKeyToCEGUIKey[sf::Keyboard::A] = CEGUI::Key::A;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::B] = CEGUI::Key::B;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::C] = CEGUI::Key::C;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::D] = CEGUI::Key::D;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::E] = CEGUI::Key::E;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F] = CEGUI::Key::F;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::G] = CEGUI::Key::G;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::H] = CEGUI::Key::H;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::I] = CEGUI::Key::I;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::J] = CEGUI::Key::J;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::K] = CEGUI::Key::K;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::L] = CEGUI::Key::L;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::M] = CEGUI::Key::M;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::N] = CEGUI::Key::N;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::O] = CEGUI::Key::O;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::P] = CEGUI::Key::P;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Q] = CEGUI::Key::Q;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::R] = CEGUI::Key::R;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::S] = CEGUI::Key::S;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::T] = CEGUI::Key::T;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::U] = CEGUI::Key::U;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::V] = CEGUI::Key::V;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::W] = CEGUI::Key::W;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::X] = CEGUI::Key::X;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Y] = CEGUI::Key::Y;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Z] = CEGUI::Key::Z;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num0] = CEGUI::Key::Zero;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num1] = CEGUI::Key::One;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num2] = CEGUI::Key::Two;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num3] = CEGUI::Key::Three;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num4] = CEGUI::Key::Four;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num5] = CEGUI::Key::Five;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num6] = CEGUI::Key::Six;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num7] = CEGUI::Key::Seven;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num8] = CEGUI::Key::Eight;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Num9] = CEGUI::Key::Nine;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Escape] = CEGUI::Key::Escape;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::LControl] = CEGUI::Key::LeftControl;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::LShift] = CEGUI::Key::LeftShift;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::LAlt] = CEGUI::Key::LeftAlt;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::LSystem] = CEGUI::Key::LeftWindows;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::RControl] = CEGUI::Key::RightControl;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::RShift] = CEGUI::Key::RightShift;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::RAlt] = CEGUI::Key::RightAlt;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::RSystem] = CEGUI::Key::RightWindows;
	//m_mapSFKeyToCEGUIKey[sf::Keyboard::Menu]		= CEGUI::Key: --Dunno what this is
	m_mapSFKeyToCEGUIKey[sf::Keyboard::LBracket] = CEGUI::Key::LeftBracket;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::RBracket] = CEGUI::Key::RightBracket;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::SemiColon] = CEGUI::Key::Semicolon;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Comma] = CEGUI::Key::Comma;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Period] = CEGUI::Key::Period;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Quote] = CEGUI::Key::Apostrophe;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Slash] = CEGUI::Key::Slash;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::BackSlash] = CEGUI::Key::Backslash;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Tilde] = CEGUI::Key::Grave;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Equal] = CEGUI::Key::Equals;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Dash] = CEGUI::Key::Minus;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Space] = CEGUI::Key::Space;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Return] = CEGUI::Key::Return;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::BackSpace] = CEGUI::Key::Backspace;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Tab] = CEGUI::Key::Tab;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::PageUp] = CEGUI::Key::PageUp;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::PageDown] = CEGUI::Key::PageDown;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::End] = CEGUI::Key::End;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Home] = CEGUI::Key::Home;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Insert] = CEGUI::Key::Insert;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Delete] = CEGUI::Key::Delete;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Add] = CEGUI::Key::Add;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Subtract] = CEGUI::Key::Subtract;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Multiply] = CEGUI::Key::Multiply;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Divide] = CEGUI::Key::Divide;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Left] = CEGUI::Key::ArrowLeft;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Right] = CEGUI::Key::ArrowRight;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Up] = CEGUI::Key::ArrowUp;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Down] = CEGUI::Key::ArrowDown;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad0] = CEGUI::Key::Numpad0;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad1] = CEGUI::Key::Numpad1;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad2] = CEGUI::Key::Numpad2;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad3] = CEGUI::Key::Numpad3;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad4] = CEGUI::Key::Numpad4;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad5] = CEGUI::Key::Numpad5;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad6] = CEGUI::Key::Numpad6;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad7] = CEGUI::Key::Numpad7;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad8] = CEGUI::Key::Numpad8;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Numpad9] = CEGUI::Key::Numpad9;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F1] = CEGUI::Key::F1;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F2] = CEGUI::Key::F2;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F3] = CEGUI::Key::F3;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F4] = CEGUI::Key::F4;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F5] = CEGUI::Key::F5;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F6] = CEGUI::Key::F6;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F7] = CEGUI::Key::F7;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F8] = CEGUI::Key::F8;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F9] = CEGUI::Key::F9;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F10] = CEGUI::Key::F10;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F11] = CEGUI::Key::F11;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F12] = CEGUI::Key::F12;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F13] = CEGUI::Key::F13;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F14] = CEGUI::Key::F14;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::F15] = CEGUI::Key::F15;
	m_mapSFKeyToCEGUIKey[sf::Keyboard::Pause] = CEGUI::Key::Pause;

	m_mapSFMouseToCEGUIMouse[sf::Mouse::Left] = CEGUI::LeftButton;
	m_mapSFMouseToCEGUIMouse[sf::Mouse::Right] = CEGUI::RightButton;
	m_mapSFMouseToCEGUIMouse[sf::Mouse::Middle] = CEGUI::MiddleButton;
	m_mapSFMouseToCEGUIMouse[sf::Mouse::XButton1] = CEGUI::X1Button;
	m_mapSFMouseToCEGUIMouse[sf::Mouse::XButton2] = CEGUI::X2Button;
}

CEGUI::Key::Scan GUIManager::toCEGUIKey(sf::Keyboard::Key Code)
{
	if (m_mapSFKeyToCEGUIKey.find(Code) == m_mapSFKeyToCEGUIKey.end())
		return (CEGUI::Key::Scan)0;

	return m_mapSFKeyToCEGUIKey[Code];
}

CEGUI::MouseButton GUIManager::toCEGUIMouseButton(sf::Mouse::Button Button)
{
	if (m_mapSFMouseToCEGUIMouse.find(Button) == m_mapSFMouseToCEGUIMouse.end())
		return (CEGUI::MouseButton)0;

	return m_mapSFMouseToCEGUIMouse[Button];
}
