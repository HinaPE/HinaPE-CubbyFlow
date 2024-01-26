#include "GAS_CF_CommitCache.h"

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

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#include <Particle/SPHSystemData/SIM_CF_SPHSystemData.h>

bool GAS_CF_CommitCache::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_CommitCache::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_CommitCache::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_CommitCache::DATANAME = "CF_CommitCache";
const SIM_DopDescription *GAS_CF_CommitCache::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_commit_cache",
								   "CF Commit Cache",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_CommitCache::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_SPHSystemData *sphdata = SIM_DATA_GET(*obj, SIM_CF_SPHSystemData::DATANAME, SIM_CF_SPHSystemData);
	if (!sphdata)
	{
		error_msg.appendSprintf("No Valid Target Data, From %s\n", DATANAME);
		return false;
	}

	SIM_GeometryCopy *geo = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy);
	if (!geo)
	{
		error_msg.appendSprintf("Geometry Is Null, From %s\n", DATANAME);
		return false;
	}

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

	if (sphdata->newPositions_Cache.IsEmpty())
	{
		error_msg.appendSprintf("newPositions_Cache is Empty, From %s\n", DATANAME);
		return false;
	}
	if (sphdata->newVelocities_Cache.IsEmpty())
	{
		error_msg.appendSprintf("newVelocities_Cache is Empty, From %s\n", DATANAME);
		return false;
	}

	// Commit Data in CubbyFlow
	for (int i = 0; i < sphdata->newPositions_Cache.Size().x; ++i)
		sphdata->InnerPtr->Positions()[i] = sphdata->newPositions_Cache[i];
	for (int i = 0; i < sphdata->newPositions_Cache.Size().x; ++i)
		sphdata->InnerPtr->Velocities()[i] = sphdata->newVelocities_Cache[i];


	// Commit Data in Geometry Sheet
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		GA_RWHandleV3 gdp_handle_pos = gdp.getP();
		GA_RWHandleV3 gdp_handle_vel = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_VELOCITY));

		GA_RWHandleV3 gdp_handle_new_pos = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEW_POSITION_CACHE_ATTRIBUTE_NAME);
		GA_RWHandleV3 gdp_handle_new_vel = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEW_VELOCITY_CACHE_ATTRIBUTE_NAME);

		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				UT_Vector3 new_pos = gdp_handle_new_pos.get(pt_off);
				UT_Vector3 new_vel = gdp_handle_new_vel.get(pt_off);
				gdp_handle_pos.set(pt_off, new_pos);
				gdp_handle_vel.set(pt_off, new_vel);
			}
	}

	return true;
}
