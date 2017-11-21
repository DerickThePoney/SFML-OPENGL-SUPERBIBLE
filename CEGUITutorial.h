#pragma once
#include "Application.h"
class CEGUITutorial :
	public Application
{
public:
	CEGUITutorial();
	~CEGUITutorial();

	void Render(double currentTime) override;
	void Initialise() override;
	void Update(double deltaTime) override;
	void Terminate() override;
	void OnResize(unsigned int width, unsigned int height) override;

	virtual void HandleMessages() override;

	bool onDialog_Closed(const CEGUI::EventArgs& e);
	bool onEditbox_TextChanged(const CEGUI::EventArgs& e);

private:
	GUIManager mGUIManager;
	sf::Text m_kString;
	CEGUI::Window* m_pkEditBox;
};

