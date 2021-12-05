#pragma once

#include <Windows.h>
class Timer
{
private:
	double m_SecondsPerCount;
	__int64 m_FrameStart;
	int m_TargetFps{ 60 };

public:
	Timer();

	float Tick();
};

