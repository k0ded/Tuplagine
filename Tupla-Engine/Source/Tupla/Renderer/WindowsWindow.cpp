#include "tgpch.h"
#include "WindowsWindow.h"

#include "CommonUtilities/Input/Input.h"
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

		if (Input::UpdateEvents(uMsg, wParam, lParam))
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

	WindowsWindow::WindowsWindow(const WindowProps& props) : m_Props(props)
	{
		m_Instance = this;

		if(!m_RegisteredClass)
		{
			WNDCLASS wndclass {};
			wndclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
			wndclass.lpfnWndProc = MainWndProc;
			wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wndclass.lpszClassName = WND_CLASS_NAME;

			ASSERT(RegisterClass(&wndclass), "Failed to register WNDCLASS");
			m_RegisteredClass = true;
		}

		CommonUtilities::Vector2<i32> position{};

		const int xScreen = GetSystemMetrics(SM_CXSCREEN);
		const int yScreen = GetSystemMetrics(SM_CYSCREEN);

		int right = xScreen - static_cast<int>(props.Width);
		int bottom = yScreen - static_cast<int>(props.Height);

		switch (props.Position)
		{
		case WindowPosition::TopLeft:
			break;
		case WindowPosition::TopRight:
			position = { right, 0 };
			break;
		case WindowPosition::BottomLeft:
			position = { 0, bottom };
			break;
		case WindowPosition::BottomRight:
			position = { right, bottom };
			break;
		case WindowPosition::TopCenter:
			position = { right / 2, 0 };
			break;
		case WindowPosition::BottomCenter:
			position = { right / 2, bottom };
			break;
		case WindowPosition::CenterLeft:
			position = { 0, bottom / 2 };
			break;
		case WindowPosition::CenterRight:
			position = { right, bottom / 2 };
			break;
		case WindowPosition::Center:
			position = { right / 2, bottom / 2 };
			break;
		}


		m_Handle = CreateWindowEx(
			0, WND_CLASS_NAME, 
			props.Title.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			position.x, position.y,
			static_cast<int>(m_Props.Width), static_cast<int>(m_Props.Height),
			nullptr, nullptr, nullptr, nullptr);

		ASSERT(m_Handle != nullptr, GetLastError());
	}

	WindowsWindow::~WindowsWindow()
	{
		DestroyWindow((HWND)m_Handle);
	}

	bool WindowsWindow::GetMinimized() const
	{
		return m_Props.Height <= 0 || m_Props.Width <= 0;
	}

	void WindowsWindow::SetTitle(const std::wstring& newTitle)
	{
		m_Props.Title = newTitle;
		SetWindowText(m_Handle, newTitle.c_str());
	}

	bool WindowsWindow::ShouldClose() const
	{
		return m_ShouldClose;
	}

	void WindowsWindow::PollEvents()
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
			{
				m_ShouldClose = true;
			}
		}
	}
}
