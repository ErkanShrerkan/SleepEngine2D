#pragma once

class Component;
class ComponentIDManager
{
public:
	template <class ComponentType>
	EnableFunctionIfTypeIsDerived(Component, ComponentType, uint)
		GetID()
	{
		uint count = myIDCounter++;
		static uint componentId = count;
		return componentId;
	}

private:
	uint myIDCounter = 0;
};

