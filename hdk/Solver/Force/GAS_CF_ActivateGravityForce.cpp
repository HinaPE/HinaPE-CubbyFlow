#include "GAS_CF_ActivateGravityForce.h"

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

#include <Particle/SIM_CF_ParticleSystemData.h>
#include <Particle/SIM_CF_SPHSystemData.h>

bool GAS_CF_ActivateGravityForce::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_ActivateGravityForce::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_ActivateGravityForce::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_ActivateGravityForce::DATANAME = "CF_ActivateGravityForce";
const SIM_DopDescription *GAS_CF_ActivateGravityForce::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_activate_gravity_force",
								   "CF Activate Gravity Force",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true); // We Only Need ONE Gravity in a simulation
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_ActivateGravityForce::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_ParticleSystemData *psdata = SIM_DATA_GET(*obj, SIM_CF_ParticleSystemData::DATANAME, SIM_CF_ParticleSystemData);
	SIM_CF_SPHSystemData *sphdata = SIM_DATA_GET(*obj, SIM_CF_SPHSystemData::DATANAME, SIM_CF_SPHSystemData);
	if (!psdata && !sphdata)
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

		CubbyFlow::ArrayView1<CubbyFlow::Vector3D> forces = psdata->InnerPtr->Forces();
		forces.Fill(CubbyFlow::Vector3D{});

		// Clear Force in Geometry Sheet
		{
			SIM_GeometryAutoWriteLock lock(geo);
			GU_Detail &gdp = lock.getGdp();

			GA_RWHandleV3 gdp_handle_force = gdp.findPointAttribute(SIM_CF_ParticleSystemData::FORCE_ATTRIBUTE_NAME);

			GA_Offset pt_off;
			GA_FOR_ALL_PTOFF(&gdp, pt_off)
				{
					gdp_handle_force.set(pt_off, UT_Vector3(0.));
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

		// Add Gravity Force in CubbyFlow
		UT_Vector3 gravity = sphdata->getGravity();
		const size_t n = sphdata->InnerPtr->NumberOfParticles();
		CubbyFlow::ArrayView1<CubbyFlow::Vector3D> forces = sphdata->InnerPtr->Forces();
		const double mass = sphdata->InnerPtr->Mass();

		CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, n, [&](size_t i)
		{
			UT_Vector3 force = mass * gravity;
			forces[i] += CubbyFlow::Vector3D{force.x(), force.y(), force.z()};
		});

		// Add Gravity Force in Geometry Sheet
		{
			SIM_GeometryAutoWriteLock lock(geo);
			GU_Detail &gdp = lock.getGdp();

			GA_RWHandleV3 gdp_handle_force = gdp.findPointAttribute(SIM_CF_SPHSystemData::FORCE_ATTRIBUTE_NAME);
			UT_Vector3 gravity_force = mass * gravity;

			GA_Offset pt_off;
			GA_FOR_ALL_PTOFF(&gdp, pt_off)
				{
					UT_Vector3 exist_force = gdp_handle_force.get(pt_off);
					gdp_handle_force.set(pt_off, gravity_force + exist_force);
				}
		}
	}

	return true;
}
