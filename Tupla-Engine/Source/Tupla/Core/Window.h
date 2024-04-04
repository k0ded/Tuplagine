#pragma once
#include <CommonUtilities/g_types.h>
#include <CommonUtilities/Math/Vector/Vector2.hpp>

namespace Tupla
{
    struct WindowProps
    {
        std::wstring Title;
        u32 Width;
        u32 Height;

        WindowProps(std::wstring title,
                    uint32_t width = 1600,
                    uint32_t height = 900)
            : Title(std::move(title)), Width(width), Height(height)
        {
        }
    };
    
    class Window
    {
    public:
        virtual ~Window() = default;
        
        std::wstring GetTitle() { return m_Props.Title; }
        CU::Vector2<u32> GetExtents() { return { m_Props.Width, m_Props.Height }; }

        u32 GetHeight() const { return m_Props.Height; }
        u32 GetWidth() const { return m_Props.Width; }

        virtual void SetTitle(const std::wstring& newTitle) = 0;
        virtual bool ShouldClose() = 0;
        virtual bool GetMinimized() = 0;
        void* GetWindowHandle() const { return m_Handle; }

    protected:
        Window(WindowProps props): m_Props(std::move(props)), m_Handle(nullptr) {}

        WindowProps m_Props;
        void* m_Handle;
    };
}
