#pragma once
#include "RenderingAssets.h"
#include "Tupla/Core/Window.h"

namespace Tupla
{
    enum class RenderImpl
    {
        None,
        DX11,
        VULKAN
    };

#define CLASS_RENDER_IMPL(x) RenderImpl GetActiveImplementation() override { return RenderImpl::T_EXPAND_MACRO(x);} \
    const char* GetName() override{ return T_STRINGIFY_MACRO(x);}
    
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual Scope<Window> StartWindow(const WindowProps& props) = 0;
        virtual RenderingAssets* GetRenderingAssets() { return m_RenderingAssets.get(); }

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void BeginGUIFrame() = 0;
        virtual void EndGUIFrame() = 0;
        
        virtual RenderImpl GetActiveImplementation() = 0;
        virtual const char* GetName() = 0;
        virtual std::string ToString() { return GetName(); }

    protected:
        Scope<RenderingAssets> m_RenderingAssets;
    };
}
