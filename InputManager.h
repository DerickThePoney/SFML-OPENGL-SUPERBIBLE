#pragma once

const char* pcKeyboardKeysNames[];
const char* pcMouseButtonsNames[];

class InputManager : public Singleton<InputManager>
{
	friend class Singleton<InputManager>;
protected:
	InputManager();
	~InputManager();

public:
	void Initialise();
	void HandleKeyboardMessages(sf::Event event);
	void HandleMouseMessages(sf::Event event);
	void PrepareNewFrame();
	void Update(double fElapsedTime);

	bool IsKeyboardButtonPressed(sf::Keyboard::Key key);
	bool IsKeyboardButtonJustDown(sf::Keyboard::Key key);
	bool IsKeyboardButtonJustRelease(sf::Keyboard::Key key);

	bool IsMouseButtonPressed(sf::Mouse::Button key);
	bool IsMouseButtonJustDown(sf::Mouse::Button key);
	bool IsMouseButtonJustRelease(sf::Mouse::Button key);
	float GetMouseWheelDelta();
	ivec2 GetMousePosition();

	void Inspect();

	void SaveSettings();
	void LoadSettings();

	template<class Archive>
	void save(Archive & archive) const
	{
		archive(m_akAssociativeArray);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		archive(m_akAssociativeArray);
	}

protected:
	std::string m_kConfigFilename;

	///Mouse
	struct Mouse
	{
		ivec2 m_kPosition;
		float m_fWheelDelta;
		bool m_bScrolled;

		std::vector<bool> m_pbIsDown[2];
		UI32 current_queue;
	};

	Mouse m_kMouseData;


	///Keyboard
	struct KeyBoard
	{
		std::vector<bool> m_pbIsDown[2];
		UI32 current_queue;
	};

	KeyBoard m_kKeyboard;



	///Special commands
	struct StringifiedCommand
	{
		std::string m_kName;
		sf::Keyboard::Key key;

		void Inspect();

		template<class Archive>
		void save(Archive & archive) const
		{
			archive(m_kName, key);
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(m_kName, key);
		}
	};

	std::vector <StringifiedCommand> m_akAssociativeArray;
};