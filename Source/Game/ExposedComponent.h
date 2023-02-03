#pragma once
#include <ThirdParty\ImGui\imgui.h>

class ExposedComponent
{
protected:
	enum class ePickMode
	{
		Drag,
		Color
	};

private:
	enum class eDataFormat
	{
		Scalar,
		Vec2,
		Vec3,
		Vec4,
		String
	};

	class ExposedVariable
	{
	public:
		void* adr;
		std::string name;
		eDataFormat format;
		ePickMode pickMode;

	protected:
		void Scalar()
		{
			ImGui::DragFloat("", (float*)adr, 0.01f);
		}

		void Vec2()
		{
			ImGui::DragFloat2("", (float*)adr, 0.01f);
		}

		void Vec3()
		{
			switch (pickMode)
			{
			case ePickMode::Drag:
				ImGui::DragFloat3("", (float*)adr, 0.01f);
				break;
			case ePickMode::Color:
				ImGui::ColorEdit3("", (float*)adr);
				break;
			default:
				break;
			}
		}

		void Vec4()
		{
			switch (pickMode)
			{
			case ePickMode::Drag:
				ImGui::DragFloat4("", (float*)adr, 0.01f);
				break;
			case ePickMode::Color:
				ImGui::ColorEdit4("", (float*)adr);
				break;
			default:
				break;
			}
		}

		void String()
		{
			ImGui::InputText("", (char*)adr, 128);
		}

	public:
		void OnImGui()
		{
			ImGui::TableSetColumnIndex(2);
			ImGui::AlignTextToFramePadding();

			//ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			//ImGui::TreeNodeEx("Variable", flags, name.c_str());

			ImGui::Text(name.c_str());
			ImGui::SetNextItemWidth(-FLT_MIN);
			//ImGui::SetNextItemWidth(-FLT_MIN);
			switch (format)
			{
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

			ImGui::NextColumn();
		}
	};

	std::vector<ExposedVariable> myExposedVariables;

protected:
	void Expose(float& aVariable, const std::string& aName)
	{
		ExposedVariable ev;
		ev.adr = &aVariable;
		ev.format = eDataFormat::Scalar;
		ev.name = aName;
		ev.pickMode = ePickMode::Drag;
		myExposedVariables.push_back(ev);
	}

	void Expose(float2& aVariable, const std::string& aName)
	{
		ExposedVariable ev;
		ev.adr = &aVariable.x;
		ev.format = eDataFormat::Vec2;
		ev.name = aName;
		ev.pickMode = ePickMode::Drag;
		myExposedVariables.push_back(ev);
	}

	void Expose(float3& aVariable, const std::string& aName, ePickMode aPickMode)
	{
		ExposedVariable ev;
		ev.adr = &aVariable;
		ev.format = eDataFormat::Vec3;
		ev.name = aName;
		ev.pickMode = aPickMode;
		myExposedVariables.push_back(ev);
	}

	void Expose(float4& aVariable, const std::string& aName, ePickMode aPickMode)
	{
		ExposedVariable ev;
		ev.adr = &aVariable;
		ev.format = eDataFormat::Vec4;
		ev.name = aName;
		ev.pickMode = aPickMode;
		myExposedVariables.push_back(ev);
	}

	void Expose(std::string& aVariable, const std::string& aName)
	{
		ExposedVariable ev;
		ev.adr = &aVariable[0];
		ev.format = eDataFormat::String;
		ev.name = aName;
		myExposedVariables.push_back(ev);
	}

public:
	void OnImGui(const std::string& aName)
	{
		ImGui::TableSetColumnIndex(1);
		ImGui::AlignTextToFramePadding();
		if (ImGui::TreeNode("Component", aName.c_str()))
		{
			for (auto& variable : myExposedVariables)
			{
				ImGui::PushID(&variable);
				variable.OnImGui();
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}

	bool HasExposedVariables() { return !myExposedVariables.empty(); }
};

