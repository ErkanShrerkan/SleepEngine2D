#pragma once
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <map>
#include "Component.h"
#include "System.h"
#include <type_traits>

class Entity;

class IComponentMap
{
public:
	virtual ~IComponentMap() = default;
	virtual void DeleteComponentFromEntity(uint anEntity) = 0;
	virtual void UpdateComponents() = 0;

public:
	std::string myName;
};

template <
	typename ComponentType,
	typename std::enable_if<
	std::is_base_of<
	Component, ComponentType>::value>::type* = nullptr>
	class ComponentMap : public IComponentMap
{
public:
	std::unordered_map<uint, ComponentType*> map;
	virtual void DeleteComponentFromEntity(uint anEntity) override
	{
		delete map[anEntity];
		map.erase(anEntity);
	}

	virtual void UpdateComponents() override
	{
		for (auto& [entity, component] : map)
		{
			component->Update();
		}
	}
};

class GameManager
{
	friend class Entity;
public:
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
		return *myEntities[anEntityID];
	}

	void OnImGui();

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

	template <typename ComponentType, typename... Args>
	typename std::enable_if<
		std::is_base_of<
		Component, ComponentType>::value,
		ComponentType&>::type
		AddComponent(uint anEntityID, Args&&... someArgs)
	{
		ComponentMap<ComponentType>& cm = GetComponentMap<ComponentType>();
		cm.map.insert(std::make_pair(anEntityID, new ComponentType(std::forward<Args>(someArgs)..., myEntities[anEntityID])));
		ComponentType& component = *cm.map[anEntityID];
		myEntityComponents[anEntityID][GetID<ComponentType>()] = &component;
		component.Start();
		return component;
	}

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
	std::unordered_map<uint, std::unordered_map<uint, Component*>> myEntityComponents;
	std::unordered_map<uint, IComponentMap*> myComponentMaps;
	std::vector<System*> mySystems;
};