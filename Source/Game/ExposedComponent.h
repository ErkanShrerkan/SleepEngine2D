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
		void* adr;
		std::string name;
		eDataFormat format;
		ePickMode pickMode;

	protected:

		void Bool()
		{
			ImGui::Checkbox("", (bool*)adr);
		}

		void Scalar()
		{
			ImGui::DragFloat("", (float*)adr, 0.1f);
		}

		void Vec2()
		{
			ImGui::DragFloat2("", (float*)adr, 0.1f);
		}

		void Vec3()
		{
			switch (pickMode)
			{
			case ePickMode::Drag:
				ImGui::DragFloat3("", (float*)adr, 0.1f);
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
				ImGui::DragFloat4("", (float*)adr, 0.1f);
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
			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();
			ImGui::Text(name.c_str());
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
			ImGui::TableNextRow();
		}
	};

	std::vector<ExposedVariable> myExposedVariables;

protected:
	void Expose(bool& aVariable, const std::string& aName)
	{
		ExposedVariable ev;
		ev.adr = &aVariable;
		ev.format = eDataFormat::Bool;
		ev.name = aName;
		myExposedVariables.push_back(ev);
	}

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
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		bool open = ImGui::TreeNode("Component", aName.c_str());

		if (!HasExposedVariables() || !open)
		{
			ImGui::PushID(INT_MIN);
			ImGui::TableSetColumnIndex(1);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("");
			ImGui::TableNextRow();
			ImGui::PopID();
			if (open)
			{
				ImGui::TreePop();
			}
			return;
		}

		if (open)
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

