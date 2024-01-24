#include "GAS_CF_FLIPSolver.h"

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

#include <SIM_CF_FLIPData.h>

#include "Core/Array/ArrayUtils.hpp"

bool GAS_CF_FLIPSolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_FLIPSolver::initializeSubclass()
{
	SIM_Data::initializeSubclass();

	frame = CubbyFlow::Frame(0, 1.0 / 60);
}

void GAS_CF_FLIPSolver::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
	const GAS_CF_FLIPSolver *src = SIM_DATA_CASTCONST(source, GAS_CF_FLIPSolver);

	this->frame = src->frame;
}

const char *GAS_CF_FLIPSolver::DATANAME = "CF_FLIPSolver";
const SIM_DopDescription *GAS_CF_FLIPSolver::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_FLIP_solver",
								   "CF FLIP Solver",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_FLIPSolver::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_FLIPData *flipdata = SIM_DATA_GET(*obj, SIM_CF_FLIPData::DATANAME, SIM_CF_FLIPData);
	if (!flipdata)
	{
		error_msg.appendSprintf("SIM_CF_FLIPData Is Null, From %s\n", DATANAME);
		return false;
	}

	flipdata->SolverPtr->Update(++frame);
	std::cout << frame.index << std::endl;

	SIM_GeometryCopy *geo = SIM_DATA_CREATE(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy,
											SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE);
	if (!geo)
	{
		error_msg.appendSprintf("Geometry Is Null, From %s\n", DATANAME);
		return false;
	}

	const auto particles = flipdata->SolverPtr->GetParticleSystemData();
	CubbyFlow::Array1<CubbyFlow::Vector3D> positions(particles->NumberOfParticles());
	CubbyFlow::Copy(particles->Positions(), positions.View());
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

		int gdp_pt_size = gdp.getNumPoints();
		int flip_pt_size = positions.Size().x;

		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				GA_Index pt_idx = gdp.pointIndex(pt_off);
				auto pos = positions[pt_idx];
				gdp.setPos3(pt_off, UT_Vector3(pos.x, pos.y, pos.z));
			}

		for (int new_pt_idx = gdp_pt_size; new_pt_idx < flip_pt_size; ++new_pt_idx)
		{
			pt_off = gdp.appendPoint();
			auto pos = positions[new_pt_idx];
			gdp.setPos3(pt_off, UT_Vector3(pos.x, pos.y, pos.z));
		}
	}

	return true;
}
