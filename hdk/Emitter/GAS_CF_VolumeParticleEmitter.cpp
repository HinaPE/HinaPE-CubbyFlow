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

#include "Core/Geometry/ImplicitTriangleMesh3.hpp"

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

	/// Implement Equal Operator of Your Custom Fields
	this->InnerPtr = nullptr; // Post Init in Runtime
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

	static PRM_Name Spacing("Spacing", "Spacing");
	static PRM_Default SpacingDefault(0.02);

	static PRM_Name MaxRegion("MaxRegion", "MaxRegion");
	static std::array<PRM_Default, 3> MaxRegionDefault{2, 2, 2};

	static PRM_Name MaxNumberOfParticles("MaxNumberOfParticles", "MaxNumberOfParticles");
	static PRM_Default MaxNumberOfParticlesDefault(10e8);

	static PRM_Name RandomSeed("RandomSeed", "RandomSeed");
	static PRM_Default RandomSeedDefault(0);

	static std::array<PRM_Template, 8> PRMS{
			PRM_Template(PRM_TOGGLE, 1, &IsOneShot, PRMzeroDefaults),
			PRM_Template(PRM_FLT, 1, &Spacing, &SpacingDefault),
			PRM_Template(PRM_FLT, 3, &MaxRegion, MaxRegionDefault.data()),
			PRM_Template(PRM_FLT, 1, &MaxNumberOfParticles, &MaxNumberOfParticlesDefault),
			PRM_Template(PRM_FLT, 1, &RandomSeed, &RandomSeedDefault),
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

bool GAS_CF_VolumeParticleEmitter::InitRuntime(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	bool IsOneShot = getIsOneShot();
	UT_Vector3 MaxRegion = getMaxRegion();
	fpreal Spacing = getSpacing();
	fpreal MaxNumberOfParticles = getMaxNumberOfParticles();
	fpreal RandomSeed = getRandomSeed();

	SIM_Geometry *src_geo = SIM_DATA_GET(*this, SIM_GEOMETRY_DATANAME, SIM_Geometry);
	if (!src_geo)
	{
		error_msg.appendSprintf("NO Source Geometry, From %s\n", DATANAME);
		return false;
	}
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
	CubbyFlow::ImplicitTriangleMesh3Ptr implicit = CubbyFlow::ImplicitTriangleMesh3::GetBuilder().WithTriangleMesh(mesh).WithResolutionX(256).MakeShared();

	CubbyFlow::BoundingBox3D bbox(
			CubbyFlow::Vector3D(-MaxRegion.x() / 2, -MaxRegion.y() / 2, -MaxRegion.z() / 2),
			CubbyFlow::Vector3D(MaxRegion.x() / 2, MaxRegion.y() / 2, MaxRegion.z() / 2)
	);

	/// Implement Initializations of Your Custom Fields
	this->InnerPtr = CubbyFlow::VolumeParticleEmitter3::GetBuilder()
			.WithImplicitSurface(implicit)
			.WithSpacing(Spacing)
			.WithMaxRegion(bbox)
			.WithIsOneShot(IsOneShot)
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
	return;
}
