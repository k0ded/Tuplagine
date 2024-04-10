#pragma once

namespace Tupla
{

    enum class ShaderStageSlot
    {
        Vertex,
        Pixel,
        Hull,
        Domain,
        Compute,
	    Count,
    };

	enum class ShaderStage
	{
        None,
		Vertex  = 1 << 0,
        Pixel   = 1 << 1,
        Hull    = 1 << 2,
        Domain  = 1 << 3,
        Compute = 1 << 4,
	};

    class Shader
    {
    public:
        virtual ~Shader() = default;
        virtual bool CompileShader(const std::wstring& path, ShaderStage stage, bool debug = true, const std::string& debugName = "GenericShader") = 0;
        virtual bool CompileShader(void* data, size_t byteSize, ShaderStage stage, const std::string& debugName) = 0;

        int GetStages() const { return m_Stage; }

    protected:
        int m_Stage;
    };
}
