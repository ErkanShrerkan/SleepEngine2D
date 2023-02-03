#pragma once

class Process
{
public:
	virtual bool Init() = 0;
	virtual bool Update() = 0;
	virtual ~Process(){}

protected:
	bool myIsRunning = true;
	bool myIsPaused = false;
};

