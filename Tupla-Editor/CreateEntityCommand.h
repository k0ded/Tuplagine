#pragma once
#include "Tupla/Scene/Entity.h"
#include "UndoHistory/Commands/EditorCommand.h"

namespace Tupla
{
	class CreateEntityCommand : public EditorCommand
	{
	public:
		CreateEntityCommand(const Entity& aStartEntity);

		void Process() override;
		void Undo() override;

	private:
		Entity* m_EntityRedo;
	};
}

