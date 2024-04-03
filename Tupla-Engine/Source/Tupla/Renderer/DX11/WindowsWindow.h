#pragma once
#include "Tupla/Core/Window.h"

namespace Tupla
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);

		bool GetMinimized() override;
		void SetTitle(const std::wstring& newTitle) override;
		bool ShouldClose() override;

		void PollEvents() const;
		void AttachEvent(const std::function<bool(UINT, WPARAM, LPARAM)>& function);

		friend LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		static inline WindowsWindow* m_Instance;
		bool m_ShouldClose = false;

		std::vector<std::function<bool(UINT, WPARAM, LPARAM)>> m_AttachedEvents;
	};
}
