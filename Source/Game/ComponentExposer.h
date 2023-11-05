#pragma once
#include <functional>
#include "IComponent.h"
#include "ComponentIDManager.h"
#include "DynamicStringBuffer.h"

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
		Scalar,
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
			id = ++Expose::idCounter;
		}
		virtual ~IExposed() = default;
		virtual void OnImGui() = 0;
		void PrepareImGui();

	public:
		eDataFormat format;
		ePickMode pickMode;
		eBounds boundsType = eBounds::None;
		int id;
		float sensitivity;
		float2 bounds;
		std::string name;
	};

	class ExposedVariable : public IExposed
	{
	public:
		~ExposedVariable();
		void OnImGui() override;

	public:
		void* adr = nullptr;

	private:
		float InBoundsValue(float aValue);
		void EditBool();
		void EditScalar();
		void EditVec2();
		void EditVec3();
		void EditVec4();
		void EditString();
	};

	class ExposedComponentRef : public IExposed
	{
	public:
		ExposedComponentRef() = delete;
		ExposedComponentRef(GameManager* aGameManager);

		void OnImGui() override;

		template<typename ComponentType>
		EnableFunctionIfTypeIsDerived(IComponent, ComponentType, void)
			StartEditComponentRef()
		{
			EditComponentRef();
		}

		const std::string& GetComponentName(uint anID);

	public:
		void** adr = nullptr;
		uint entityID = INVALID_ENTITY;
		uint componentID = NULL;
		std::string componentName = "NULL";
		std::function<void(ExposedComponentRef&)> editFunc;
	
	private:	
		void EditComponentRef();
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
	void OnImGui(const std::string& aName);
	void Update() { /*TODO: Fix variable update here*/ }
	bool HasExposedVariables() { return !myExposedVariables.empty(); }

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
		ecrr.format = Expose::eDataFormat::ComponentRef;
		ecrr.name = aName;

		auto& idManager = Singleton<ComponentIDManager>();
		ecrr.componentID = idManager.GetID<ComponentType>();
		ecrr.componentName = ecrr.GetComponentName(ecrr.componentID);

		ecrr.editFunc = [&](Expose::ExposedComponentRef& ecr) 
		{ 
			ecr.StartEditComponentRef<ComponentType>();
		};
		
		myExposedVariables.push_back(ecr);
	}

private:
	GameManager* myGameManager;
	std::vector<sptr(Expose::IExposed)> myExposedVariables;
};
