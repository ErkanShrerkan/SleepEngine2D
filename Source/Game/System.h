#pragma once

class GameManager;
class System
{
public:
	System(GameManager* aGM)
		: myGameManager(aGM)
	{
        //myComponentTypes = { typeid(ComponentTypes)... };
	}

	virtual ~System() = default;

	virtual void Update() = 0;

protected:
    std::vector<std::type_info> myComponentTypes;
	GameManager* myGameManager;
};

