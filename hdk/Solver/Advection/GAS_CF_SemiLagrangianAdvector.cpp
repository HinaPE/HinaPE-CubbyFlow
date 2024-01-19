#include "GAS_CF_SemiLagrangianAdvector.h"

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

bool GAS_CF_SemiLagrangianAdvector::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_SemiLagrangianAdvector::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_SemiLagrangianAdvector::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_SemiLagrangianAdvector::DATANAME = "CF_SemiLagrangianAdvector";
const SIM_DopDescription *GAS_CF_SemiLagrangianAdvector::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_semilagrangian_advector",
								   "CF Semi-Lagrangian Advector",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true); // We Only Need ONE Advector in a simulation
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_SemiLagrangianAdvector::Solve(SIM_Engine &, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
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

	fpreal dt = timestep;

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

		const size_t n = psdata->InnerPtr->NumberOfParticles();
		CubbyFlow::ArrayView1<CubbyFlow::Vector3D> forces = psdata->InnerPtr->Forces();
		CubbyFlow::ArrayView1<CubbyFlow::Vector3D> velocities = psdata->InnerPtr->Velocities();
		CubbyFlow::ArrayView1<CubbyFlow::Vector3D> positions = psdata->InnerPtr->Positions();
		const double mass = psdata->InnerPtr->Mass();

		CubbyFlow::ParticleSystemData3::VectorData m_newPositions;
		CubbyFlow::ParticleSystemData3::VectorData m_newVelocities;
		m_newPositions.Resize(n);
		m_newVelocities.Resize(n);
		CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, n, [&](size_t i) {
			// Integrate velocity first
			CubbyFlow::Vector3D& newVelocity = m_newVelocities[i];
			newVelocity = velocities[i] + dt * forces[i] / mass;

			// Integrate position.
			CubbyFlow::Vector3D& newPosition = m_newPositions[i];
			newPosition = positions[i] + dt * newVelocity;
		});
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

		const size_t n = sphdata->InnerPtr->NumberOfParticles();
		CubbyFlow::ArrayView1<CubbyFlow::Vector3D> forces = sphdata->InnerPtr->Forces();
		CubbyFlow::ArrayView1<CubbyFlow::Vector3D> velocities = sphdata->InnerPtr->Velocities();
		CubbyFlow::ArrayView1<CubbyFlow::Vector3D> positions = sphdata->InnerPtr->Positions();
		const double mass = sphdata->InnerPtr->Mass();

		CubbyFlow::ParticleSystemData3::VectorData m_newPositions;
		CubbyFlow::ParticleSystemData3::VectorData m_newVelocities;
		m_newPositions.Resize(n);
		m_newVelocities.Resize(n);
		CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, n, [&](size_t i) {
			// Integrate velocity first
			CubbyFlow::Vector3D& newVelocity = m_newVelocities[i];
			newVelocity = velocities[i] + dt * forces[i] / mass;

			// Integrate position.
			CubbyFlow::Vector3D& newPosition = m_newPositions[i];
			newPosition = positions[i] + dt * newVelocity;
		});
	}

	return true;
}
