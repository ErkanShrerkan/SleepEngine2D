#include "pch.h"
#include "Keyboard.h"
#include <Windows.h>
#include <algorithm>

#define IS_KEY_DOWN(key) GetAsyncKeyState(key)

Keyboard::Keyboard()
{
}

Keyboard::~Keyboard()
{
	DeInit();
}

bool Keyboard::GetKeyPressed(uint aKey)
{
	if (!KeyExists(aKey))
		return false;

	return (myKeyStates[aKey] == eInputState::Pressed);
}

bool Keyboard::GetKeyHeld(uint aKey)
{
	if (!KeyExists(aKey))
		return false;

	return (myKeyStates[aKey] == eInputState::Held);
}

bool Keyboard::GetKeyReleased(uint aKey)
{
	if (!KeyExists(aKey))
		return false;

	return (myKeyStates[aKey] == eInputState::Released);
}

bool Keyboard::GetKeyDown(uint aKey)
{
	if (!KeyExists(aKey))
		return false;

	return GetKeyPressed(aKey) || GetKeyHeld(aKey);
}

Vector2f Keyboard::GetDirection()
{
	float x = 0, y = 0;

	y += (int)(bool)(GetKeyDown(VK_UP) + GetKeyDown('W'));
	x -= (int)(bool)(GetKeyDown(VK_LEFT) + GetKeyDown('A'));
	y -= (int)(bool)(GetKeyDown(VK_DOWN) + GetKeyDown('S'));
	x += (int)(bool)(GetKeyDown(VK_RIGHT) + GetKeyDown('D'));

	return Vector2f({ x, y }).GetNormalized();
}

std::vector<KeyUpdate> Keyboard::Update()
{
	std::vector<KeyUpdate> keyUpdates;
	for (auto& [key, state] : myKeyStates)
	{
		switch (state)
		{
		case eInputState::Neutral:
			if (IS_KEY_DOWN(key))
			{
				state = eInputState::Pressed;
				keyUpdates.push_back({ eInputState::Pressed, key});
			}
			else
			{
				keyUpdates.push_back({ eInputState::Neutral, key });
			}
			break;
		case eInputState::Pressed:
			if (!(IS_KEY_DOWN(key)))
			{
				state = eInputState::Released;
				keyUpdates.push_back({ eInputState::Released, key });
			}
			else
			{
				state = eInputState::Held;
				keyUpdates.push_back({ eInputState::Held, key });
			}
			break;
		case eInputState::Held:
			if (!(IS_KEY_DOWN(key)))
			{
				state = eInputState::Released;
				keyUpdates.push_back({ eInputState::Released, key });
			}
			else
			{
				keyUpdates.push_back({ eInputState::Held, key });
			}
			break;
		case eInputState::Released:
			state = eInputState::Neutral; 
			keyUpdates.push_back({ eInputState::Neutral, key });
			break;
		}
	}

	return keyUpdates;
}


void Keyboard::DeInit()
{
	myKeyStates.clear();
	//myKeys.clear();
}

bool Keyboard::KeyExists(unsigned int aKey)
{
	return myKeyStates.find(aKey) != myKeyStates.end();
}
