#pragma once

namespace Tupla
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void CompileOnce(const std::vector<char>& code) = 0;
        virtual void RecompileCode() = 0;
        u32 GetCodeSize() const { return m_LastCodeSize; }

    protected:
        u32 m_LastCodeSize {};
    };
}
