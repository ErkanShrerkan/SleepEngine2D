#pragma once
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <map>
#include "Component.h"
#include "System.h"
#include <type_traits>
#include <set>

namespace Game
{
	class Editor;
}

class SceneManager;
class GameManager;
class Entity;

class IComponentMap
{
public:
	virtual ~IComponentMap() = default;
	virtual void DeleteComponentFromEntity(uint anEntity) = 0;
	virtual void UpdateComponents() = 0;
	virtual void* RegisterNewComponent(uint anEntity) = 0;
	virtual void AddComponent(uint anEntity, GameManager& aGM) = 0;
	std::string GetName()
	{
		return myName;
	}

public:
	std::string myName;
	bool myIsEditorOnly;
};

template <
	typename ComponentType,
	typename std::enable_if<
	std::is_base_of<
	Component, ComponentType>::value>::type* = nullptr>
	class ComponentMap;

class GameManager
{
	friend class Entity;
	friend class SceneManager;
	friend class Game::Editor;
public:
	GameManager();
	~GameManager();
	void Init();
	void Update();

	/// <summary>
	/// Gets ID of component classes only
	/// </summary>
	/// <typeparam name="ComponentType"></typeparam>
	/// <returns></returns>
	template <class ComponentType>
	typename std::enable_if
		<std::is_base_of
		<Component, ComponentType>::value,
		uint>::type
		GetID()
	{
		uint count = myIDCounter++;
		static uint componentId = count;
		return componentId;
	}

	template <typename ComponentType>
	typename std::enable_if<
		std::is_base_of<
		Component, ComponentType>::value,
		ComponentMap<ComponentType>&>::type
		GetComponentMap()
	{
		return *static_cast<ComponentMap<ComponentType>*>(myComponentMaps[GetID<ComponentType>()]);
	}

	Entity& GetEntity(uint anEntityID)
	{
		return *myEntities.at(anEntityID);
	}

	std::unordered_map<uint, Entity*>& GetEntities()
	{
		return myEntities;
	}

	template <typename ComponentType, typename... Args>
	typename std::enable_if<
		std::is_base_of<
		Component, ComponentType>::value,
		ComponentType&>::type
		AddComponent(uint anEntityID, Args&&... someArgs)
	{
		//ComponentType* component = nullptr;
		uint componentID = GetID<ComponentType>();
		auto& entityComponents = myEntityComponents[anEntityID];

		if (entityComponents.find(componentID) != entityComponents.end())
			return *static_cast<ComponentType*>(myEntityComponents[anEntityID][componentID]);

		Entity* entity = myEntities[anEntityID];
		ComponentType* component = reinterpret_cast<ComponentType*>(GetComponentMap<ComponentType>().RegisterNewComponent(anEntityID));
		*component = ComponentType(std::forward<Args>(someArgs)...);
		component->SetEntity(entity);

		entityComponents[componentID] = component;
		component->Start();
		return *component;
	}

private:
	template <typename ComponentType>
	typename std::enable_if<
		std::is_base_of<
		Component, ComponentType>::value,
		void>::type
		RegisterComponent(const std::string& aName, bool isEditorOnly = false)
	{
		uint id = GetID<ComponentType>();
		auto& map = myComponentMaps[id];
		map = new ComponentMap<ComponentType>();
		map->myName = aName;
		map->myIsEditorOnly = isEditorOnly;
	}

	Entity& CreateEntity();
	void RemoveEntity(uint anEntityID);
	void MarkEntityForRemoval(uint anEntityID);
	void UnLoadAll();

	template <typename ComponentType>
	typename std::enable_if<
		std::is_base_of
		<Component, ComponentType>::value,
		ComponentType*>::type
		GetComponent(uint anEntityID)
	{
		auto& map = GetComponentMap<ComponentType>().map;
		auto findIt = map.find(anEntityID);
		if (findIt == map.end())
		{
			return nullptr;
		}
		else
		{
			return &findIt->second;
		}
	}

	template <typename SystemType>
	typename std::enable_if
		<std::is_base_of
		<System, SystemType>::value,
		void>::type
		RegisterSystem()
	{
		mySystems.push_back(new SystemType(this));
	}

private:
	uint myNextEntityID = 0;
	uint myIDCounter = 0;

	std::unordered_map<uint, Entity*> myEntities;
	// myEntityComponents is just a register to quickly access all the entity's components
	// first key is the entity's ID, and the second key is the component's ID
	std::unordered_map<uint, std::unordered_map<uint, Component*>> myEntityComponents;
	// IComponentMap is in charge of cleaning up component pointers
	// key is the component's ID
	std::unordered_map<uint, IComponentMap*> myComponentMaps;
	std::vector<System*> mySystems;
	std::set<uint> myEntitiesToRemove;
	SceneManager* mySceneManager;

private:
	void UpdateEntityRemoval();
	void UpdateSystems();
	void UpdateComponents();
	void UpdateHierarchy(bool aBool = true);
	bool GetUpdateHierarchy() { return myEntityHierarchyNeedsUpdating; }

	std::unordered_map<uint, std::unordered_map<uint, Component*>>& GetEntityComponents() { return myEntityComponents; }
	std::unordered_map<uint, IComponentMap*>& GetComponentMaps() { return myComponentMaps; }
private:
	bool myEntityHierarchyNeedsUpdating = true;
};

template <
	typename ComponentType,
	typename std::enable_if<
	std::is_base_of<
	Component, ComponentType>::value>::type*>
	class ComponentMap : public IComponentMap
{
public:
	std::unordered_map<uint, ComponentType> map;

public:
	~ComponentMap()
	{
	}

	virtual void DeleteComponentFromEntity(uint anEntity) override
	{
		if (map.find(anEntity) == map.end())
			return;

		map.erase(anEntity);
	}

	virtual void UpdateComponents() override
	{
		// if Update function is not overrided it is not called
		if (!OVERRIDED(Component, ComponentType, Update))
			return;

		for (auto& [entity, component] : map)
		{
			component.Update();
		}
	}

	virtual void* RegisterNewComponent(uint anEntity) override
	{
		auto findIt = map.find(anEntity);
		if (findIt != map.end())
			return &(findIt->second);

		return  reinterpret_cast<void*>(&map[anEntity]);
	}

	virtual void AddComponent(uint anEntity, GameManager& aGM) override
	{
		aGM.AddComponent<ComponentType>(anEntity);
		return;
	}
};