#pragma once

#include <Windows.h>
#include <map>
#include <string>
class Timer
{
private:
	double m_SecondsPerCount{ 0 };
	__int64 m_FrameStart{ 0 };
	int m_TargetFps{ 60 };

	std::map<std::string, __int64> m_Timers;

public:
	Timer();

	float TickFPS();
	float Gap();
	void StartTimer(std::string name);
	float GetTimer(std::string name);
};

