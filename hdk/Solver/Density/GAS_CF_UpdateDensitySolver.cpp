#include "GAS_CF_UpdateDensitySolver.h"

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

#include <Particle/SIM_CF_SPHSystemData.h>

bool GAS_CF_UpdateDensitySolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_UpdateDensitySolver::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_UpdateDensitySolver::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_UpdateDensitySolver::DATANAME = "CF_UpdateDensitySolver";
const SIM_DopDescription *GAS_CF_UpdateDensitySolver::getDopDescription()
{
	static std::array<PRM_Template, 8> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_update_density_solver",
								   "CF Update Density Solver",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_UpdateDensitySolver::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
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

	SIM_CF_SPHSystemData *sphdata = SIM_DATA_GET(*obj, SIM_CF_SPHSystemData::DATANAME, SIM_CF_SPHSystemData);
	if (!sphdata)
	{
		error_msg.appendSprintf("No Valid Target Data, From %s\n", DATANAME);
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

	sphdata->InnerPtr->UpdateDensities();

	size_t p_size = sphdata->InnerPtr->NumberOfParticles();
	const auto &cf_array_density = sphdata->InnerPtr->Densities();
	if (p_size != cf_array_density.Size().x)
	{
		error_msg.appendSprintf("Error Array Size::cf_array_density, From %s\n", DATANAME);
		return false;
	}

	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		GA_RWHandleF gdp_handle_density = gdp.findPointAttribute(SIM_CF_SPHSystemData::DENSITY_ATTRIBUTE_NAME);

		for (int i = 0; i < p_size; ++i)
		{
			GA_Offset pt_off = sphdata->GetParticleOffset(i, error_msg);

			if (!gdp_handle_density->getIndexMap().isOffsetActive(pt_off))
			{
				error_msg.appendSprintf("Offset INVALID, From %s\n", DATANAME);
				return false;
			}

			auto density = cf_array_density[i];
			gdp_handle_density.set(pt_off, density);
		}
	}
	return true;
}
