#include "GAS_Hina_VolumeGridEmitter.h"

#include <Particle/SIM_Hina_ParticleFluidData.h>
#include <Geometry/SIM_Hina_Box.h>
#include <Geometry/SIM_Hina_Plane.h>
#include <Geometry/SIM_Hina_Sphere.h>

#include "Core/Geometry/TriangleMesh3.hpp"
#include "Core/Geometry/ImplicitSurfaceSet.hpp"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		VolumeGridEmitter,
		false,
		ACTIVATE_GAS_DENSITY \
        NEW_BOOL_PARAMETER(IsOneShot, true) \
)

void GAS_Hina_VolumeGridEmitter::_init()
{
	this->InnerPtr = nullptr;
}
void GAS_Hina_VolumeGridEmitter::_makeEqual(const GAS_Hina_VolumeGridEmitter *src)
{
	this->InnerPtr = src->InnerPtr;
}
bool GAS_Hina_VolumeGridEmitter::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	// Init Phase
	if (!InnerPtr)
	{
		// Do init phase
		_AllSurfaces.Clear();
		_search_and_add_all_box();
		_search_and_add_all_sphere();
		_search_and_add_all_plane();
		_search_and_add_geometry();
		if (_AllSurfaces.IsEmpty())
		{
			error_msg.appendSprintf("NO Source Geometry, From %s\n", DATANAME);
			return false;
		}
		CubbyFlow::ImplicitSurfaceSet3Ptr implicit = CubbyFlow::ImplicitSurfaceSet3::GetBuilder().WithExplicitSurfaces(_AllSurfaces).MakeShared();
		bool IsOneShot = getIsOneShot();
		this->InnerPtr = CubbyFlow::VolumeGridEmitter3::GetBuilder()
				.WithSourceRegion(implicit)
				.WithIsOneShot(IsOneShot)
				.MakeShared();
	}

	// Solver Phase

	return true;
}
void GAS_Hina_VolumeGridEmitter::_search_and_add_all_box()
{
	SIM_ConstDataArray Hina_Boxes;
	filterConstSubData(Hina_Boxes, nullptr, SIM_DataFilterByType("SIM_Hina_Box"), nullptr, SIM_DataFilterNone());
	for (const auto &data: Hina_Boxes)
	{
		const auto *box = dynamic_cast<const SIM_Hina_Box *>(data);
		auto suface_ptr = box->RuntimeConstruct();
		_AllSurfaces.Append(suface_ptr);
	}
}
void GAS_Hina_VolumeGridEmitter::_search_and_add_all_sphere()
{
	SIM_ConstDataArray Hina_Spheres;
	filterConstSubData(Hina_Spheres, nullptr, SIM_DataFilterByType("SIM_Hina_Sphere"), nullptr, SIM_DataFilterNone());
	for (const auto &data: Hina_Spheres)
	{
		const auto *sphere = dynamic_cast<const SIM_Hina_Sphere *>(data);
		auto suface_ptr = sphere->RuntimeConstruct();
		_AllSurfaces.Append(suface_ptr);
	}
}
void GAS_Hina_VolumeGridEmitter::_search_and_add_all_plane()
{
	SIM_ConstDataArray Hina_Planes;
	filterConstSubData(Hina_Planes, nullptr, SIM_DataFilterByType("SIM_Hina_Plane"), nullptr, SIM_DataFilterNone());
	for (const auto &data: Hina_Planes)
	{
		const auto *plane = dynamic_cast<const SIM_Hina_Plane *>(data);
		auto suface_ptr = plane->RuntimeConstruct();
		_AllSurfaces.Append(suface_ptr);
	}
}
void GAS_Hina_VolumeGridEmitter::_search_and_add_geometry()
{
	// For performance, We only support upto 1 external SIM_Geometry
	SIM_Geometry *src_geo = SIM_DATA_GET(*this, SIM_GEOMETRY_DATANAME, SIM_Geometry);
	if (src_geo)
	{
		SIM_GeometryAutoReadLock lock(src_geo);
		const GU_Detail *gdp_source = lock.getGdp();

		CubbyFlow::TriangleMesh3::PointArray points;
		CubbyFlow::TriangleMesh3::IndexArray point_indices;
		{
			GA_Offset pt_off;
			GA_FOR_ALL_PTOFF(gdp_source, pt_off)
				{
					const UT_Vector3 pos = gdp_source->getPos3(pt_off);
					points.Append({pos.x(), pos.y(), pos.z()});
				}

			const GEO_Primitive *prim;
			GA_FOR_ALL_PRIMITIVES(gdp_source, prim)
			{
				const auto *poly = dynamic_cast<const GEO_PrimPoly *>(prim);

				// Triangulate Polygon
				std::vector<size_t> polyIndices;
				for (int vi = 0; vi < poly->getVertexCount(); ++vi)
					polyIndices.push_back(poly->getPointIndex(vi));
				for (size_t i = 1; i < polyIndices.size() - 1; ++i)
					point_indices.Append({polyIndices[0], polyIndices[i + 1], polyIndices[i]}); // notice the normal
			}
		}
		CubbyFlow::TriangleMesh3Ptr mesh = CubbyFlow::TriangleMesh3::GetBuilder().WithPoints(points).WithPointIndices(point_indices).MakeShared();

		_AllSurfaces.Append(mesh);
	}
}
