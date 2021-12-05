#include "Timer.h"

Timer::Timer()
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0f / countsPerSec;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_FrameStart);
}

float Timer::Tick()
{
	__int64 frameEnd;
	QueryPerformanceCounter((LARGE_INTEGER*)&frameEnd);
	float gap = (frameEnd - m_FrameStart) * m_SecondsPerCount;
	m_FrameStart = frameEnd;
	return m_TargetFps / (1.0f / gap);
}
