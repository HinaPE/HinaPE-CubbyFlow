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
		ACTIVATE_GAS_GEOMETRY \
        NEW_BOOL_PARAMETER(IsOneShot, true) \
        NEW_FLOAT_PARAMETER(MaxNumOfParticles, 100000) \
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
	// Init Phase
	if (!InnerPtr)
	{
		// Do init phase
		_AllSurfaces.Clear();
		_search_and_add_all_sphere();
		_search_and_add_all_box();
		_search_and_add_geometry();
		if (_AllSurfaces.IsEmpty())
		{
			error_msg.appendSprintf("NO Source Geometry, From %s\n", DATANAME);
			return false;
		}
		CubbyFlow::ImplicitSurfaceSet3Ptr implicit = CubbyFlow::ImplicitSurfaceSet3::GetBuilder().WithExplicitSurfaces(_AllSurfaces).MakeShared();
		bool IsOneShot = getIsOneShot();
		size_t MaxNumberOfParticles = getMaxNumOfParticles();
		this->InnerPtr = CubbyFlow::VolumeParticleEmitter3::GetBuilder()
				.WithImplicitSurface(implicit)
				.WithIsOneShot(IsOneShot)
				.WithMaxNumberOfParticles(MaxNumberOfParticles)
				.MakeShared();
	}


	// Solver Phase
	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getGeometryCopy(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	UT_Vector3D MaxRegion = data->getFluidDomainD();
	CubbyFlow::BoundingBox3D FluidDomain(
			CubbyFlow::Vector3D(-MaxRegion.x() / 2, -MaxRegion.y() / 2, -MaxRegion.z() / 2),
			CubbyFlow::Vector3D(MaxRegion.x() / 2, MaxRegion.y() / 2, MaxRegion.z() / 2)
	);
	this->InnerPtr->SetTarget(data->InnerPtr);
	this->InnerPtr->SetSpacing(data->getTargetSpacing());
	this->InnerPtr->SetMaxRegion(FluidDomain);
	this->InnerPtr->Update(time, timestep);

	size_t pt_size = data->pt_size();
	// Sync To Geometry Spread Sheet
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		data->runtime_init_handles(gdp);

		for (int pt_idx = 0; pt_idx < pt_size; ++pt_idx)
		{
			if (data->state(pt_idx) == PARTICLE_STATE_NEW_ADDED)
			{
				GA_Offset pt_off = gdp.appendPoint();
				data->set_offset(pt_idx, pt_off);
				data->set_state(pt_idx, PARTICLE_STATE_CLEAN);
				data->set_gdp_index(pt_off, pt_idx);
			}
			data->set_gdp_state(pt_idx, data->state(pt_idx));
		}
	}
	data->sync_position(geo);
	data->sync_velocity(geo);
	data->sync_force(geo);

	return true;
}

void GAS_Hina_VolumeParticleEmittter::_search_and_add_all_sphere()
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
void GAS_Hina_VolumeParticleEmittter::_search_and_add_geometry()
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
void GAS_Hina_VolumeParticleEmittter::_search_and_add_all_plane()
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
void GAS_Hina_VolumeParticleEmittter::_search_and_add_all_box()
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
