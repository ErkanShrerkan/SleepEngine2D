#pragma once
#include <functional>
#include "IComponent.h"
#include "ComponentIDManager.h"

class GameManager;
class Component;

class ExposableString
{
public:
	ExposableString();
	ExposableString(const std::string& aString);
	ExposableString(uint aSize);
	void SetString(const std::string& aString);
	void SetSize(uint aSize);
	uint GetSize();
	std::string GetString();
	char* operator[](uint anIndex);

private:
	std::vector<char> buf;
};

namespace Expose
{
	enum class ePickMode
	{
		Drag,
		Color
	};

	enum class eBounds
	{
		None,
		Loop,
		Clamp
	};

	enum class eDataFormat
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
		virtual ~IExposed() = default;
		virtual void OnImGui() = 0;
		void PrepareImGui();

	public:
		std::string name;
		eDataFormat format;
		ePickMode pickMode;
		eBounds boundsType = eBounds::None;
		float2 bounds;
		float sensitivity;
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
			auto& idManager = Singleton<ComponentIDManager>();
			uint componentID = idManager.GetID<ComponentType>();
			const std::string& componentName = GetComponentName(componentID);
			EditComponentRef(componentName);
		}

	public:
		void* adr = nullptr;
		std::function<void(ExposedComponentRef&)> editFunc;
	
	private:	
		void EditComponentRef(const std::string& aComponentName);
		Component*& GetComponentPtr();
		const std::string& GetComponentName(uint anID);

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
		Expose::ePickMode aPickMode = Expose::ePickMode::Color,
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
		ecrr.adr = &aComponentRef;
		ecrr.format = Expose::eDataFormat::ComponentRef;
		ecrr.name = aName;
		ecrr.editFunc = [&](Expose::ExposedComponentRef& ecr) { ecr.StartEditComponentRef<ComponentType>(); };
		
		myExposedVariables.push_back(ecr);
	}

private:
	std::vector<sptr(Expose::IExposed)> myExposedVariables;
	GameManager* myGameManager;
};