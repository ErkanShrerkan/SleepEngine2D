#pragma once
#include <unordered_map>
#include "InputData.h"

struct Key
{
	Key(uint aKey)
		:key(aKey)
	{}

	Key(){}

	uint key = 0;
};

struct KeyUpdate
{
	eInputState state;
	uint key;
};

class Keyboard
{
public:
	Keyboard();
	~Keyboard();
	bool GetKeyPressed(uint aKey);
	bool GetKeyHeld(uint aKey);
	bool GetKeyReleased(uint aKey);
	bool GetKeyDown(uint aKey);
	Vector2f GetDirection();
	std::vector<KeyUpdate> Update();

	void AddKey(uint aKey)
	{ 
		myKeyStates[aKey] = eInputState::Neutral;
	};

	void DeInit();

private:
	bool KeyExists(unsigned int aKey);
	std::unordered_map<uint, eInputState> myKeyStates;
	//std::unordered_map<uint, Key> myKeys;
};
