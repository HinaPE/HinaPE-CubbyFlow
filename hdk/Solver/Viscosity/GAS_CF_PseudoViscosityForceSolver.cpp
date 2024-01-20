#include "GAS_CF_PseudoViscosityForceSolver.h"

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

#include "Core/Particle/SPHKernels.hpp"

bool GAS_CF_PseudoViscosityForceSolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_PseudoViscosityForceSolver::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_PseudoViscosityForceSolver::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_PseudoViscosityForceSolver::DATANAME = "CF_PseudoViscosityForceSolver";
const SIM_DopDescription *GAS_CF_PseudoViscosityForceSolver::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_pseudo_viscosity_force_solver",
								   "CF Pseudo Viscosity Force Solver",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_PseudoViscosityForceSolver::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
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

	// Add Pseudo Viscosity Force in CubbyFlow
	{
		using namespace CubbyFlow;
		double m_pseudoViscosityCoefficient = sphdata->getPseudoViscosityCoefficient();

		SPHSystemData3Ptr particles = sphdata->InnerPtr;
		const size_t numberOfParticles = particles->NumberOfParticles();
		ArrayView1<Vector3D> x = particles->Positions();
		ArrayView1<Vector3D> v = particles->Velocities();
		ArrayView1<double> d = particles->Densities();

		const double mass = particles->Mass();
		const SPHSpikyKernel3 kernel{ particles->KernelRadius() };

		Array1<Vector3D> smoothedVelocities{ numberOfParticles };

		ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i) {
			double weightSum = 0.0;
			Vector3D smoothedVelocity;

			const auto& neighbors = particles->NeighborLists()[i];
			for (size_t j : neighbors)
			{
				const double dist = x[i].DistanceTo(x[j]);
				const double wj = mass / d[j] * kernel(dist);
				weightSum += wj;
				smoothedVelocity += wj * v[j];
			}

			const double wi = mass / d[i];
			weightSum += wi;
			smoothedVelocity += wi * v[i];

			if (weightSum > 0.0)
			{
				smoothedVelocity /= weightSum;
			}

			smoothedVelocities[i] = smoothedVelocity;
		});

		double factor = timestep * m_pseudoViscosityCoefficient;
		factor = std::clamp(factor, 0.0, 1.0);

		ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i) {
			v[i] = Lerp(v[i], smoothedVelocities[i], factor);
		});
	}

	// Sync Geometry Sheet
	CubbyFlow::ArrayView1<CubbyFlow::Vector3D> velocities = sphdata->InnerPtr->Velocities();
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

		GA_RWHandleV3 gdp_handle_vel = gdp.findPointAttribute(gdp.getStdAttributeName(GEO_ATTRIBUTE_VELOCITY));
		GA_RWHandleI gdp_handle_CL_PT_IDX = gdp.findPointAttribute(SIM_CF_SPHSystemData::CL_PT_IDX_ATTRIBUTE_NAME);

		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				int cl_index = gdp_handle_CL_PT_IDX.get(pt_off);
				UT_Vector3 velocity = UT_Vector3D{velocities[cl_index].x, velocities[cl_index].y, velocities[cl_index].z};
				gdp_handle_vel.set(pt_off, velocity);
			}
	}

	return true;
}
