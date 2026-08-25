#include "pch.h"
#include "CMeshRender.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::FinalTick()
{
}

void CMeshRender::Render()
{
	// Animator3D Binding
	if (Animator3D())
		Animator3D()->Binding();

	for (int i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		if (nullptr == GetMaterial(i))
			continue;	

		GetMaterial(i)->SetScalar(FLOAT_0, (float)GetOwner()->GetID());
		GetMaterial(i)->Binding();

		GetMesh()->Render(i);
	}

	if(Animator3D())
		Animator3D()->ClearData();
}