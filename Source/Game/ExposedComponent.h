#pragma once
#include <ThirdParty\ImGui\imgui.h>

class Component;

namespace Expose
{
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
		void* adr;

	private:
		float InBoundsValue(float aValue);
		void Bool();
		void Scalar();
		void Vec2();
		void Vec3();
		void Vec4();
		void String();
	};

	template<typename ComponentType>
	class ExposedComponentRef : public IExposed
	{
	public:
		~ExposedComponentRef();
		void OnImGui() override;

	public:
		void* adr;

	private:
		void ComponentRef();
	};
}

class ExposedComponent
{
public:
	virtual ~ExposedComponent();
	void OnImGui(const std::string& aName);
	void Update() { /*TODO: Fix variable update here*/ }
	bool HasExposedVariables() { return !myExposedVariables.empty(); }

protected:
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
		Expose::ExposableString& aVariable,
		const std::string& aName);

	template <typename ComponentType>
	EnableFunctionIfTypeIsDerived(Component, ComponentType, void)
		Expose(
			ComponentType*& aComponentRef,
			const std::string& aName);

private:
	std::vector<sptr(Expose::IExposed)> myExposedVariables;
};
