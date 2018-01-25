#include "stdafx.h"
#include "InputManager.h"

InputManager* Singleton<InputManager>::s_kInstance = nullptr;

////////////////////////////////////////////////////////////
/// \brief Key codes
///
////////////////////////////////////////////////////////////
const char* pcKeyboardKeysNames[] = ///// COPIED FROM SFML!
{
	"A",        ///< The A key
	"B",            ///< The B key
	"C",            ///< The C key
	"D",            ///< The D key
	"E",            ///< The E key
	"F",            ///< The F key
	"G",            ///< The G key
	"H",            ///< The H key
	"I",            ///< The I key
	"J",            ///< The J key
	"K",            ///< The K key
	"L",            ///< The L key
	"M",            ///< The M key
	"N",            ///< The N key
	"O",            ///< The O key
	"P",            ///< The P key
	"Q",            ///< The Q key
	"R",            ///< The R key
	"S",            ///< The S key
	"T",            ///< The T key
	"U",            ///< The U key
	"V",            ///< The V key
	"W",            ///< The W key
	"X",            ///< The X key
	"Y",            ///< The Y key
	"Z",            ///< The Z key
	"Num0",         ///< The 0 key
	"Num1",         ///< The 1 key
	"Num2",         ///< The 2 key
	"Num3",         ///< The 3 key
	"Num4",         ///< The 4 key
	"Num5",         ///< The 5 key
	"Num6",         ///< The 6 key
	"Num7",         ///< The 7 key
	"Num8",         ///< The 8 key
	"Num9",         ///< The 9 key
	"Escape",       ///< The Escape key
	"LControl",     ///< The left Control key
	"LShift",       ///< The left Shift key
	"LAlt",         ///< The left Alt key
	"LSystem",      ///< The left OS specific key: window (Windows and Linux)", apple (MacOS X)", ...
	"RControl",     ///< The right Control key
	"RShift",       ///< The right Shift key
	"RAlt",         ///< The right Alt key
	"RSystem",      ///< The right OS specific key: window (Windows and Linux)", apple (MacOS X)", ...
	"Menu",         ///< The Menu key
	"LBracket",     ///< The [ key
	"RBracket",     ///< The ] key
	"SemiColon",    ///< The ; key
	"Comma",        ///< The ", key
	"Period",       ///< The . key
	"Quote",        ///< The ' key
	"Slash",        ///< The / key
	"BackSlash",    ///< The \ key
	"Tilde",        ///< The ~ key
	"Equal",        ///< The = key
	"Dash",         ///< The - key
	"Space",        ///< The Space key
	"Return",       ///< The Return key
	"BackSpace",    ///< The Backspace key
	"Tab",          ///< The Tabulation key
	"PageUp",       ///< The Page up key
	"PageDown",     ///< The Page down key
	"End",          ///< The End key
	"Home",         ///< The Home key
	"Insert",       ///< The Insert key
	"Delete",       ///< The Delete key
	"Add",          ///< The + key
	"Subtract",     ///< The - key
	"Multiply",     ///< The * key
	"Divide",       ///< The / key
	"Left",         ///< Left arrow
	"Right",        ///< Right arrow
	"Up",           ///< Up arrow
	"Down",         ///< Down arrow
	"Numpad0",      ///< The numpad 0 key
	"Numpad1",      ///< The numpad 1 key
	"Numpad2",      ///< The numpad 2 key
	"Numpad3",      ///< The numpad 3 key
	"Numpad4",      ///< The numpad 4 key
	"Numpad5",      ///< The numpad 5 key
	"Numpad6",      ///< The numpad 6 key
	"Numpad7",      ///< The numpad 7 key
	"Numpad8",      ///< The numpad 8 key
	"Numpad9",      ///< The numpad 9 key
	"F1",           ///< The F1 key
	"F2",           ///< The F2 key
	"F3",           ///< The F3 key
	"F4",           ///< The F4 key
	"F5",           ///< The F5 key
	"F6",           ///< The F6 key
	"F7",           ///< The F7 key
	"F8",           ///< The F8 key
	"F9",           ///< The F9 key
	"F10",          ///< The F10 key
	"F11",          ///< The F11 key
	"F12",          ///< The F12 key
	"F13",          ///< The F13 key
	"F14",          ///< The F14 key
	"F15",          ///< The F15 key
	"Pause"         ///< The Pause key
};

const char* pcMouseButtonsNames[] =
{
	"Left",       ///< The left mouse button
	"Right",      ///< The right mouse button
	"Middle",     ///< The middle (wheel) mouse button
	"XButton1",   ///< The first extra mouse button
	"XButton2"   ///< The second extra mouse button
};


InputManager::InputManager()
	: m_kConfigFilename("settings/InputManager.xml")
{
	m_kKeyboard.current_queue = 1;
	m_kMouseData.current_queue = 1;
}

InputManager::~InputManager()
{
}

void InputManager::Initialise()
{
	m_kKeyboard.m_pbIsDown[0] = std::vector<bool>(sf::Keyboard::Key::KeyCount, false);
	m_kKeyboard.m_pbIsDown[1] = std::vector<bool>(sf::Keyboard::Key::KeyCount, false);

	m_kMouseData.m_pbIsDown[0] = std::vector<bool>(sf::Mouse::Button::ButtonCount, false);
	m_kMouseData.m_pbIsDown[1] = std::vector<bool>(sf::Mouse::Button::ButtonCount, false);

	LoadSettings();
}

void InputManager::HandleKeyboardMessages(sf::Event event)
{
	sf::Keyboard::Key key = event.key.code;

	if (key == sf::Keyboard::Key::Unknown) return;

	if (event.type == sf::Event::KeyPressed)
	{
		m_kKeyboard.m_pbIsDown[m_kKeyboard.current_queue][key] = true;
	}
	else if (event.type == sf::Event::KeyReleased)
	{
		m_kKeyboard.m_pbIsDown[m_kKeyboard.current_queue][key] = false;
	}
}

void InputManager::HandleMouseMessages(sf::Event event)
{
	ivec2 kCurrentPosition;
	switch (event.type)
	{
	case sf::Event::MouseMoved:
		kCurrentPosition = ivec2(event.mouseMove.x, event.mouseMove.y);
		m_kMouseData.m_kPositionDelta = kCurrentPosition - m_kMouseData.m_kPosition;
		m_kMouseData.m_kPosition = kCurrentPosition;
		m_kMouseData.m_bHasMoved = true;
		break;
	case sf::Event::MouseButtonPressed:
		m_kMouseData.m_pbIsDown[m_kMouseData.current_queue][event.mouseButton.button] = true;
		break;
	case sf::Event::MouseButtonReleased:
		m_kMouseData.m_pbIsDown[m_kMouseData.current_queue][event.mouseButton.button] = false;
		break;
	case sf::Event::MouseWheelScrolled:
		m_kMouseData.m_fWheelDelta = event.mouseWheelScroll.delta;
		m_kMouseData.m_bScrolled = true;
		break;
	default:
		break;
	}
}

void InputManager::PrepareNewFrame()
{
	m_kKeyboard.current_queue = 1 - m_kKeyboard.current_queue;
	m_kKeyboard.m_pbIsDown[m_kKeyboard.current_queue] = m_kKeyboard.m_pbIsDown[1 - m_kKeyboard.current_queue];

	m_kMouseData.current_queue = 1 - m_kMouseData.current_queue;
	m_kMouseData.m_pbIsDown[m_kMouseData.current_queue] = m_kMouseData.m_pbIsDown[1 - m_kMouseData.current_queue];
	
	if (m_kMouseData.m_bScrolled)
	{
		m_kMouseData.m_bScrolled = false;
	}
	else
	{
		m_kMouseData.m_fWheelDelta = 0;
	}

	if (m_kMouseData.m_bHasMoved)
	{
		m_kMouseData.m_bHasMoved = false;
	}
	else
	{
		m_kMouseData.m_kPositionDelta = ivec2(0);
	}
}

void InputManager::Update(double fElapsedTime)
{
	Inspect();
}

bool InputManager::IsKeyboardButtonPressed(sf::Keyboard::Key key)
{
	return m_kKeyboard.m_pbIsDown[m_kKeyboard.current_queue][key];
}

bool InputManager::IsKeyboardButtonJustDown(sf::Keyboard::Key key)
{
	return (m_kKeyboard.m_pbIsDown[m_kKeyboard.current_queue][key]) &&
		(m_kKeyboard.m_pbIsDown[1 - m_kKeyboard.current_queue][key] != 
			m_kKeyboard.m_pbIsDown[m_kKeyboard.current_queue][key]);
}

bool InputManager::IsKeyboardButtonJustRelease(sf::Keyboard::Key key)
{
	return (!m_kKeyboard.m_pbIsDown[m_kKeyboard.current_queue][key]) &&
		(m_kKeyboard.m_pbIsDown[1 - m_kKeyboard.current_queue][key] !=
			m_kKeyboard.m_pbIsDown[m_kKeyboard.current_queue][key]);
}

bool InputManager::IsMouseButtonPressed(sf::Mouse::Button key)
{
	return m_kMouseData.m_pbIsDown[m_kMouseData.current_queue][key];
}

bool InputManager::IsMouseButtonJustDown(sf::Mouse::Button key)
{
	return (m_kMouseData.m_pbIsDown[m_kMouseData.current_queue][key]) &&
		(m_kMouseData.m_pbIsDown[1 - m_kMouseData.current_queue][key] !=
			m_kMouseData.m_pbIsDown[m_kMouseData.current_queue][key]);
}

bool InputManager::IsMouseButtonJustRelease(sf::Mouse::Button key)
{
	return (!m_kMouseData.m_pbIsDown[m_kMouseData.current_queue][key]) &&
		(m_kMouseData.m_pbIsDown[1 - m_kMouseData.current_queue][key] !=
			m_kMouseData.m_pbIsDown[m_kMouseData.current_queue][key]);
}

float InputManager::GetMouseWheelDelta()
{
	return m_kMouseData.m_fWheelDelta;
}

ivec2 InputManager::GetMousePosition()
{
	return m_kMouseData.m_kPosition;
}

ivec2 InputManager::GetMousePositionDelta()
{
	return m_kMouseData.m_kPositionDelta;
}

void InputManager::Inspect()
{
	ImGui::Begin("Inputs management");

	if (ImGui::Button("Save settings"))
	{
		SaveSettings();
	}

	ImGui::SameLine();
	if (ImGui::Button("Reload settings"))
	{
		LoadSettings();
	}

	static bool bShowKeyboard = false;
	static bool bShowMouse = false;
	const char* buttonCheckMethod[] = { "Just pressed", "Just released", "pressed" };

	ImGui::Checkbox("Show keys", &bShowKeyboard);

	if (bShowKeyboard)
	{
		
		static int value = 0;
		ImGui::Combo("Check method for keyboard buttons", &value, buttonCheckMethod, 3);

		for (UI32 i = 0; i < sf::Keyboard::Key::KeyCount; ++i)
		{
			if ((i & 15) != 0)
			{
				ImGui::SameLine();
			}
			bool val;
			switch (value)
			{
			case 0:
				val = IsKeyboardButtonJustDown((sf::Keyboard::Key)i);
				break;
			case 1:
				val = IsKeyboardButtonJustRelease((sf::Keyboard::Key)i);
				break;
			case 2:
				val = IsKeyboardButtonPressed((sf::Keyboard::Key)i);
				break;
			}
			ImGui::Checkbox(pcKeyboardKeysNames[i], &val);
		}
	}
	
	ImGui::Separator();
	ImGui::Checkbox("Show mouse", &bShowMouse);

	if (bShowMouse)
	{
		ImGui::InputInt2("Mouse position", m_kMouseData.m_kPosition.GetData(), ImGuiInputTextFlags_ReadOnly);
		ImGui::InputInt2("Mouse position delta", m_kMouseData.m_kPositionDelta.GetData(), ImGuiInputTextFlags_ReadOnly);
		ImGui::InputFloat("Mouse wheel delta", &m_kMouseData.m_fWheelDelta, 0, 0, -1, ImGuiInputTextFlags_ReadOnly);

		static int valueMouse = 0;
		ImGui::Combo("Check method for mouse buttons", &valueMouse, buttonCheckMethod, 3);
		for (UI32 i = 0; i < sf::Mouse::Button::ButtonCount; ++i)
		{
			if ((i & 15) != 0)
			{
				ImGui::SameLine();
			}

			bool val;
			switch (valueMouse)
			{
			case 0:
				val = IsMouseButtonJustDown((sf::Mouse::Button)i);
				break;
			case 1:
				val = IsMouseButtonJustRelease((sf::Mouse::Button)i);
				break;
			case 2:
				val = IsMouseButtonPressed((sf::Mouse::Button)i);
				break;
			}
			ImGui::Checkbox(pcMouseButtonsNames[i], &val);
		}
	}


	ImGui::Separator();
	ImGui::Text("%s", "Stringified commands");
	for (UI32 it = 0; it < m_akAssociativeArray.size(); ++it)
	{
		ImGui::PushID(it);
		m_akAssociativeArray[it].Inspect();
		ImGui::PopID();
	}

	if (ImGui::Button("Add stringified command"))
	{
		m_akAssociativeArray.push_back({ "New command", sf::Keyboard::Key::A });
	}

	ImGui::End();
}

void InputManager::SaveSettings()
{
	std::ofstream ofstr(m_kConfigFilename);
	cereal::XMLOutputArchive output(ofstr);
	output(*this);
}

void InputManager::LoadSettings()
{
	try
	{
		std::ifstream ofstr(m_kConfigFilename);
		cereal::XMLInputArchive output(ofstr);
		output(*this);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	
}

void InputManager::StringifiedCommand::Inspect()
{
	char buff[128];
	strcpy(buff, m_kName.c_str());

	ImGui::Indent();
	ImGui::InputText("Name", buff, 128);
	m_kName = buff;

	I32 iKey = key;

	if (ImGui::Combo("Key", &iKey, pcKeyboardKeysNames, OGL_ARRAYSIZE(pcKeyboardKeysNames)))
	{
		key = (sf::Keyboard::Key)iKey;
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
		///todo
	}
	ImGui::Separator();
	ImGui::Unindent();
}
