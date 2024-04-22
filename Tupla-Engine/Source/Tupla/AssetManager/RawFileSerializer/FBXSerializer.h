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
		Y_UP,
		Z_UP,
		X_UP,
	};

	class FBXSerializer : public IModelSerializer
	{
	public:
		std::vector<Ref<Mesh>> SerializeModel(const std::string& aSourcePath) override;

	private:
		void GatherMeshes();
		void ExtractEmbedded(const std::string& srcDirectory);
		int DetectMeshLOD(const ofbx::Mesh* mesh);

		CU::Vector3f FixOrientation(const CU::Vector3f& vec) const;

		FBXSerializeSettings settings;

		float m_FBXScale = 1.f;
		FBXOrientation m_Orientation = FBXOrientation::Y_UP;
		std::vector<Ref<Mesh>> m_Meshes {};

		ofbx::IScene* m_Scene{};
	};
}
