#include "pch.h"
#include "Light2DUI.h"

#include <Engine/CLight2D.h>

Light2DUI::Light2DUI()
	: ComponentUI("Light2DUI", COMPONENT_TYPE::LIGHT2D)
{
}

Light2DUI::~Light2DUI()
{
}

void Light2DUI::Tick_UI()
{
	ComponentUI::Tick_UI();

	Ptr<CLight2D> pLight2D = GetTargetObject()->Light2D();

	Vec3 vColor = pLight2D->GetLightColor();

	ImGui::Text("Light Color");
	ImGui::SameLine(100);
	if (ImGui::ColorEdit3("##LightColor", vColor))
	{
		pLight2D->SetLightColor(vColor);
	}
}