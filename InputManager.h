#pragma once

const char* pcKeyboardKeysNames[];

class InputManager : public Singleton<InputManager>
{
	friend class Singleton<InputManager>;
protected:
	InputManager();
	~InputManager();

public:
	void Initialise();
	void HandleKeyboardMessages(sf::Event event);
	void Update(double fElaspsedTime);

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

	std::vector<bool> m_pbIsDown[2];
	UI32 current_queue;

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