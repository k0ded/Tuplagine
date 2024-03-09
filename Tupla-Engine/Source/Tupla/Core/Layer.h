#pragma once
#include <string>

namespace Tupla
{
    class Layer
    {
    public:
        Layer(const std::string& name = "Layer") { m_DebugName = name; }
        virtual ~Layer() = default;
    
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnGUI() {}

        const std::string& GetName() { return m_DebugName; }
    private:
        std::string m_DebugName;
    };
}
