#include "tgpch.h"
#include "FBXSerializer.h"

#include <filesystem>

#include "CommonUtilities/File.h"
#include "Tupla/Core/Application.h"
#include "CommonUtilities/Strings.hpp"
#include "CommonUtilities/Math/Matrices/Matrix4x4.hpp"
#include "Tupla/Renderer/Primitives/Mesh.h"
#include "Tupla/Renderer/Primitives/Vertex.h"

std::vector<Tupla::Ref<Tupla::Mesh>> Tupla::FBXSerializer::SerializeModel(const std::string& aSourcePath)
{
	if(m_Scene)
	{
		m_Scene->destroy();
		m_Scene = nullptr;
		m_Meshes.clear();
	}

	std::vector<std::byte> data;
	if (!CU::ReadFileBinary(aSourcePath.c_str(), data))
	{
		LOG_ERROR("Failed to read model: {}", aSourcePath.c_str());
		return {};
	}

	m_Scene = ofbx::load(reinterpret_cast<const u8*>(data.data()), static_cast<i32>(data.size()), static_cast<u16>(ofbx::LoadFlags::NONE), nullptr, nullptr);

	if (!m_Scene)
	{
		LOG_ERROR("Failed to import \"{}: {}\n"
			"Please try to convert the FBX file with Autodesk FBX Converter or some other software to the latest version.",
			aSourcePath, ofbx::getError()
		);
		return {};
	}

	m_FBXScale = m_Scene->getGlobalSettings()->UnitScaleFactor;

	const ofbx::GlobalSettings* gSettings = m_Scene->getGlobalSettings();

	switch (gSettings->OriginalUpAxis)
	{
	case ofbx::UpVector_AxisX: m_Orientation = FBXOrientation::X_UP; break;
	case ofbx::UpVector_AxisY: m_Orientation = FBXOrientation::Y_UP; break;
	case ofbx::UpVector_AxisZ: m_Orientation = FBXOrientation::Z_UP; break;
	}

	ExtractEmbedded(std::filesystem::path(aSourcePath).parent_path().string());
	GatherMeshes();
	return m_Meshes;
}

void Tupla::FBXSerializer::GatherMeshes()
{
	int meshCount = m_Scene->getMeshCount();

	for (int meshId = 0; meshId < meshCount; ++meshId)
	{
		const ofbx::Mesh* fbxMesh = m_Scene->getMesh(meshId);
		const int materialCount = fbxMesh->getMaterialCount();

		for (int j = 0; j < materialCount; ++j)
		{
			Ref<Mesh> mesh = CreateRef<Mesh>(fbxMesh->name);
			m_Meshes.push_back(mesh);

			mesh->myIsSkinned = false;

			const ofbx::Skin* skin = fbxMesh->getSkin();
			if(skin)
			{
				for (int i = 0; i < skin->getClusterCount(); ++i)
				{
					if(skin->getCluster(i)->getIndicesCount() > 0)
					{
						mesh->myIsSkinned = true;
						break;
					}
				}
			}

			// TODO: Create a valid material and copy relevant information!
			mesh->mySubMesh = materialCount > 1 ? j : -1;
			mesh->myLOD = DetectMeshLOD(fbxMesh);

			const ofbx::GeometryData& geom = fbxMesh->getGeometryData();
			ofbx::Vec3Attributes positions = geom.getPositions();
			ofbx::Vec3Attributes normals = geom.getNormals();
			ofbx::Vec3Attributes tangents = geom.getTangents();
			ofbx::Vec4Attributes colors = settings.importVertexColors ? geom.getColors() : ofbx::Vec4Attributes{{}, 0, 0 };
			ofbx::Vec2Attributes uvs = geom.getUVs();
			const bool compute_tangents = !tangents.values && uvs.values;

			const auto partition = geom.getPartition(0);
			std::vector<Vertex> vertices(geom.getPositions().count);
			std::vector<u32> indices(partition.max_polygon_triangles * 3);
			std::vector<u32> v{};

			CU::Matrix4x4<float> matrix;

#pragma region triangulation
			if(!indices.empty())
			{
				for (int polygonIdx = 0; polygonIdx < partition.polygon_count; ++polygonIdx)
				{
					const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygonIdx];
					u32 tri_count = ofbx::triangulate(geom, polygon, reinterpret_cast<i32*>(indices.data()));

					for (u32 i = 0; i < tri_count; ++i) {
						ofbx::Vec3 pos = positions.get(indices[i]);
						pos = FixOrientation(pos); // TODO: Fix orientation for wrong UP vectors!
						vertices[indices[i]].Position = (static_cast<CU::Vector3f>(pos).ToVec4(1) * matrix).ToVec3() * (settings.meshScale * m_FBXScale);

						if (normals.values) {
							auto norm = FixOrientation((static_cast<CU::Vector3f>(normals.get(indices[i])).ToVec4(1) * matrix).ToVec3().GetNormalized()).ToVec4();
							vertices[indices[i]].Normal = CU::Pack4b(norm);
						}

						if (uvs.values) {
							auto [x, y] = uvs.get(indices[i]);
							vertices[indices[i]].UV = { x, 1 - y }; // THIS REMAPS THE UV COORDINATES TO WHAT WE'RE USED TO.
						}

						if (colors.values && settings.importVertexColors) {
							CU::Vector4f c = colors.get(indices[i]);
							vertices[indices[i]].Color = CU::Pack4b(c);
						}

						if (tangents.values)
						{
							auto tan = (static_cast<CU::Vector3f>(tangents.get(indices[i])).ToVec4() * matrix).ToVec3().GetNormalized();
							vertices[indices[i]].Tangent = CU::Pack4b(tan.ToVec4());
						}

						v.push_back(indices[i]);

						/*if (mesh->myIsSkinned) { TODO: LATER!
							if (positions.indices) {
								write(skinning[positions.indices[indices[i]]].joints);
								write(skinning[positions.indices[indices[i]]].weights);
							}
							else {
								write(skinning[indices[i]].joints);
								write(skinning[indices[i]].weights);
							}
						}*/
					}
				}
			}
			else
			{
				LOG_ERROR("Importing FBX meshes without indices currently not supported!");
				m_Meshes.pop_back();
				continue;
				//for (int i = 0; i < vertices.size(); ++i)
				//{
				//	ofbx::Vec3 cp = positions.get(i);
				//	CU::Vector3f pos = (static_cast<CU::Vector3f>(cp).ToVec4(1) * matrix * (settings.meshScale * m_FBXScale)).ToVec3();
				//	pos = FixOrientation(pos);
				//	vertices[i].Position = pos;

				//	if (normals.values) {
				//		vertices[i].Normal = FixOrientation((static_cast<CU::Vector3f>(normals.get(i)).ToVec4(1) * matrix).ToVec3()).ToVec4(); // TODO: FIX ORIENTATION HERE TOO
				//	}

				//	if (uvs.values) {
				//		vertices[i].UV = uvs.get(i);
				//	}

				//	if (colors.values && settings.importVertexColors) {
				//		vertices[i].Color = colors.get(i);
				//	}

				//	if (tangents.values)
				//	{
				//		vertices[i].Tangent = static_cast<CU::Vector3f>(tangents.get(i)).ToVec4() * matrix;
				//	}
				//}
			}
#pragma endregion

			mesh->CreateMesh(vertices, v);
		}
	}
}

void Tupla::FBXSerializer::ExtractEmbedded(const std::string& srcDirectory)
{
	UNREFERENCED_PARAMETER(srcDirectory);
	// TODO: Extract embedded data to file!
	return;
	for (int i = 0; i < m_Scene->getEmbeddedDataCount(); ++i)
	{
		const ofbx::DataView embedded = m_Scene->getEmbeddedData(i);

		m_Scene->getEmbeddedFilename(i);
	}
}

int Tupla::FBXSerializer::DetectMeshLOD(const ofbx::Mesh* mesh)
{
	const char* nodeName = mesh->name;
	const char* lod_str = CU::FindInsensitive(mesh->name, "_LOD");

	if(!lod_str)
	{
		// TODO: Might do some more fancy name acrobatics later
		return 0;
	}

	lod_str += strlen("_LOD");

	int lod = atoi(lod_str);
	return lod;
}

CU::Vector3f Tupla::FBXSerializer::FixOrientation(const CU::Vector3f& vec) const
{
	switch (m_Orientation)
	{
	case FBXOrientation::Y_UP: return vec;
	case FBXOrientation::Z_UP: return { vec.x, vec.z, -vec.y };
	case FBXOrientation::X_UP: return { -vec.y, vec.x, vec.z };
	}

	LOG_ERROR("Orientation hasn't been implemented!");
	return {};
}
