#pragma once
#include <ThirdParty\ImGui\imgui.h>

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

class ExposedComponent
{
public:
	void OnImGui(const std::string& aName);
	void Update()
	{
		// TODO: Fix variable update here
	}
	bool HasExposedVariables() { return !myExposedVariables.empty(); }

protected:
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

protected:
	void Expose(
		bool& aVariable, 
		const std::string& aName);
	void Expose(
		float& aVariable,
		const std::string& aName,
		float aSensitivity,
		eBounds aBoundsType = eBounds::None,
		float2 someBounds = { 0, 100 });
	void Expose(
		float2& aVariable,
		const std::string& aName,
		float aSensitivity,
		eBounds aBoundsType = eBounds::None,
		float2 someBounds = { 0, 100 });
	void Expose(
		float3& aVariable,
		const std::string& aName,
		float aSensitivity,
		ePickMode aPickMode = ePickMode::Color,
		eBounds aBoundsType = eBounds::None,
		float2 someBounds = { 0, 100 });
	void Expose(
		float4& aVariable,
		const std::string& aName,
		float aSensitivity,
		ePickMode aPickMode = ePickMode::Color,
		eBounds aBoundsType = eBounds::None,
		float2 someBounds = { 0, 100 });
	void Expose(
		ExposableString& aVariable, 
		const std::string& aName);

private:
	enum class eDataFormat
	{
		Bool,
		Scalar,
		Vec2,
		Vec3,
		Vec4,
		String
	};

	class ExposedVariable
	{
	public:
		void OnImGui();

	public:
		std::string name;
		eDataFormat format;
		ePickMode pickMode;
		eBounds boundsType = eBounds::None;
		void* adr;
		float2 bounds;
		float sensitivity;

	private:
		float InBoundsValue(float aValue);

	protected:
		void Bool();
		void Scalar();
		void Vec2();
		void Vec3();
		void Vec4();
		void String();
	};

private:
	std::vector<ExposedVariable> myExposedVariables;
};

