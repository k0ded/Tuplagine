#pragma once
#include <string>

namespace Tupla
{
	struct VertexElementDesc
	{
		std::string Semantic;
		unsigned SemanticIndex;
		unsigned Type = 0;
	};

	typedef enum PIPELINE_STAGE
	{
		PIPELINE_STAGE_INPUTASSEMBLER = 0x1L,
		PIPELINE_STAGE_VERTEX_SHADER = 0x2L,
		PIPELINE_STAGE_GEOMETRY_SHADER = 0x4L,
		PIPELINE_STAGE_RASTERIZER = 0x8L,
		PIPELINE_STAGE_PIXEL_SHADER = 0x10L,
		PIPELINE_STAGE_OUTPUT_MERGER = 0x20L,
		PIPELINE_STAGE_COMPUTE_SHADER = 0x40L
	} PIPELINE_STAGE;
}