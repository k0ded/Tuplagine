#pragma once

namespace Tupla
{
	enum class ShaderStage
	{
        None,
		Vertex,
        Hull,
        Domain,
        Pixel,
        Compute,
        Count
	};

    class Shader
    {
    public:
        virtual ~Shader() = default;
        virtual void CompileShader() = 0;

        ShaderStage GetStage() const { return m_Stage; }

    protected:
        ShaderStage m_Stage;
    };
}
