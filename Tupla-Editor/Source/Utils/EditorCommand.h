#pragma once

namespace Tupla
{
	class EditorCommand
	{
	public:
		virtual ~EditorCommand();

		virtual void Process() = 0;
		virtual void Undo() = 0;
	};
}
