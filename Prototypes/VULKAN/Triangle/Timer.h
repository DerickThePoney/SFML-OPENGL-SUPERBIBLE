#pragma once

namespace VkEngine
{
	template<typename period>
	class Timer
	{
	public:
		Timer(const std::string& name = "default")
			: m_kTimerName(name), m_kStart(std::chrono::high_resolution_clock::now())
		{

		}
		~Timer()
		{
			auto endTime = std::chrono::high_resolution_clock::now();
			float elapsed = std::chrono::duration<float, period>(endTime - m_kStart).count();

			std::cout << "Timer named " << m_kTimerName << " recorded elpased:" << elapsed << std::endl;
		}

	private:
		std::chrono::high_resolution_clock::time_point m_kStart;
		std::string m_kTimerName;
	};

}