#include "GAS_CF_Semi_Implicit_Euler.h"

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

bool GAS_CF_Semi_Implicit_Euler::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_Semi_Implicit_Euler::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_Semi_Implicit_Euler::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_Semi_Implicit_Euler::DATANAME = "CF_Semi_Implicit_Euler";
const SIM_DopDescription *GAS_CF_Semi_Implicit_Euler::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_semi_implicit_euler",
								   "CF Semi Implicit Euler",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true); // We Only Need ONE in a simulation
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_Semi_Implicit_Euler::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
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

	// Apply Semi Implicit Euler in CubbyFlow
	{
		using namespace CubbyFlow;
		auto &m_particleSystemData = sphdata->InnerPtr;
		const size_t n = m_particleSystemData->NumberOfParticles();
		ArrayView1<Vector3D> forces = m_particleSystemData->Forces();
		ArrayView1<Vector3D> velocities = m_particleSystemData->Velocities();
		ArrayView1<Vector3D> positions = m_particleSystemData->Positions();
		const double mass = m_particleSystemData->Mass();

		auto &m_newPositions = sphdata->newPositions_Cache;
		auto &m_newVelocities = sphdata->newVelocities_Cache;
		double timeStepInSeconds = timestep;
		m_newPositions.Resize(n);
		m_newVelocities.Resize(n);
		ParallelFor(ZERO_SIZE, n, [&](size_t i)
		{
			// Integrate velocity first
			Vector3D &newVelocity = m_newVelocities[i];
			newVelocity = velocities[i] + timeStepInSeconds * forces[i] / mass;

			// Integrate position.
			Vector3D &newPosition = m_newPositions[i];
			newPosition = positions[i] + timeStepInSeconds * newVelocity;
		});
	}


	// Sync Geometry Sheet
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

		GA_RWHandleV3 gdp_handle_new_pos = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEW_POSITION_CACHE_ATTRIBUTE_NAME);
		GA_RWHandleV3 gdp_handle_new_vel = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEW_VELOCITY_CACHE_ATTRIBUTE_NAME);
		GA_RWHandleI gdp_handle_CL_PT_IDX = gdp.findPointAttribute(SIM_CF_SPHSystemData::CL_PT_IDX_ATTRIBUTE_NAME);

		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				int cl_index = gdp_handle_CL_PT_IDX.get(pt_off);
				UT_Vector3 new_pos = UT_Vector3D{sphdata->newPositions_Cache[cl_index].x, sphdata->newPositions_Cache[cl_index].y, sphdata->newPositions_Cache[cl_index].z};
				gdp_handle_new_pos.set(pt_off, new_pos);
				UT_Vector3 new_vel = UT_Vector3D{sphdata->newVelocities_Cache[cl_index].x, sphdata->newVelocities_Cache[cl_index].y, sphdata->newVelocities_Cache[cl_index].z};
				gdp_handle_new_vel.set(pt_off, new_vel);
			}
	}

	return true;
}
