#pragma once

#include <CEGUI\CEGUI.h>
#include <CEGUI\RendererModules\OpenGL\GL3Renderer.h>
#include <SFML/Graphics/RenderWindow.hpp>

class GUIManager
{
public:
	GUIManager();
	~GUIManager();

	bool Initialize(sf::RenderWindow* Win);
	void Update(float deltaSeconds);
	void Terminate();
	bool handleEvent(sf::Event& Event);
	void Draw();

	CEGUI::System* getSystem() { return mSystem; }
	CEGUI::WindowManager* getWindowManager() { return mWindowManager; }
	void setRootWindow(CEGUI::Window* Win) { mSystem->getDefaultGUIContext().setRootWindow(Win); }
	CEGUI::Window* getRootWindow() { return mSystem->getDefaultGUIContext().getRootWindow(); }

private:
	sf::RenderWindow* mWindow;

	CEGUI::System* mSystem;
	CEGUI::GUIContext* m_pkGuiContext;
	CEGUI::OpenGL3Renderer* mRenderer;
	CEGUI::WindowManager* mWindowManager;

	typedef std::map<sf::Keyboard::Key, CEGUI::Key::Scan> KeyMap;
	typedef std::map<sf::Mouse::Button, CEGUI::MouseButton> MouseButtonMap;

	KeyMap m_mapSFKeyToCEGUIKey;
	MouseButtonMap m_mapSFMouseToCEGUIMouse;

	void initializeMaps();
	CEGUI::Key::Scan toCEGUIKey(sf::Keyboard::Key Code);
	CEGUI::MouseButton toCEGUIMouseButton(sf::Mouse::Button Button);
};

