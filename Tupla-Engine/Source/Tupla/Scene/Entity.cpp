#include "tgpch.h"
#include "Entity.h"
#include "Scene.h"

Tupla::Entity::Entity(Scene* aScene)
{
	CoCreateGuid(&m_Id);
	m_Scene = aScene;
	m_Coordinator = &m_Scene->GetHOT();
	m_HOT = m_Scene->GetHOT().CreateEntity();
}
