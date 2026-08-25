#include "pch.h"
#include "Light3DUI.h"

Light3DUI::Light3DUI()
	: ComponentUI("Light3DUI", COMPONENT_TYPE::LIGHT3D)
{
}

Light3DUI::~Light3DUI()
{
}

void Light3DUI::Tick_UI()
{
	ComponentUI::Tick_UI();

	Ptr<CLight3D> pLight3D = GetTargetObject()->Light3D();

	Vec3 vColor = pLight3D->GetLightColor();

	ImGui::Text("Light Color");
	ImGui::SameLine(100);
	if (ImGui::ColorEdit3("##LightColor", vColor))
	{
		pLight3D->SetLightColor(vColor);
	}
}
