#include "GAS_CF_PressureForceSolver.h"

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

#include "Core/Particle/SPHKernels.hpp"
#include "Core/Utils/PhysicsHelpers.hpp"

bool GAS_CF_PressureForceSolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_PressureForceSolver::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_PressureForceSolver::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_PressureForceSolver::DATANAME = "CF_PressureForceSolver";
const SIM_DopDescription *GAS_CF_PressureForceSolver::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_pressure_force_solver",
								   "CF Pressure Force Solver",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_PressureForceSolver::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
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

	// Add Pressure Force in CubbyFlow
	size_t p_size = sphdata->InnerPtr->NumberOfParticles();
	std::vector<CubbyFlow::Vector3D> pressure_cache;
	pressure_cache.resize(p_size);

	{
		// ComputePressure
		double m_speedOfSound = sphdata->getSpeedOfSound();
		double m_eosExponent = sphdata->getEosExponent();
		double m_negativePressureScale = sphdata->getNegativePressureScale();

		using namespace CubbyFlow;
		auto &particles = sphdata->InnerPtr;
		const size_t numberOfParticles = particles->NumberOfParticles();
		ArrayView1<double> d = particles->Densities();
		ArrayView1<double> p = particles->Pressures();

		// See Murnaghan-Tait equation of state from
		// https://en.wikipedia.org/wiki/Tait_equation
		const double targetDensity = particles->TargetDensity();
		const double eosScale = targetDensity * Square(m_speedOfSound);

		ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i)
		{
			p[i] = ComputePressureFromEos(d[i], targetDensity, eosScale,
										  m_eosExponent,
										  m_negativePressureScale);
		});
	}

	{
		// AccumulatePressureForce
		using namespace CubbyFlow;
		auto &particles = sphdata->InnerPtr;
		const ArrayView1<Vector3D> positions = particles->Positions();
		const ArrayView1<double> densities = particles->Densities();
		const ArrayView1<double> pressures = particles->Pressures();
		ArrayView1<Vector3D> pressureForces = particles->Forces();

		const size_t numberOfParticles = particles->NumberOfParticles();

		const double massSquared = Square(particles->Mass());
		const SPHSpikyKernel3 kernel{particles->KernelRadius()};

		ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i)
		{
			const auto &neighbors = particles->NeighborLists()[i];
			for (size_t j: neighbors)
			{
				const double dist = positions[i].DistanceTo(positions[j]);
				if (dist > 0.0)
				{
					Vector3D dir = (positions[j] - positions[i]) / dist;
					pressure_cache[i] = -massSquared *
										(pressures[i] / (densities[i] * densities[i]) +
										 pressures[j] / (densities[j] * densities[j])) *
										kernel.Gradient(dist, dir);
					pressureForces[i] += pressure_cache[i];
				}
			}
		});
	}


	const CubbyFlow::ArrayView1<double> pressures = sphdata->InnerPtr->Pressures();
	// Add Pressure Force in Geometry Sheet
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

		GA_RWHandleV3 gdp_handle_force = gdp.findPointAttribute(SIM_CF_SPHSystemData::FORCE_ATTRIBUTE_NAME);
		GA_RWHandleI gdp_handle_CL_PT_IDX = gdp.findPointAttribute(SIM_CF_SPHSystemData::CL_PT_IDX_ATTRIBUTE_NAME);
		GA_RWHandleF gdp_handle_pressure = gdp.findPointAttribute(SIM_CF_SPHSystemData::PRESSURE_ATTRIBUTE_NAME);

		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				UT_Vector3 exist_force = gdp_handle_force.get(pt_off);
				int cl_index = gdp_handle_CL_PT_IDX.get(pt_off);
				UT_Vector3 pressure_force = UT_Vector3D{pressure_cache[cl_index].x, pressure_cache[cl_index].y, pressure_cache[cl_index].z};
				gdp_handle_force.set(pt_off, pressure_force + exist_force);
				auto pressure = pressures[cl_index];
				gdp_handle_pressure.set(pt_off, pressure);
			}
	}

	return true;
}
