#include "pch.h"
#include "ExposedComponent.h"

ExposableString::ExposableString()
{
	SetSize(128);
}

ExposableString::ExposableString(const std::string& aString)
{
	SetSize(128);
	SetString(aString);
}

ExposableString::ExposableString(uint aSize)
{
	SetSize(aSize);
}

void ExposableString::SetString(const std::string& aString)
{
	strncpy_s(&buf[0], buf.size(), aString.data(), buf.size() - 1);
}

void ExposableString::SetSize(uint aSize)
{
	buf.resize(aSize);
}

uint ExposableString::GetSize()
{
	return (uint)buf.size();
}

std::string ExposableString::GetString()
{
	return std::string(&buf[0]);
}

char* ExposableString::operator[](uint anIndex)
{
	return &buf[anIndex];
}

float ExposedComponent::ExposedVariable::InBoundsValue(float aValue)
{
	float returnVal = aValue;
	switch (boundsType)
	{
	case eBounds::Loop:
		returnVal += returnVal > bounds.y ? bounds.x - bounds.y : (returnVal < bounds.x ? bounds.y - bounds.x : 0.f);
		break;
	case eBounds::Clamp:
		returnVal = Math::Clamp(returnVal, bounds.x, bounds.y);
		break;
	default:
		break;
	}
	return returnVal;
}

void ExposedComponent::ExposedVariable::Bool()
{
	ImGui::Checkbox("", (bool*)adr);
}

void ExposedComponent::ExposedVariable::Scalar()
{
	ImGui::DragFloat("", (float*)adr, sensitivity);
	*(float*)adr = InBoundsValue(*(float*)adr);
}

void ExposedComponent::ExposedVariable::Vec2()
{
	ImGui::DragFloat2("", (float*)adr, sensitivity);
	float2& f2 = *(float2*)adr;
	f2.x = InBoundsValue(f2.x);
	f2.y = InBoundsValue(f2.y);
}

void ExposedComponent::ExposedVariable::Vec3()
{
	float3& f3 = *(float3*)adr;
	switch (pickMode)
	{
	case ePickMode::Drag:
		ImGui::DragFloat3("", (float*)adr, sensitivity);
		f3.x = InBoundsValue(f3.x);
		f3.y = InBoundsValue(f3.y);
		f3.z = InBoundsValue(f3.z);
		break;
	case ePickMode::Color:
		ImGui::ColorEdit3("", (float*)adr);
		break;
	default:
		break;
	}
}

void ExposedComponent::ExposedVariable::Vec4()
{
	float4& f4 = *(float4*)adr;
	switch (pickMode)
	{
	case ePickMode::Drag:
		ImGui::DragFloat4("", (float*)adr, sensitivity);
		f4.x = InBoundsValue(f4.x);
		f4.y = InBoundsValue(f4.y);
		f4.z = InBoundsValue(f4.z);
		f4.w = InBoundsValue(f4.w);
		break;
	case ePickMode::Color:
		ImGui::ColorEdit4("", (float*)adr);
		break;
	default:
		break;
	}
}

void ExposedComponent::ExposedVariable::String()
{
	ExposableString& es = *(ExposableString*)adr;
	ImGui::InputText("", es[0], es.GetSize());
}

void ExposedComponent::ExposedVariable::OnImGui()
{
	ImGui::AlignTextToFramePadding();
	ImGui::Text("  ");
	ImGui::SameLine();
	ImGui::Text(name.c_str());
	ImGui::TableSetColumnIndex(1);
	ImGui::SetNextItemWidth(-FLT_MIN);
	switch (format)
	{
	case eDataFormat::Bool:
		Bool();
		break;
	case eDataFormat::Scalar:
		Scalar();
		break;
	case eDataFormat::Vec2:
		Vec2();
		break;
	case eDataFormat::Vec3:
		Vec3();
		break;
	case eDataFormat::Vec4:
		Vec4();
		break;
	case eDataFormat::String:
		String();
		break;
	default:
		break;
	}
}

void ExposedComponent::Expose(
	bool& aVariable, 
	const std::string& aName)
{
	ExposedVariable ev;
	ev.adr = &aVariable;
	ev.format = eDataFormat::Bool;
	ev.name = aName;
	myExposedVariables.push_back(ev);
}

void ExposedComponent::Expose(
	float& aVariable, 
	const std::string& aName, 
	float aSensitivity, 
	eBounds aBoundsType, 
	float2 someBounds)
{
	ExposedVariable ev;
	ev.adr = &aVariable;
	ev.format = eDataFormat::Scalar;
	ev.name = aName;
	ev.pickMode = ePickMode::Drag;

	ev.sensitivity = aSensitivity;
	ev.boundsType = aBoundsType;
	ev.bounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ExposedComponent::Expose(
	float2& aVariable, 
	const std::string& aName, 
	float aSensitivity, 
	eBounds aBoundsType, 
	float2 someBounds)
{
	ExposedVariable ev;
	ev.adr = &aVariable.x;
	ev.format = eDataFormat::Vec2;
	ev.name = aName;
	ev.pickMode = ePickMode::Drag;

	ev.sensitivity = aSensitivity;
	ev.boundsType = aBoundsType;
	ev.bounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ExposedComponent::Expose(
	float3& aVariable, 
	const std::string& aName, 
	float aSensitivity,
	ePickMode aPickMode,
	eBounds aBoundsType, 
	float2 someBounds)
{
	ExposedVariable ev;
	ev.adr = &aVariable;
	ev.format = eDataFormat::Vec3;
	ev.name = aName;
	ev.pickMode = aPickMode;

	ev.sensitivity = aSensitivity;
	ev.boundsType = aBoundsType;
	ev.bounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ExposedComponent::Expose(
	float4& aVariable, 
	const std::string& aName, 
	float aSensitivity, 
	ePickMode aPickMode, 
	eBounds aBoundsType, 
	float2 someBounds)
{
	ExposedVariable ev;
	ev.adr = &aVariable;
	ev.format = eDataFormat::Vec4;
	ev.name = aName;
	ev.pickMode = aPickMode;

	ev.sensitivity = aSensitivity;
	ev.boundsType = aBoundsType;
	ev.bounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ExposedComponent::Expose(
	ExposableString& aVariable, 
	const std::string& aName)
{
	ExposedVariable ev;
	ev.adr = &aVariable;
	ev.format = eDataFormat::String;
	ev.name = aName;
	myExposedVariables.push_back(ev);
}

void ExposedComponent::OnImGui(const std::string& aName)
{
	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	bool open = ImGui::TreeNode("Component", aName.c_str());

	if (!HasExposedVariables() || !open)
	{
		ImGui::PushID(INT_MIN);
		ImGui::PopID();
		if (open)
		{
			ImGui::TreePop();
		}
		return;
	}

	if (open)
	{
		ImGui::BeginTable("values", 2);
		ImGui::TableNextRow();
		for (int i = 0; i < myExposedVariables.size(); i++)
		{
			auto& variable = myExposedVariables[i];
			ImGui::PushID(&variable);
			ImGui::TableSetColumnIndex(0);
			variable.OnImGui();
			if (i != myExposedVariables.size() - 1)
			{
				ImGui::TableNextRow();
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
		ImGui::TreePop();
	}
}
