#include "pch.h"
#include "Entity.h"

UINT Entity::g_NextID = 0;

Entity::Entity()
	: m_ID(g_NextID++)
	, m_RefCount(0)
{
}

Entity::Entity(const Entity& _Origin)
	: m_Name(_Origin.m_Name)
	, m_ID(g_NextID++)
	, m_RefCount(0)
{
}

Entity::~Entity()
{
}
