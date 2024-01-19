#include "GAS_CF_UpdateToGeometrySheet.h"

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

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

bool GAS_CF_UpdateToGeometrySheet::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_UpdateToGeometrySheet::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_UpdateToGeometrySheet::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_UpdateToGeometrySheet::DATANAME = "CF_UpdateToGeometrySheetSolver";
const SIM_DopDescription *GAS_CF_UpdateToGeometrySheet::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_update_geometry_sheet_solver",
								   "CF Update To Geometry Sheet Solver",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_UpdateToGeometrySheet::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
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

		size_t p_size = psdata->InnerPtr->NumberOfParticles();
		const auto &cf_array_pos = psdata->InnerPtr->Positions();
		if (p_size != cf_array_pos.Size().x)
		{
			error_msg.appendSprintf("Error Array Size::cf_array_pos, From %s\n", DATANAME);
			return false;
		}
		const auto &cf_array_vel = psdata->InnerPtr->Velocities();
		if (p_size != cf_array_vel.Size().x)
		{
			error_msg.appendSprintf("Error Array Size::cf_array_vel, From %s\n", DATANAME);
			return false;
		}
		const auto &cf_array_force = psdata->InnerPtr->Forces();
		if (p_size != cf_array_force.Size().x)
		{
			error_msg.appendSprintf("Error Array Size::cf_array_force, From %s\n", DATANAME);
			return false;
		}

		{
			SIM_GeometryAutoWriteLock lock(geo);
			GU_Detail &gdp = lock.getGdp();

			for (int i = 0; i < p_size; ++i)
			{
				SIM_CF_ParticleSystemData::ParticleState state = psdata->GetParticleState(i, error_msg);
				if (state == SIM_CF_ParticleSystemData::PARTICLE_ADDED)
				{
					GA_Offset new_offset = gdp.appendPoint();
					psdata->SetParticleOffset(i, new_offset, error_msg);
				}
				GA_Offset pt_off = psdata->GetParticleOffset(i, error_msg);
				auto pos = cf_array_pos[i];

				gdp.setPos3(pt_off, UT_Vector3D{pos.x, pos.y, pos.z});
				psdata->SetParticleState(i, SIM_CF_ParticleSystemData::PARTICLE_CLEAN, error_msg);

				GA_RWHandleV3 gdp_handle_vel = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_VELOCITY));
				GA_RWHandleV3 gdp_handle_force = gdp.findPointAttribute(SIM_CF_ParticleSystemData::FORCE_ATTRIBUTE_NAME);
			}
		}
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

		size_t p_size = sphdata->InnerPtr->NumberOfParticles();
		const auto &cf_array_pos = sphdata->InnerPtr->Positions();
		const auto &cf_array_vel = sphdata->InnerPtr->Velocities();
		const auto &cf_array_force = sphdata->InnerPtr->Forces();

		{
			SIM_GeometryAutoWriteLock lock(geo);
			GU_Detail &gdp = lock.getGdp();

			for (int i = 0; i < p_size; ++i)
			{
				SIM_CF_SPHSystemData::ParticleState state = sphdata->GetParticleState(i, error_msg);
				if (state == SIM_CF_ParticleSystemData::PARTICLE_ADDED)
				{
					GA_Offset new_offset = gdp.appendPoint();
					sphdata->SetParticleOffset(i, new_offset, error_msg);
				}
				GA_Offset pt_off = sphdata->GetParticleOffset(i, error_msg);
				auto pos = cf_array_pos[i];

				gdp.setPos3(pt_off, UT_Vector3D{pos.x, pos.y, pos.z});
				sphdata->SetParticleState(i, SIM_CF_SPHSystemData::PARTICLE_CLEAN, error_msg);

				GA_RWHandleV3 gdp_handle_vel = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_VELOCITY));
				GA_RWHandleV3 gdp_handle_force = gdp.findPointAttribute(SIM_CF_ParticleSystemData::FORCE_ATTRIBUTE_NAME);
			}
		}
	}

	return true;
}
