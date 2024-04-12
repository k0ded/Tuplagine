#include "EditorHistory.h"

void Tupla::EditorHistory::PerformCommand(const Ref<EditorCommand>& editorCommand)
{
	editorCommand->Process();
	editorUndos.push(editorCommand);
	editorRedos = {};
}

void Tupla::EditorHistory::Undo()
{
	if (editorUndos.empty()) return;
	editorUndos.top()->Undo();
	editorUndos.pop();
}

void Tupla::EditorHistory::Redo()
{
	if (editorRedos.empty()) return;
	const auto& redo = editorRedos.top();
	redo->Process();
	editorUndos.push(redo);
	editorRedos.pop();
}
