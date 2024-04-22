#include "tgpch.h"
#include "Entity.h"
#include "Scene.h"

Tupla::Entity::Entity(Scene* aScene)
{
	CoCreateGuid(&m_Id);
	m_Scene = aScene;
}
