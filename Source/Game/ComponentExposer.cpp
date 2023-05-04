#include "pch.h"
#include <ThirdParty\ImGui\imgui.h>
#include "ComponentExposer.h"
//#include "Component.h"
#include "GameManager.h"
#include "Entity.h"

namespace Expose
{
	float ExposedVariable::InBoundsValue(float aValue)
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

	void ExposedVariable::EditBool()
	{
		ImGui::Checkbox("", (bool*)adr);
	}

	void ExposedVariable::EditScalar()
	{
		ImGui::DragFloat("", (float*)adr, sensitivity);
		*(float*)adr = InBoundsValue(*(float*)adr);
	}

	void ExposedVariable::EditVec2()
	{
		ImGui::DragFloat2("", (float*)adr, sensitivity);
		float2& f2 = *(float2*)adr;
		f2.x = InBoundsValue(f2.x);
		f2.y = InBoundsValue(f2.y);
	}

	void ExposedVariable::EditVec3()
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

	void ExposedVariable::EditVec4()
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

	void ExposedVariable::EditString()
	{
		ExposableString& es = *(ExposableString*)adr;
		ImGui::InputText("", es[0], es.GetSize());
	}

	void IExposed::PrepareImGui()
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text("  ");
		ImGui::SameLine();
		ImGui::Text(name.c_str());
		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	ExposedComponentRef::ExposedComponentRef(GameManager* aGameManager)
		: myGameManager(*aGameManager) { }

	void ExposedComponentRef::OnImGui()
	{
		PrepareImGui();
		editFunc(*this);
	}

	void ExposedComponentRef::EditComponentRef()
	{
		AcceptDragDropPayLoad();
		ExposableString es(32);
		auto& component = GetComponentPtr();
		if (!component)
		{
			std::string label = "(None) " + componentName;
			ImGui::InputTextWithHint("", label.c_str(), es[0], es.GetSize(), ImGuiInputTextFlags_ReadOnly);
			AcceptDragDropPayLoad();
		}
		else
		{
			std::string id = std::to_string(component->GameObject().GetID());
			std::string label("(" + id + ") " + componentName);
			es.SetString(label);
			ImGui::InputText("", es[0], es.GetSize(), ImGuiInputTextFlags_ReadOnly);
			AcceptDragDropPayLoad();
		}
	}

	Component*& ExposedComponentRef::GetComponentPtr()
	{
		return *(Component**)adr;
	}

	void ExposedComponentRef::AcceptDragDropPayLoad()
	{
		if (!ImGui::BeginDragDropTarget())
			return;

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_ENTITY_REF"))
		{
			IM_ASSERT(payload->DataSize == sizeof(uint));
			uint entity = *(const uint*)payload->Data;
			Component* componentPtr = myGameManager.GetComponentsFromEntity(entity).at(componentID);
			*adr = componentPtr;
			myGameManager.RegisterComponentRef(adr, componentPtr);
		}

		ImGui::EndDragDropTarget();
	}

	const std::string& ExposedComponentRef::GetComponentName(uint anID)
	{
		return myGameManager.GetComponentTypeNameByID(anID);
	}

	ExposedVariable::~ExposedVariable()
	{
		/*printe("EXPOSED VARIABLE DELETED\n");*/
	}

	void ExposedVariable::OnImGui()
	{
		PrepareImGui();
		switch (format)
		{
		case eDataFormat::Bool:	EditBool();	break;
		case eDataFormat::Scalar: EditScalar();	break;
		case eDataFormat::Vec2:	EditVec2();	break;
		case eDataFormat::Vec3:	EditVec3();	break;
		case eDataFormat::Vec4:	EditVec4();	break;
		case eDataFormat::String: EditString();	break;
		default: break;
		}
	}
}

void ComponentExposer::Expose(
	bool& aVariable,
	const std::string& aName)
{
	sptr(Expose::ExposedVariable) ev =
		std::make_shared<Expose::ExposedVariable>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.format = Expose::eDataFormat::Bool;
	evr.name = aName;

	myExposedVariables.push_back(ev);
}

void ComponentExposer::Expose(
	float& aVariable,
	const std::string& aName,
	float aSensitivity,
	Expose::eBounds aBoundsType,
	float2 someBounds)
{
	sptr(Expose::ExposedVariable) ev =
		std::make_shared<Expose::ExposedVariable>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.format = Expose::eDataFormat::Scalar;
	evr.name = aName;
	evr.pickMode = Expose::ePickMode::Drag;
	evr.sensitivity = aSensitivity;
	evr.boundsType = aBoundsType;
	evr.bounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ComponentExposer::Expose(
	float2& aVariable,
	const std::string& aName,
	float aSensitivity,
	Expose::eBounds aBoundsType,
	float2 someBounds)
{
	sptr(Expose::ExposedVariable) ev =
		std::make_shared<Expose::ExposedVariable>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.format = Expose::eDataFormat::Vec2;
	evr.name = aName;
	evr.pickMode = Expose::ePickMode::Drag;
	evr.sensitivity = aSensitivity;
	evr.boundsType = aBoundsType;
	evr.bounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ComponentExposer::Expose(
	float3& aVariable,
	const std::string& aName,
	float aSensitivity,
	Expose::ePickMode aPickMode,
	Expose::eBounds aBoundsType,
	float2 someBounds)
{
	sptr(Expose::ExposedVariable) ev =
		std::make_shared<Expose::ExposedVariable>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.format = Expose::eDataFormat::Vec3;
	evr.name = aName;
	evr.pickMode = aPickMode;
	evr.sensitivity = aSensitivity;
	evr.boundsType = aBoundsType;
	evr.bounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ComponentExposer::Expose(
	float4& aVariable,
	const std::string& aName,
	float aSensitivity,
	Expose::ePickMode aPickMode,
	Expose::eBounds aBoundsType,
	float2 someBounds)
{
	sptr(Expose::ExposedVariable) ev =
		std::make_shared<Expose::ExposedVariable>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.format = Expose::eDataFormat::Vec4;
	evr.name = aName;
	evr.pickMode = aPickMode;
	evr.sensitivity = aSensitivity;
	evr.boundsType = aBoundsType;
	evr.bounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ComponentExposer::Expose(
	ExposableString& aVariable,
	const std::string& aName)
{
	sptr(Expose::ExposedVariable) ev =
		std::make_shared<Expose::ExposedVariable>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.format = Expose::eDataFormat::String;
	evr.name = aName;

	myExposedVariables.push_back(ev);
}

ComponentExposer::~ComponentExposer()
{
	/*printe("EXPOSED COMPONENT DELETED\n");*/
}

void ComponentExposer::SetGameManager(GameManager* aGameManager)
{
	myGameManager = aGameManager;
}

void ComponentExposer::OnImGui(const std::string& aName)
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

			variable->OnImGui();

			if (i != myExposedVariables.size() - 1)
				ImGui::TableNextRow();

			ImGui::PopID();
		}
		ImGui::EndTable();
		ImGui::TreePop();
	}
}
