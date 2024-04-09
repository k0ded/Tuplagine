#include "tgpch.h"
#include "WindowsWindow.h"

#include "Tupla/Core/Application.h"

#define WND_CLASS_NAME L"MainWindowClass"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Tupla
{
	LRESULT CALLBACK MainWndProc(
		HWND hwnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
	)
	{
		if (!Tupla::Application::m_Restarting && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
			return true;

		switch (uMsg)
		{
		case WM_SIZE:
			WindowsWindow::m_Instance->m_Props.Width = LOWORD(lParam);
			WindowsWindow::m_Instance->m_Props.Height = HIWORD(lParam);
			break;
		case WM_CLOSE:
			WindowsWindow::m_Instance->m_ShouldClose = true;
			return true;
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) : Window(props)
	{
		m_Instance = this;
		m_Props.Title = props.Title;

		if(!m_RegisteredClass)
		{
			WNDCLASS wndclass = { 0, MainWndProc, 0, 0, 0, 0, 0, 0, 0, WND_CLASS_NAME };
			ASSERT(RegisterClass(&wndclass), "Failed to register WNDCLASS");
			m_RegisteredClass = true;
		}
			

		m_Handle = CreateWindowEx(
			0, WND_CLASS_NAME, 
			props.Title.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			0, 0,
			static_cast<int>(m_Props.Width), static_cast<int>(m_Props.Height),
			nullptr, nullptr, nullptr, nullptr);

		ASSERT(m_Handle != nullptr, GetLastError());
	}

	WindowsWindow::~WindowsWindow()
	{
		DestroyWindow((HWND)m_Handle);
	}

	bool WindowsWindow::GetMinimized()
	{
		return m_Props.Height <= 0 || m_Props.Width <= 0;
	}

	void WindowsWindow::SetTitle(const std::wstring& newTitle)
	{
		m_Props.Title = newTitle;
		SetWindowText((HWND)m_Handle, newTitle.c_str());
	}

	bool WindowsWindow::ShouldClose()
	{
		return m_ShouldClose;
	}

	void WindowsWindow::PollEvents() const
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			for (const auto& function : m_AttachedEvents)
			{
				if (function(msg.message, msg.wParam, msg.lParam))
					break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowsWindow::AttachEvent(const std::function<bool(UINT, WPARAM, LPARAM)>& function)
	{
		m_AttachedEvents.push_back(function);
	}
}
