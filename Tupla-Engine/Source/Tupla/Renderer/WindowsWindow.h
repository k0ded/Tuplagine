#pragma once
#include "CommonUtilities/Math/Vector/Vector2.hpp"

namespace Tupla
{
	enum class WindowPosition
	{
		TopLeft,
		TopRight,
		TopCenter,
		BottomLeft,
		BottomRight,
		BottomCenter,

		CenterLeft,
		CenterRight,
		Center,
	};

	struct WindowProps
	{
		std::wstring Title;
		u32 Width;
		u32 Height;
		WindowPosition Position = WindowPosition::Center;

		WindowProps(std::wstring title,
			uint32_t width = 1600,
			uint32_t height = 900)
			: Title(std::move(title)), Width(width), Height(height)
		{
		}
	};

	class WindowsWindow
	{
	public:
		WindowsWindow(const WindowProps& props);
		~WindowsWindow();

		bool GetMinimized() const;
		void SetTitle(const std::wstring& newTitle);
		bool ShouldClose() const;

		void PollEvents();

		std::wstring GetTitle() { return m_Props.Title; }
		CU::Vector2<u32> GetExtents() { return { m_Props.Width, m_Props.Height }; }

		u32 GetHeight() const { return m_Props.Height; }
		u32 GetWidth() const { return m_Props.Width; }

		HWND GetWindowHandle() const { return m_Handle; }

		friend LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		static inline WindowsWindow* m_Instance;
		static inline bool m_RegisteredClass = false;

		WindowProps m_Props;
		HWND m_Handle;
		bool m_ShouldClose = false;
	};
}
