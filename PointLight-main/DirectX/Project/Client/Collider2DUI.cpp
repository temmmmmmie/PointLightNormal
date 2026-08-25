#include "pch.h"
#include "Collider2DUI.h"

Collider2DUI::Collider2DUI()
	: ComponentUI("Collider2DUI", COMPONENT_TYPE::COLLIDER2D)
{
}

Collider2DUI::~Collider2DUI()
{
}

void Collider2DUI::Tick_UI()
{
	ComponentUI::Tick_UI();


	CCollider2D* pCollider =  GetTargetObject()->Collider2D();

	Vec2 Offset = pCollider->GetOffset();
	Vec2 Scale = pCollider->GetSclae();
	bool bInde = pCollider->GetIndependentScale();

	ImGui::Text("Offset");
	ImGui::SameLine(100);
	if (ImGui::DragFloat2("##Offset", Offset))
	{
		pCollider->SetOffset(Offset);
	}

	ImGui::Text("Scale");
	ImGui::SameLine(100);
	if (ImGui::DragFloat2("##Scale", Scale))
	{
		pCollider->SetScale(Scale);
	}

	ImGui::Text("IndependentScale");
	ImGui::SameLine(125);
	if (ImGui::Checkbox("##IdependentScale", &bInde))
	{
		pCollider->SetIndependentScale(bInde);
	}
}