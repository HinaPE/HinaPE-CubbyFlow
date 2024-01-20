#include "GAS_CF_VolumeParticleEmitter.h"

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_GuideShared.h>
#include <SIM/SIM_ColliderLabel.h>
#include <SIM/SIM_ForceGravity.h>
#include <SIM/SIM_Time.h>
#include <SIM/SIM_Utils.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>

#include <SOP/SOP_Node.h>

#include <GU/GU_Detail.h>
#include <GEO//GEO_PrimPoly.h>

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#include <Particle/SIM_CF_ParticleSystemData.h>
#include <Particle/SIM_CF_SPHSystemData.h>
#include <Geometry/SIM_CF_Sphere.h>
#include <Geometry/SIM_CF_Box.h>
#include <Geometry/SIM_CF_Plane.h>

#include "Core/Geometry/ImplicitSurfaceSet.hpp"
#include "Core/Geometry/TriangleMesh3.hpp"

bool GAS_CF_VolumeParticleEmitter::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!InnerPtr)
	{
		if (!InitRuntime(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
		{
			SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
			return false;
		}
	}

	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_VolumeParticleEmitter::initializeSubclass()
{
	SIM_Data::initializeSubclass();

	/// Implement Initialization Operator of Your Custom Fields
	this->InnerPtr = nullptr;
}

void GAS_CF_VolumeParticleEmitter::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
	const GAS_CF_VolumeParticleEmitter *src = SIM_DATA_CASTCONST(source, GAS_CF_VolumeParticleEmitter);

	/// Implement Equal Operator of Your Custom Fields
	this->InnerPtr = src->InnerPtr;
}

const char *GAS_CF_VolumeParticleEmitter::DATANAME = "CF_VolumeParticleEmitter";
const SIM_DopDescription *GAS_CF_VolumeParticleEmitter::getDopDescription()
{
	static PRM_Name IsOneShot("IsOneShot", "IsOneShot");

//	static PRM_Name Spacing("Spacing", "Spacing"); # Please use FluidDomain in SPHSystemData
//	static PRM_Default SpacingDefault(0.02);

//	static PRM_Name MaxRegion("MaxRegion", "MaxRegion"); # Please use Spacing in SPHSystemData
//	static std::array<PRM_Default, 3> MaxRegionDefault{2, 2, 2};

	static PRM_Name MaxNumberOfParticles("MaxNumberOfParticles", "MaxNumberOfParticles");
	static PRM_Default MaxNumberOfParticlesDefault(20000);

	static PRM_Name RandomSeed("RandomSeed", "RandomSeed");
	static PRM_Default RandomSeedDefault(0);

	static std::array<PRM_Template, 4> PRMS{
			PRM_Template(PRM_TOGGLE, 1, &IsOneShot, PRMoneDefaults),
			PRM_Template(PRM_INT, 1, &MaxNumberOfParticles, &MaxNumberOfParticlesDefault),
			PRM_Template(PRM_INT, 1, &RandomSeed, &RandomSeedDefault),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_volume_particle_emitter",
								   "CF Volume Particle Emitter",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

/**
 * This function will read each SIM_Geometry SubData attached to this Emitter, and combine them as a total Emitter Source
 *
 * [Notice]
 * - It would take A LOG OF TIME to construct Implicit Surface for a Geometry From a Mesh (SOP Networks, etc.)
 * - It is more recommended to use Our Own SIM_Geometry(SIM_CF_Sphere, etc.), It would be more efficient.
 */
bool GAS_CF_VolumeParticleEmitter::InitRuntime(SIM_Engine &, SIM_Object *, SIM_Time, SIM_Time, UT_WorkBuffer &error_msg)
{
	CubbyFlow::Array1<CubbyFlow::Surface3Ptr> MultipleSurfaces;

	// Find All SIM_CF_Sphere
	{
		SIM_ConstDataArray CF_Spheres;
		filterConstSubData(CF_Spheres, nullptr, SIM_DataFilterByType("SIM_CF_Sphere"), nullptr, SIM_DataFilterNone());
		for (const auto &data: CF_Spheres)
		{
			const SIM_CF_Sphere *sphere = static_cast<const SIM_CF_Sphere *>(data);
			auto suface_ptr = sphere->RuntimeConstructCFSphere();
			if (!suface_ptr)
			{
				error_msg.appendSprintf("SIM_CF_Sphere::CFSphere Construct Error, From %s\n", DATANAME);
				return false;
			}
			MultipleSurfaces.Append(suface_ptr);
		}
	}

	// Find All SIM_CF_Box
	{
		SIM_ConstDataArray CF_Boxes;
		filterConstSubData(CF_Boxes, nullptr, SIM_DataFilterByType("SIM_CF_Box"), nullptr, SIM_DataFilterNone());
		for (const auto &data: CF_Boxes)
		{
			const SIM_CF_Box *box = static_cast<const SIM_CF_Box *>(data);
			auto suface_ptr = box->RuntimeConstructCFBox();
			if (!suface_ptr)
			{
				error_msg.appendSprintf("SIM_CF_Box::CF_Box Construct Error, From %s\n", DATANAME);
				return false;
			}
			MultipleSurfaces.Append(suface_ptr);
		}
	}

	// Find All SIM_CF_Plane
	{
		SIM_ConstDataArray CF_Planes;
		filterConstSubData(CF_Planes, nullptr, SIM_DataFilterByType("SIM_CF_Plane"), nullptr, SIM_DataFilterNone());
		for (const auto &data: CF_Planes)
		{
			const SIM_CF_Plane *plane = static_cast<const SIM_CF_Plane *>(data);
			auto suface_ptr = plane->RuntimeConstructCFPlane();
			if (!suface_ptr)
			{
				error_msg.appendSprintf("SIM_CF_Plane::CF_Plane Construct Error, From %s\n", DATANAME);
				return false;
			}
			MultipleSurfaces.Append(suface_ptr);
		}
	}

	// [Notice] This is very slow when Building SDF!
	// For performance, We only support upto 1 external SIM_Geometry
	SIM_Geometry *src_geo = SIM_DATA_GET(*this, SIM_GEOMETRY_DATANAME, SIM_Geometry);
	if (src_geo)
	{
		SIM_GeometryAutoReadLock lock(src_geo);
		const GU_Detail *gdp_source = lock.getGdp();
		if (!gdp_source)
		{
			error_msg.appendSprintf("Source Geometry GDP is nullptr, From %s\n", DATANAME);
			return false;
		}

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
				if (!poly)
				{
					error_msg.appendSprintf("ERROR ON CONVERT PRIM TO POLY, From %s\n", DATANAME);
					return false;
				}

				// Triangulate Polygon
				std::vector<size_t> polyIndices;
				for (int vi = 0; vi < poly->getVertexCount(); ++vi)
					polyIndices.push_back(poly->getPointIndex(vi));
				for (size_t i = 1; i < polyIndices.size() - 1; ++i)
					point_indices.Append({polyIndices[0], polyIndices[i + 1], polyIndices[i]}); // notice the normal

			}
		}
		CubbyFlow::TriangleMesh3Ptr mesh = CubbyFlow::TriangleMesh3::GetBuilder().WithPoints(points).WithPointIndices(point_indices).MakeShared();

		MultipleSurfaces.Append(mesh);
	}

	if (MultipleSurfaces.IsEmpty())
	{
		error_msg.appendSprintf("NO Source Geometry, From %s\n", DATANAME);
		return false;
	}

	CubbyFlow::ImplicitSurfaceSet3Ptr implicit = CubbyFlow::ImplicitSurfaceSet3::GetBuilder().WithExplicitSurfaces(MultipleSurfaces).MakeShared();

	bool IsOneShot = getIsOneShot();
	size_t MaxNumberOfParticles = getMaxNumberOfParticles();
	size_t RandomSeed = getRandomSeed();

	this->InnerPtr = CubbyFlow::VolumeParticleEmitter3::GetBuilder()
			.WithImplicitSurface(implicit)
			.WithIsOneShot(IsOneShot)
			.WithMaxNumberOfParticles(MaxNumberOfParticles)
			.WithRandomSeed(RandomSeed)
			.MakeShared();

	return true;
}

bool GAS_CF_VolumeParticleEmitter::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_GeometryCopy *geo = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy);
	if (!geo)
	{
		error_msg.appendSprintf("Geometry Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_ParticleSystemData *psdata = SIM_DATA_GET(*obj, SIM_CF_ParticleSystemData::DATANAME, SIM_CF_ParticleSystemData);
	SIM_CF_SPHSystemData *sphdata = SIM_DATA_GET(*obj, SIM_CF_SPHSystemData::DATANAME, SIM_CF_SPHSystemData);
	if (!psdata && !sphdata)
	{
		error_msg.appendSprintf("No Valid Target Data, From %s\n", DATANAME);
		return false;
	}

	if (psdata)
	{
		if (!psdata->Configured)
		{
			error_msg.appendSprintf("ParticleSystemData Not Configured Yet, From %s\n", DATANAME);
			return false;
		}

		if (!psdata->InnerPtr)
		{
			error_msg.appendSprintf("ParticleSystemData InnerPtr is nullptr, From %s\n", DATANAME);
			return false;
		}

		this->InnerPtr->SetTarget(psdata->InnerPtr);
		this->InnerPtr->SetSpacing(psdata->getTargetSpacing());
		UT_Vector3 MaxRegion = psdata->getParticlesDomain();
		CubbyFlow::BoundingBox3D fluid_domain(
				CubbyFlow::Vector3D(-MaxRegion.x() / 2, -MaxRegion.y() / 2, -MaxRegion.z() / 2),
				CubbyFlow::Vector3D(MaxRegion.x() / 2, MaxRegion.y() / 2, MaxRegion.z() / 2)
		);
		this->InnerPtr->SetMaxRegion(fluid_domain);
		this->InnerPtr->Update(time, timestep);
	}

	if (sphdata)
	{
		if (!sphdata->Configured)
		{
			error_msg.appendSprintf("SPHSystemData Not Configured Yet, From %s\n", DATANAME);
			return false;
		}

		if (!sphdata->InnerPtr)
		{
			error_msg.appendSprintf("SPHSystemData InnerPtr is nullptr, From %s\n", DATANAME);
			return false;
		}

		this->InnerPtr->SetTarget(sphdata->InnerPtr);
		this->InnerPtr->SetSpacing(sphdata->getTargetSpacing());
		UT_Vector3 MaxRegion = sphdata->getFluidDomain();
		CubbyFlow::BoundingBox3D fluid_domain(
				CubbyFlow::Vector3D(-MaxRegion.x() / 2, -MaxRegion.y() / 2, -MaxRegion.z() / 2),
				CubbyFlow::Vector3D(MaxRegion.x() / 2, MaxRegion.y() / 2, MaxRegion.z() / 2)
		);
		this->InnerPtr->SetMaxRegion(fluid_domain);
		this->InnerPtr->Update(time, timestep);
	}

	return true;
}

SIM_Guide *GAS_CF_VolumeParticleEmitter::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}

void GAS_CF_VolumeParticleEmitter::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
}
