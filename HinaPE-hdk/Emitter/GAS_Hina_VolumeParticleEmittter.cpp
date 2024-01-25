#include "GAS_Hina_VolumeParticleEmittter.h"

#include <Particle/SIM_Hina_ParticleFluidData.h>
#include <Geometry/SIM_Hina_Box.h>
#include <Geometry/SIM_Hina_Plane.h>
#include <Geometry/SIM_Hina_Sphere.h>

#include "Core/Geometry/TriangleMesh3.hpp"
#include "Core/Geometry/ImplicitSurfaceSet.hpp"

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		VolumeParticleEmittter,
		false,
		NEW_BOOL_PARAMETER(IsOneShot, true) \
        NEW_FLOAT_PARAMETER(MaxNumOfOarticles, 100000) \
        ACTIVATE_GAS_GEOMETRY \
)

void GAS_Hina_VolumeParticleEmittter::_init()
{
	this->InnerPtr = nullptr;
}
void GAS_Hina_VolumeParticleEmittter::_makeEqual(const GAS_Hina_VolumeParticleEmittter *src)
{
	this->InnerPtr = src->InnerPtr;
}
bool GAS_Hina_VolumeParticleEmittter::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	if (!InnerPtr)
	{
		// Do init phase
		_AllSurfaces.Clear();
		search_and_add_all_sphere();
		search_and_add_all_box();
		search_and_add_geometry();
		if (_AllSurfaces.IsEmpty())
		{
			error_msg.appendSprintf("NO Source Geometry, From %s\n", DATANAME);
			return false;
		}
	}
	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)

	SIM_GeometryCopy *geo = getGeometryCopy(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

	}

	return true;
}

void GAS_Hina_VolumeParticleEmittter::search_and_add_all_sphere()
{
	SIM_ConstDataArray CF_Spheres;
	filterConstSubData(CF_Spheres, nullptr, SIM_DataFilterByType("SIM_CF_Sphere"), nullptr, SIM_DataFilterNone());
	for (const auto &data: CF_Spheres)
	{
		const SIM_Hina_Sphere *sphere = static_cast<const SIM_Hina_Sphere *>(data);
		auto suface_ptr = sphere->RuntimeConstruct();
		_AllSurfaces.Append(suface_ptr);
	}
}
void GAS_Hina_VolumeParticleEmittter::search_and_add_geometry()
{
	SIM_ConstDataArray CF_Boxes;
	filterConstSubData(CF_Boxes, nullptr, SIM_DataFilterByType("SIM_CF_Box"), nullptr, SIM_DataFilterNone());
	for (const auto &data: CF_Boxes)
	{
		const SIM_Hina_Box *box = static_cast<const SIM_Hina_Box *>(data);
		auto suface_ptr = box->RuntimeConstruct();
		_AllSurfaces.Append(suface_ptr);
	}
}
void GAS_Hina_VolumeParticleEmittter::search_and_add_all_plane()
{
	SIM_ConstDataArray CF_Planes;
	filterConstSubData(CF_Planes, nullptr, SIM_DataFilterByType("SIM_CF_Plane"), nullptr, SIM_DataFilterNone());
	for (const auto &data: CF_Planes)
	{
		const SIM_Hina_Plane *plane = static_cast<const SIM_Hina_Plane *>(data);
		auto suface_ptr = plane->RuntimeConstruct();
		_AllSurfaces.Append(suface_ptr);
	}
}
void GAS_Hina_VolumeParticleEmittter::search_and_add_all_box()
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
