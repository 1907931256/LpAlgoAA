#pragma once
class CMyTimercount
{
public:
	CMyTimercount();
	~CMyTimercount();

public:
	void Reset();
	float GetTime(BOOL bReset) { return ComputeTime(bReset) * 1.0f; }
	float GetTimeMilli(BOOL bReset) { return ComputeTime(bReset) * 1000.0f; }
	float GetTimeMicro(BOOL bReset) { return ComputeTime(bReset) * 1000000.0f; }
	float GetNumber();

protected:
	LONGLONG m_CounterFrequency;
	LONGLONG m_ResetTime;

private:
	float ComputeTime(BOOL bReset);
};

