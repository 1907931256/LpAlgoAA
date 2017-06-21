#include "stdafx.h"
#include "MyTimercount.h"


CMyTimercount::CMyTimercount()
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_CounterFrequency);
	Reset();
}

CMyTimercount::~CMyTimercount()
{
}

void CMyTimercount::Reset()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&m_ResetTime);
}

float CMyTimercount::ComputeTime(BOOL bReset)
{
	LONGLONG currentTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);

	// Compute difference from last reset
	LONGLONG diffTime = currentTime - m_ResetTime;

	// Reset time
	if (bReset) m_ResetTime = currentTime;

	// Return time in seconds
	return (float)diffTime / (float)m_CounterFrequency;
}

float CMyTimercount::GetNumber()
{
	LONGLONG currentTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);

	// Return time in seconds
	return (float)currentTime / (float)m_CounterFrequency;
}
