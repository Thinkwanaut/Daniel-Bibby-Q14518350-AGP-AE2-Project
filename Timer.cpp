#include "Timer.h"

Timer::Timer()
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0f / countsPerSec;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_FrameStart);
}

// Get frame time and return lag multiplier based on target framerate
float Timer::Tick()
{
	__int64 frameEnd;
	QueryPerformanceCounter((LARGE_INTEGER*)&frameEnd);
	float gap = (frameEnd - m_FrameStart) * m_SecondsPerCount;
	m_FrameStart = frameEnd;
	return m_TargetFps / (1.0f / gap);
}

void Timer::StartTimer(std::string name)
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_Timers[name]);
}

float Timer::GetTimer(std::string name)
{
	if (m_Timers.find(name) == m_Timers.end()) return -1;
	__int64 curTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&curTime);
	return (curTime - m_Timers[name]) * m_SecondsPerCount;
}
