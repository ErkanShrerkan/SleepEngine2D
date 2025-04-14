#include "pch.h"
//#include <ThirdParty\ImGui\imgui.h>
#include "ComponentExposer.h"
//#include "Component.h"
#include "GameManager.h"
#include "Entity.h"

namespace Expose
{
	void IExposed::PrepareImGui()
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text("  ");
		ImGui::SameLine();
		ImGui::Text(myName.c_str());
		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	void IExposed::OnImGui()
	{
		PrepareImGui();
		Edit();
	}

	float IExposed::InBoundsValue(float aValue)
	{
		float returnVal = aValue;
		switch (myBoundsType)
		{
		case eBounds::Loop:
			returnVal += returnVal > myBounds.y ? myBounds.x - myBounds.y : (returnVal < myBounds.x ? myBounds.y - myBounds.x : 0.f);
			break;
		case eBounds::Clamp:
			returnVal = Math::Clamp(returnVal, myBounds.x, myBounds.y);
			break;
		default:
			break;
		}
		return returnVal;
	}

	std::string IExposed::GetDataFormatAsString(eDataFormat format)
	{
		switch (format)
		{
		case eDataFormat::Bool:
			return "bool";
		case eDataFormat::Float:
			return "float";
		case eDataFormat::Vec2:
			return "vec2";
		case eDataFormat::Vec3:
			return "vec3";
		case eDataFormat::Vec4:
			return "vec4";
		case eDataFormat::String:
			return "string";
		case eDataFormat::ComponentRef:
			return "reference";
		}

		return "error";
	}

	void ExposedComponentRef::Edit()
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
			std::string objectID = std::to_string(component->GameObject().GetID());
			std::string label("(" + objectID + ") " + componentName);
			es.SetString(label);
			ImGui::InputText("", es[0], es.GetSize(), ImGuiInputTextFlags_ReadOnly);
			AcceptDragDropPayLoad();
		}
	}

	rapidjson::Value ExposedComponentRef::Serialize([[maybe_unused]] rapidjson::Document::AllocatorType& allocator)
	{
		return rapidjson::Value();
	}

	const std::string& ExposedComponentRef::GetComponentName(uint anID)
	{
		return myGameManager.GetComponentTypeNameByID(anID);
	}

	Component*& ExposedComponentRef::GetComponentPtr()
	{
		return *(Component**)adr;
	}

	void ExposedComponentRef::AcceptDragDropPayLoad()
	{
		if (!ImGui::BeginDragDropTarget())
			return;

		if (const ImGuiPayload* entityPayload = ImGui::AcceptDragDropPayload("DRAG_ENTITY_REF"))
		{
			IM_ASSERT(entityPayload->DataSize == sizeof(uint));
			uint entity = *(const uint*)entityPayload->Data;
			Component* componentPtr = nullptr;

			try
			{
				componentPtr = myGameManager.GetComponentsFromEntity(entity).at(componentID);
			}
			catch (const std::exception&)
			{
				ImGui::EndDragDropTarget();
				return;
			}

			*adr = componentPtr;
			myGameManager.RegisterComponentRef(adr, componentPtr);
		}
		else if (const ImGuiPayload* compPayload = ImGui::AcceptDragDropPayload("DRAG_COMPONENT_REF"))
		{
			IM_ASSERT(compPayload->DataSize == sizeof(Component*));
			Component* componentPtr = *(Component**)compPayload->Data;

			try
			{
				std::ignore = myGameManager.GetComponentsFromEntity(componentPtr->GameObject().GetID()).at(componentID);
			}
			catch (const std::exception&)
			{
				ImGui::EndDragDropTarget();
				return;
			}

			*adr = componentPtr;
			myGameManager.RegisterComponentRef(adr, componentPtr);
		}

		ImGui::EndDragDropTarget();
	}

	void ExposedBool::Edit()
	{
		ImGui::Checkbox("", (bool*)adr);
	}

	rapidjson::Value ExposedBool::Serialize(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value info(rapidjson::kObjectType);
		rapidjson::Value name(rapidjson::kObjectType);
		rapidjson::Value type(rapidjson::kObjectType);
		rapidjson::Value value(rapidjson::kObjectType);

		name.SetString(myName.c_str(), static_cast<uint>(myName.size()), allocator);

		const std::string typeString = GetDataFormatAsString(myFormat);
		type.SetString(typeString.c_str(), static_cast<uint>(typeString.size()), allocator);

		info.AddMember("Name", name, allocator);
		info.AddMember("Type", type, allocator);

		value.SetBool(*reinterpret_cast<bool*>(adr));

		info.AddMember("Value", value, allocator);
		return info;
	}

	void ExposedFloat::Edit()
	{
		ImGui::DragFloat("", (float*)adr, mySensitivity);
		*(float*)adr = InBoundsValue(*(float*)adr);
	}

	rapidjson::Value ExposedFloat::Serialize(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value info(rapidjson::kObjectType);
		rapidjson::Value name(rapidjson::kObjectType);
		rapidjson::Value type(rapidjson::kObjectType);
		rapidjson::Value value(rapidjson::kObjectType);

		name.SetString(myName.c_str(), static_cast<uint>(myName.size()), allocator);

		const std::string typeString = GetDataFormatAsString(myFormat);
		type.SetString(typeString.c_str(), static_cast<uint>(typeString.size()), allocator);

		info.AddMember("Name", name, allocator);
		info.AddMember("Type", type, allocator);

		value.SetFloat(*reinterpret_cast<float*>(adr));

		info.AddMember("Value", value, allocator);
		return info;
	}

	void ExposedVec2::Edit()
	{
		ImGui::DragFloat2("", (float*)adr, mySensitivity);
		float2& f2 = *(float2*)adr;
		f2.x = InBoundsValue(f2.x);
		f2.y = InBoundsValue(f2.y);
	}

	rapidjson::Value ExposedVec2::Serialize(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value info(rapidjson::kObjectType);
		rapidjson::Value name(rapidjson::kObjectType);
		rapidjson::Value type(rapidjson::kObjectType);
		rapidjson::Value value(rapidjson::kArrayType);

		name.SetString(myName.c_str(), static_cast<uint>(myName.size()), allocator);

		const std::string typeString = GetDataFormatAsString(myFormat);
		type.SetString(typeString.c_str(), static_cast<uint>(typeString.size()), allocator);

		info.AddMember("Name", name, allocator);
		info.AddMember("Type", type, allocator);

		Vector2f vec = *reinterpret_cast<Vector2f*>(adr);
		value.PushBack(vec.x, allocator);
		value.PushBack(vec.y, allocator);

		info.AddMember("Value", value, allocator);
		return info;
	}

	void ExposedVec3::Edit()
	{
		float3& f3 = *(float3*)adr;
		switch (myPickMode)
		{
		case ePickMode::Drag:
			ImGui::DragFloat3("", (float*)adr, mySensitivity);
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

	rapidjson::Value ExposedVec3::Serialize(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value info(rapidjson::kObjectType);
		rapidjson::Value name(rapidjson::kObjectType);
		rapidjson::Value type(rapidjson::kObjectType);
		rapidjson::Value value(rapidjson::kArrayType);

		name.SetString(myName.c_str(), static_cast<uint>(myName.size()), allocator);

		const std::string typeString = GetDataFormatAsString(myFormat);
		type.SetString(typeString.c_str(), static_cast<uint>(typeString.size()), allocator);

		info.AddMember("Name", name, allocator);
		info.AddMember("Type", type, allocator);

		Vector3f vec = *reinterpret_cast<Vector3f*>(adr);
		value.PushBack(vec.x, allocator);
		value.PushBack(vec.y, allocator);
		value.PushBack(vec.z, allocator);

		info.AddMember("Value", value, allocator);
		return info;
	}

	void ExposedVec4::Edit()
	{
		float4& f4 = *(float4*)adr;
		switch (myPickMode)
		{
		case ePickMode::Drag:
			ImGui::DragFloat4("", (float*)adr, mySensitivity);
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

	rapidjson::Value ExposedVec4::Serialize(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value info(rapidjson::kObjectType);
		rapidjson::Value name(rapidjson::kObjectType);
		rapidjson::Value type(rapidjson::kObjectType);
		rapidjson::Value value(rapidjson::kArrayType);

		name.SetString(myName.c_str(), static_cast<uint>(myName.size()), allocator);

		const std::string typeString = GetDataFormatAsString(myFormat);
		type.SetString(typeString.c_str(), static_cast<uint>(typeString.size()), allocator);

		info.AddMember("Name", name, allocator);
		info.AddMember("Type", type, allocator);

		Vector4f vec = *reinterpret_cast<Vector4f*>(adr);
		value.PushBack(vec.x, allocator);
		value.PushBack(vec.y, allocator);
		value.PushBack(vec.z, allocator);
		value.PushBack(vec.w, allocator);

		info.AddMember("Value", value, allocator);
		return info;
	}

	void ExposedString::Edit()
	{
		ExposableString& es = *(ExposableString*)adr;
		ImGui::InputText("", es[0], es.GetSize());

		if (!ImGui::BeginDragDropTarget())
			return;

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_FILENAME"))
		{
			IM_ASSERT(payload->DataSize == sizeof(char) * 128);
			DynamicStringBuffer dsb = *(const DynamicStringBuffer*)payload->Data;
			es.SetString(dsb.GetString());
		}

		ImGui::EndDragDropTarget();
	}

	rapidjson::Value ExposedString::Serialize(rapidjson::Document::AllocatorType& allocator)
	{
		rapidjson::Value info(rapidjson::kObjectType);
		rapidjson::Value name(rapidjson::kObjectType);
		rapidjson::Value type(rapidjson::kObjectType);
		rapidjson::Value value(rapidjson::kObjectType);

		name.SetString(myName.c_str(), static_cast<uint>(myName.size()), allocator);

		const std::string typeString = GetDataFormatAsString(myFormat);
		type.SetString(typeString.c_str(), static_cast<uint>(typeString.size()), allocator);

		info.AddMember("Name", name, allocator);
		info.AddMember("Type", type, allocator);

		const ExposableString& string = *reinterpret_cast<ExposableString*>(adr);
		value.SetString(string.GetString().c_str(), string.GetStringSize(), allocator);

		info.AddMember("Value", value, allocator);
		return info;
	}
}

rapidjson::Value ComponentExposer::Serialize(rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value exposedVariablesArray(rapidjson::kArrayType);

	for (auto& variable : myExposedVariables)
	{
		rapidjson::Value variableData = variable->Serialize(allocator);
		exposedVariablesArray.PushBack(variableData, allocator);
	}

	return exposedVariablesArray;
}

void ComponentExposer::Expose(
	bool& aVariable,
	const std::string& aName)
{
	sptr(Expose::ExposedBool) ev =
		std::make_shared<Expose::ExposedBool>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.myFormat = Expose::eDataFormat::Bool;
	evr.myName = aName;

	myExposedVariables.push_back(ev);
}

void ComponentExposer::Expose(
	float& aVariable,
	const std::string& aName,
	float aSensitivity,
	Expose::eBounds aBoundsType,
	float2 someBounds)
{
	sptr(Expose::ExposedFloat) ev =
		std::make_shared<Expose::ExposedFloat>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.myFormat = Expose::eDataFormat::Float;
	evr.myName = aName;
	evr.myPickMode = Expose::ePickMode::Drag;
	evr.mySensitivity = aSensitivity;
	evr.myBoundsType = aBoundsType;
	evr.myBounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ComponentExposer::Expose(
	float2& aVariable,
	const std::string& aName,
	float aSensitivity,
	Expose::eBounds aBoundsType,
	float2 someBounds)
{
	sptr(Expose::ExposedVec2) ev =
		std::make_shared<Expose::ExposedVec2>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.myFormat = Expose::eDataFormat::Vec2;
	evr.myName = aName;
	evr.myPickMode = Expose::ePickMode::Drag;
	evr.mySensitivity = aSensitivity;
	evr.myBoundsType = aBoundsType;
	evr.myBounds = someBounds;

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
	sptr(Expose::ExposedVec3) ev =
		std::make_shared<Expose::ExposedVec3>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.myFormat = Expose::eDataFormat::Vec3;
	evr.myName = aName;
	evr.myPickMode = aPickMode;
	evr.mySensitivity = aSensitivity;
	evr.myBoundsType = aBoundsType;
	evr.myBounds = someBounds;

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
	sptr(Expose::ExposedVec4) ev =
		std::make_shared<Expose::ExposedVec4>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.myFormat = Expose::eDataFormat::Vec4;
	evr.myName = aName;
	evr.myPickMode = aPickMode;
	evr.mySensitivity = aSensitivity;
	evr.myBoundsType = aBoundsType;
	evr.myBounds = someBounds;

	myExposedVariables.push_back(ev);
}

void ComponentExposer::Expose(
	ExposableString& aVariable,
	const std::string& aName)
{
	sptr(Expose::ExposedString) ev =
		std::make_shared<Expose::ExposedString>();

	auto& evr = *ev;
	evr.adr = &aVariable;
	evr.myFormat = Expose::eDataFormat::String;
	evr.myName = aName;

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

void ComponentExposer::OnImGuiBegin(const std::string& aName)
{
	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	isOpen = ImGui::TreeNode("Component", aName.c_str());
}

void ComponentExposer::OnImGui()
{
	if (!HasExposedVariables() || !isOpen)
	{
		ImGui::PushID(INT_MIN);
		ImGui::PopID();
		if (isOpen)
		{
			ImGui::TreePop();
		}
		return;
	}

	if (!isOpen)
	{
		return;
	}

	ImGui::BeginTable("values", 2);
	ImGui::TableNextRow();
	for (int i = 0; i < myExposedVariables.size(); i++)
	{
		auto& variable = myExposedVariables[i];
		ImGui::PushID(&variable);
		ImGui::TableSetColumnIndex(0);

		ImGui::PushID(variable->myId);
		variable->OnImGui();
		ImGui::PopID();

		if (i != myExposedVariables.size() - 1)
			ImGui::TableNextRow();

		ImGui::PopID();
	}
	ImGui::EndTable();
	ImGui::TreePop();
}
