#pragma once

namespace Tupla
{
	class Mesh;
}

namespace Tupla
{
	enum class Origin : i32
	{
		SOURCE, // Keep vertex data as is
		CENTER, // Center all meshes as a group
		BOTTOM, // Same as center, but doesn't center the Y coordinate

		CENTER_EACH_MESH // Center each mesh in fbx separately, when exporting each mesh as a subresource
	};

	class IModelSerializer
	{
	public:
		virtual ~IModelSerializer() = default;

		virtual std::vector<Ref<Mesh>> SerializeModel(const std::string& aSourcePath) = 0;
	};
}
