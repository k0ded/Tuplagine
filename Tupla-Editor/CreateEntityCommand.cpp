#include "CreateEntityCommand.h"

#include "Tupla/Scene/SceneManager.h"

Tupla::CreateEntityCommand::CreateEntityCommand(const Entity& aStartEntity)
{

}

void Tupla::CreateEntityCommand::Process()
{
	m_EntityRedo = &SceneManager::GetActiveScene().CreateEntity();
}

void Tupla::CreateEntityCommand::Undo()
{
	SceneManager::GetActiveScene().DeleteEntity(m_EntityRedo->GetId());
	m_EntityRedo = nullptr;
}
