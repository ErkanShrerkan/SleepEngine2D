#pragma once
#include <Engine\Keyboard.h>


class InputDispatcher
{
public:
	void Dispatch();
	void AddInputObserver();
private:
	std::vector<KeyUpdate> myKeyUpdatesToDispatch;
};

