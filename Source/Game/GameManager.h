#pragma once
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <map>
#include <type_traits>
#include <set>

#include "ComponentIDManager.h"
#include "Component.h"
#include "System.h"

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

	const std::string& GetName()
	{
		return myName;
	}

public:
	std::string myName;
	bool myIsEditorOnly;
};

template <typename ComponentType, typename std::enable_if<
	std::is_base_of<Component, ComponentType>
	::value>::type* = nullptr>
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

	// Scene Manager Interface
	void SaveScene();
	void SaveSceneAs(const std::string& aSceneName);
	void LoadScene(const std::string& aSceneName);

	/// <summary>
	/// Gets ID of component classes only
	/// </summary>
	/// <typeparam name="ComponentType"></typeparam>
	/// <returns></returns>
	template <class ComponentType>
	EnableFunctionIfTypeIsDerived(Component, ComponentType, uint)
		GetID()
	{
		return Singleton<ComponentIDManager>().GetID<ComponentType>();
	}

	template <typename ComponentType>
	EnableFunctionIfTypeIsDerived(Component, ComponentType, ComponentMap<ComponentType>&)
		GetComponentMap()
	{
		return *static_cast<ComponentMap<ComponentType>*>(myComponentMaps.at(GetID<ComponentType>()).get());
	}

	const std::string& GetComponentTypeNameByID(uint anID)
	{
		return myComponentMaps[anID]->GetName();
	}

	Entity& GetEntity(uint anEntityID)
	{
		return *myEntities.at(anEntityID);
	}

	std::unordered_map<uint, sptr(Entity)>& GetEntities()
	{
		return myEntities;
	}

	template <typename ComponentType, typename... Args>
	EnableFunctionIfTypeIsDerived(Component, ComponentType, ComponentType&)
		AddComponent(uint anEntityID, Args&&... someArgs)
	{
		uint componentID = GetID<ComponentType>();
		auto& entityComponents = myEntityComponents[anEntityID];
		void* newComponent = nullptr;

		if (entityComponents.find(componentID) != entityComponents.end())
		{
			auto& components = myEntityComponents[anEntityID];
			newComponent = components[componentID];
			return *(ComponentType*)newComponent;
		}

		sptr(Entity) entity = myEntities[anEntityID];
		auto& cm = GetComponentMap<ComponentType>();
		newComponent = cm.RegisterNewComponent(anEntityID);
		ComponentType* component = (ComponentType*)newComponent;
		*component = ComponentType(std::forward<Args>(someArgs)...);
		component->SetEntity(entity);

		entityComponents[componentID] = component;
		component->Start();
		return *component;
	}

	std::unordered_map<uint, Component*>& GetComponentsFromEntity(uint anEntityID)
	{
		return myEntityComponents.at(anEntityID);
	}

	void RemoveRefsToComponent(Component* aComponent);
	void RegisterComponentRef(void** aRefAddress, Component* aReferencedComponent);

private:
	template <typename ComponentType>
	EnableFunctionIfTypeIsDerived(Component, ComponentType, void)
		RegisterComponent(const std::string& aName, bool isEditorOnly = false)
	{
		uint id = GetID<ComponentType>();
		auto& map = myComponentMaps[id];
		map = std::make_shared<ComponentMap<ComponentType>>();
		map->myName = aName;
		map->myIsEditorOnly = isEditorOnly;
	}

	Entity& CreateEntity();
	void RemoveEntity(uint anEntityID);
	void MarkEntityForRemoval(uint anEntityID);
	void UnLoadAll();

	template <typename ComponentType>
	EnableFunctionIfTypeIsDerived(Component, ComponentType, ComponentType*)
		GetComponent(uint anEntityID)
	{
		auto& map = GetComponentMap<ComponentType>().map;
		auto findIt = map.find(anEntityID);

		return findIt == map.end() ? nullptr : &findIt->second;
	}

	template <typename SystemType>
	EnableFunctionIfTypeIsDerived(System, SystemType, void)
		RegisterSystem()
	{
		mySystems.push_back(std::make_shared<SystemType>(this));
	}

private:
	uint myNextEntityID = 0;

	std::unordered_map<uint, sptr(Entity)> myEntities;
	// myEntityComponents is just a register to quickly access all the entity's components
	// first key is the entity's ID, and the second key is the component's ID
	std::unordered_map<uint, std::unordered_map<uint, Component*>> myEntityComponents;
	// IComponentMap is in charge of cleaning up component pointers
	// key is the component's ID
	std::unordered_map<uint, sptr(IComponentMap)> myComponentMaps;

	std::unordered_map<Component*, std::set<void**>> myRegisteredComponentRefs;

	std::vector<sptr(System)> mySystems;
	std::set<uint> myEntitiesToRemove;
	sptr(SceneManager) mySceneManager;

private:
	void UpdateEntityRemoval();
	void UpdateSystems();
	void UpdateComponents();
	void UpdateHierarchy(bool aBool = true);
	bool GetUpdateHierarchy() { return myEntityHierarchyNeedsUpdating; }

	std::unordered_map<uint, std::unordered_map<uint, Component*>>& GetEntityComponents() { return myEntityComponents; }
	std::unordered_map<uint, sptr(IComponentMap)>& GetComponentMaps() { return myComponentMaps; }
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
	~ComponentMap(){}

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