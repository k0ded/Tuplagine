#pragma once
#include "IModelSerializer.h"
#include "openfbx/ofbx.h"

namespace Tupla
{
	struct FBXSerializeSettings
	{
		float meshScale = 1.f;
		Origin origin = Origin::SOURCE;
		bool mikktspaceTangents = false;
		bool importVertexColors = true;
		//bool vertexColorIsAO = false; Maybe later
		bool bakeVertexAO = false;
		float minBakeVertexAO = 0.f;

		// Level of detail
		u32 lodCount = 1;
		float lodDistances[4] = { 10, 100, 1000, 10000 };
		float autoLODCoefficients[4] = { 0.75f, 0.5f, 0.25f, 0.125f };
		u8 autoLODMask = 0;

		float boundingScale = 1.f;
	};

	enum class FBXOrientation
	{
		X_UP,
		Y_UP,
		Z_UP
	};

	class FBXSerializer : public IModelSerializer
	{
	public:
		std::vector<Ref<Mesh>> SerializeModel(const std::string& aSourcePath) override;

	private:
		void GatherMeshes();
		void ExtractEmbedded(const std::string& srcDirectory);
		int DetectMeshLOD(const ofbx::Mesh* mesh);

		// Working memory
		FBXSerializeSettings settings;
		float m_FBXScale;
		FBXOrientation m_Orientation;

		std::vector<Ref<Mesh>> m_Meshes {};

		ofbx::IScene* m_Scene{};
	};
}
