#include "pch.h"
#include "TransformUI.h"

TransformUI::TransformUI()
	: ComponentUI("TransformUI", COMPONENT_TYPE::TRANSFORM)
{
}

TransformUI::~TransformUI()
{
}

void TransformUI::Tick_UI()
{
	ComponentUI::Tick_UI();
	
	Vec3 Pos = GetTargetObject()->Transform()->GetRelativePos();
	Vec3 Scale = GetTargetObject()->Transform()->GetRelativeScale();
	Vec3 Rot = GetTargetObject()->Transform()->GetRelativeRot();
	// operator ÄÁ¹öÀü, Vec3 -> float[3]

	bool IsIndependentScale = GetTargetObject()->Transform()->IsIndependentScale();

	ImGui::Text("Position");
	ImGui::SameLine(100);
	if (ImGui::DragFloat3("##Position", Pos))
	{
		GetTargetObject()->Transform()->SetRelativePos(Pos);
	}

	ImGui::Text("Scale");
	ImGui::SameLine(100);
	if (ImGui::DragFloat3("##Scale", Scale))
	{

		GetTargetObject()->Transform()->SetRelativeScale(Scale);
	}

	ImGui::Text("Rotation");
	ImGui::SameLine(100);

	Rot = (Rot / XM_PI) * 180.f;
	if (ImGui::DragFloat3("##Rotation", Rot, 0.1f))
	{
		Rot = (Rot / 180.f) * XM_PI;
		GetTargetObject()->Transform()->SetRelativeRot(Rot);
	}

	ImGui::Text("IndependentScale");
	ImGui::SameLine(125);
	if (ImGui::Checkbox("##IdependentScale", &IsIndependentScale))
	{
		GetTargetObject()->Transform()->SetIndependentScale(IsIndependentScale);
	}
}