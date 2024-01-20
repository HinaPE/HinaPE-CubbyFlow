#include "GAS_CF_ViscosityForceSolver.h"

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

bool GAS_CF_ViscosityForceSolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_ViscosityForceSolver::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_ViscosityForceSolver::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_ViscosityForceSolver::DATANAME = "CF_ViscosityForceSolver";
const SIM_DopDescription *GAS_CF_ViscosityForceSolver::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_viscosity_force_solver",
								   "CF Viscosity Force Solver",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_ViscosityForceSolver::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
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

	// Add Viscosity Force in CubbyFlow
	size_t p_size = sphdata->InnerPtr->NumberOfParticles();
	std::vector<CubbyFlow::Vector3D> viscosity_cache;
	viscosity_cache.resize(p_size);

	CubbyFlow::ArrayView1<CubbyFlow::Vector3D> x = sphdata->InnerPtr->Positions();
	CubbyFlow::ArrayView1<CubbyFlow::Vector3D> v = sphdata->InnerPtr->Velocities();
	CubbyFlow::ArrayView1<double> d = sphdata->InnerPtr->Densities();
	CubbyFlow::ArrayView1<CubbyFlow::Vector3D> f = sphdata->InnerPtr->Forces();

	const double viscosity = sphdata->getViscosityCoefficient();
	const double mass_squared = sphdata->InnerPtr->Mass() * sphdata->InnerPtr->Mass();
	const CubbyFlow::SPHSpikyKernel3 kernel{sphdata->InnerPtr->KernelRadius()};

	CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, p_size, [&](size_t i)
	{
		const auto &neighbors = sphdata->InnerPtr->NeighborLists()[i];
		for (size_t j: neighbors)
		{
			const double dist = x[i].DistanceTo(x[j]);

			viscosity_cache[i] = viscosity * mass_squared * (v[j] - v[i]) /
								 d[j] * kernel.SecondDerivative(dist);
			f[i] += viscosity_cache[i];
		}
	});


	// Sync Geometry Sheet
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

		GA_RWHandleV3 gdp_handle_force = gdp.findPointAttribute(SIM_CF_SPHSystemData::FORCE_ATTRIBUTE_NAME);
		GA_RWHandleI gdp_handle_CL_PT_IDX = gdp.findPointAttribute(SIM_CF_SPHSystemData::CL_PT_IDX_ATTRIBUTE_NAME);

		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				UT_Vector3 exist_force = gdp_handle_force.get(pt_off);
				int cl_index = gdp_handle_CL_PT_IDX.get(pt_off);
				UT_Vector3 viscosity_force = UT_Vector3D{viscosity_cache[cl_index].x, viscosity_cache[cl_index].y, viscosity_cache[cl_index].z};
				gdp_handle_force.set(pt_off, viscosity_force + exist_force);
			}
	}

	return true;
}
