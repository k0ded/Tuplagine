#pragma once

namespace Tupla
{
    class Shader
    {
    public:
        virtual ~Shader() = default;
        virtual Scope<std::string> GetCode();
        u32 GetCodeSize() const { return m_LastCodeSize; }

    protected:
        u32 m_LastCodeSize {};
    };
}
