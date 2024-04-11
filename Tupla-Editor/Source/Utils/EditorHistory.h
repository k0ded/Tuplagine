#pragma once
#include <stack>

#include "Tupla/Core/Base.h"

namespace Tupla
{
	class EditorCommand;

	class EditorHistory
	{
	public:
		static void PerformCommand(const Ref<EditorCommand>& editorCommand);

		static void Undo();
		static void Redo();

	private:
		inline static int ptr = 0;
		inline static std::stack<Ref<EditorCommand>> editorUndos{};
		inline static std::stack<Ref<EditorCommand>> editorRedos{};
	};
}
