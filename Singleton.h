#pragma once

template<class T>
class Singleton
{
public:
	static T* Instance()
	{
		if (s_kInstance == nullptr)
		{
			s_kInstance = new T();
		}

		return s_kInstance;
	}

	static void Delete()
	{
		SAFE_DELETE(s_kInstance);
	}

protected:
	static T* s_kInstance;
};
