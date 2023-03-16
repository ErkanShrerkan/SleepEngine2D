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
	virtual void AddComponent(uint anEntity, GameManager& aGameManager) = 0;
	std::string GetName()
	{
		return myName;
	}

public:
	std::string myName;
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

	void OnImGui();

	template <typename ComponentType, typename... Args>
	typename std::enable_if<
		std::is_base_of<
		Component, ComponentType>::value,
		ComponentType&>::type
		AddComponent(uint anEntityID, Args&&... someArgs)
	{
		ComponentType* component = nullptr;
		auto& entityComponents = myEntityComponents[anEntityID];
		uint componentID = GetID<ComponentType>();

		if (entityComponents.find(componentID) != entityComponents.end())
		{
			component = static_cast<ComponentType*>(myEntityComponents[anEntityID][componentID]);
			return *component;
		}

		ComponentMap<ComponentType>& cm = GetComponentMap<ComponentType>();
		component = new ComponentType(std::forward<Args>(someArgs)...);
		Entity* entity = myEntities[anEntityID];
		component->SetEntity(entity);
		cm.map.insert(std::make_pair(anEntityID, component));
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
		RegisterComponent(const std::string& aName)
	{
		myComponentMaps[GetID<ComponentType>()] = new ComponentMap<ComponentType>();
		myComponentMaps[GetID<ComponentType>()]->myName = aName;
	}

	Entity& CreateEntity();
	void RemoveEntity(uint anEntityID);
	void MarkEntityForRemoval(uint anEntityID);
	void UnLoadAll();

	template <typename ComponentType>
	typename std::enable_if<
		std::is_base_of
		<Component, ComponentType>::value,
		ComponentType&>::type
		GetComponent(uint anEntityID)
	{
		return *(GetComponentMap<ComponentType>().map.at(anEntityID));
	}

	template <typename SystemType>
	typename std::enable_if
		<std::is_base_of
		<System, SystemType>::value,
		void>::type
		RegisterSystem()
	{
		mySystems.push_back(reinterpret_cast<System*>(new SystemType(this)));
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
	//// Editor Functions
	//void AddEntityComponent();
	//void AddEntity();
	//void SelectEntity();
	//void ModifyValues();
	//void ListEntityRecursive(uint anID);
	//bool ValidSelection();
	//void BuildHierarchy();
	void UpdateHierarchy(bool aBool = true);
	bool GetUpdateHierarchy() { return myEntityHierarchyNeedsUpdating; }
	//void SceneHierarchy();
	//void Inspector();
	std::unordered_map<uint, Entity*>& GetEntities() { return myEntities; }
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
	std::unordered_map<uint, ComponentType*> map;
	virtual void DeleteComponentFromEntity(uint anEntity) override
	{
		// deletes the component pointer
		delete map[anEntity];
		// removes the entity entry in the map
		map.erase(anEntity);
	}

	virtual void UpdateComponents() override
	{
		for (auto& [entity, component] : map)
		{
			component->Update();
		}
	}

	virtual void AddComponent(uint anEntity, GameManager& aGameManager) override
	{
		aGameManager.AddComponent<ComponentType>(anEntity);
	}
};