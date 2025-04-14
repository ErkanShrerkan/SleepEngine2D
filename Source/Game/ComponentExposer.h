#pragma once

#include "IComponent.h"
#include "ComponentIDManager.h"
#include "DynamicStringBuffer.h"

#include <Engine/JsonDocument.h>
#include <ThirdParty\ImGui\imgui.h>

#include <functional>

typedef DynamicStringBuffer ExposableString;

class GameManager;
class Component;

namespace Expose
{
	static int idCounter = INT_MIN;

	enum class ePickMode : char
	{
		Drag,
		Color
	};

	enum class eBounds : char 
	{
		None,
		Loop,
		Clamp
	};

	enum class eDataFormat : char
	{
		Bool,
		Float,
		Vec2,
		Vec3,
		Vec4,
		String,
		ComponentRef,
	};

	class IExposed
	{
	public:
		IExposed()
		{
			myId = ++Expose::idCounter;
		}

		virtual ~IExposed() = default;
		virtual void Edit() = 0;
		virtual rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) = 0;

		void PrepareImGui();
		void OnImGui();

		float InBoundsValue(float aValue);

		std::string GetDataFormatAsString(eDataFormat format);

	public:
		eDataFormat myFormat;
		ePickMode myPickMode;
		eBounds myBoundsType = eBounds::None;
		int myId;
		float mySensitivity;
		float2 myBounds;
		std::string myName;
	};

	class ExposedBool : public IExposed
	{
	public:
		void Edit() override;

		rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;

	public:
		void* adr = nullptr;
	};

	class ExposedFloat : public IExposed
	{
	public:
		void Edit() override;
		rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;

	public:
		void* adr = nullptr;
	};

	class ExposedVec2 : public IExposed
	{
	public:
		void Edit() override;
		rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;

	public:
		void* adr = nullptr;
	};

	class ExposedVec3 : public IExposed
	{
	public:
		void Edit() override;
		rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;

	public:
		void* adr = nullptr;
	};

	class ExposedVec4 : public IExposed
	{
	public:
		void Edit() override;
		rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;

	public:
		void* adr = nullptr;
	};

	class ExposedString : public IExposed
	{
	public:
		void Edit() override;
		rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;

	public:
		void* adr = nullptr;
	};

	class ExposedComponentRef : public IExposed
	{
	public:
		ExposedComponentRef() = delete;
		ExposedComponentRef(GameManager* aGameManager)
			: myGameManager(*aGameManager) { }

		void Edit() override;
		rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator) override;

		const std::string& GetComponentName(uint anID);

	public:
		void** adr = nullptr;
		uint entityID = INVALID_ENTITY;
		uint componentID = NULL;
		std::string componentName = "NULL";
	
	private:	
		Component*& GetComponentPtr();
		void AcceptDragDropPayLoad();

	private:
		GameManager& myGameManager;
	};
}

class ComponentExposer
{
public:
	~ComponentExposer();

	void SetGameManager(GameManager* aGameManager);
	void OnImGuiBegin(const std::string& aName);
	void OnImGui();
	
	void Update() { /*TODO: Fix variable update here*/ }
	bool HasExposedVariables() { return !myExposedVariables.empty(); }

	rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator);

	void Expose(
		bool& aVariable,
		const std::string& aName);

	void Expose(
		float& aVariable,
		const std::string& aName,
		float aSensitivity,
		Expose::eBounds aBoundsType = Expose::eBounds::None,
		float2 someBounds = { 0, 100 });

	void Expose(
		float2& aVariable,
		const std::string& aName,
		float aSensitivity,
		Expose::eBounds aBoundsType = Expose::eBounds::None,
		float2 someBounds = { 0, 100 });

	void Expose(
		float3& aVariable,
		const std::string& aName,
		float aSensitivity,
		Expose::ePickMode aPickMode = Expose::ePickMode::Drag,
		Expose::eBounds aBoundsType = Expose::eBounds::None,
		float2 someBounds = { 0, 100 });

	void Expose(
		float4& aVariable,
		const std::string& aName,
		float aSensitivity,
		Expose::ePickMode aPickMode = Expose::ePickMode::Color,
		Expose::eBounds aBoundsType = Expose::eBounds::None,
		float2 someBounds = { 0, 100 });

	void Expose(
		ExposableString& aVariable,
		const std::string& aName);

	template <typename ComponentType>
	EnableFunctionIfTypeIsDerived(IComponent, ComponentType, void)
		Expose(
			ComponentType*& aComponentRef,
			const std::string& aName)
	{
		aComponentRef = nullptr;
		sptr(Expose::ExposedComponentRef) ecr =
			std::make_shared<Expose::ExposedComponentRef>(myGameManager);

		auto& ecrr = *ecr;
		ecrr.adr = (void**)&aComponentRef;
		ecrr.myFormat = Expose::eDataFormat::ComponentRef;
		ecrr.myName = aName;

		auto& idManager = Singleton<ComponentIDManager>();
		ecrr.componentID = idManager.GetID<ComponentType>();
		ecrr.componentName = ecrr.GetComponentName(ecrr.componentID);
		
		myExposedVariables.push_back(ecr);
	}

private:
	bool isOpen = false;
	GameManager* myGameManager;
	std::vector<sptr(Expose::IExposed)> myExposedVariables;
};
