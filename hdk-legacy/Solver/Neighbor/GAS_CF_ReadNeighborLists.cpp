#include "GAS_CF_ReadNeighborLists.h"

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

#include <Particle/ParticleSystemData/SIM_CF_ParticleSystemData.h>
#include <Particle/SPHSystemData/SIM_CF_SPHSystemData.h>

bool GAS_CF_ReadNeighborLists::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_ReadNeighborLists::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_ReadNeighborLists::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_ReadNeighborLists::DATANAME = "CF_ReadNeighborLists";
const SIM_DopDescription *GAS_CF_ReadNeighborLists::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_read_neighbor_lists",
								   "CF Read Neighbor Lists",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_ReadNeighborLists::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
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

		// TODO: consider whether need to enable particle system data to support neighbor lists
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

		int p_size = sphdata->InnerPtr->NumberOfParticles();

		// First we should build Searcher as normal.
		sphdata->InnerPtr->BuildNeighborSearcher();

		// Then, directly read in neighbor lists without calling [sphdata->InnerPtr->BuildNeighborLists()]
		// Drop "const" modifier (though it is dangerous, we can do this in current situation)
		auto &lists = const_cast<CubbyFlow::Array1<CubbyFlow::Array1<size_t>> &>(sphdata->InnerPtr->NeighborLists());
		lists.Resize(p_size);

		{
			SIM_GeometryAutoWriteLock lock(geo);
			GU_Detail &gdp = lock.getGdp();
			GA_RWHandleI gdp_handle_neighbor_sum = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEIGHBOR_SUM_ATTRIBUTE_NAME);
			GA_ROHandleIA gdp_handle_neighbor_list = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEIGHBOR_LIST_ATTRIBUTE_NAME);
			GA_ROHandleI gdp_handle_CL_PT_IDX = gdp.findPointAttribute(SIM_CF_SPHSystemData::CL_PT_IDX_ATTRIBUTE_NAME);

			GA_Offset pt_off;
			GA_FOR_ALL_PTOFF(&gdp, pt_off)
				{
					int cl_index = gdp_handle_CL_PT_IDX.get(pt_off);

					UT_Int32Array nArray;
					gdp_handle_neighbor_list.get(pt_off, nArray);
					int n_size = nArray.size();
					lists[cl_index].Resize(n_size);

					for (int i = 0; i < n_size; ++i)
						lists[cl_index][i] = nArray[i];

					gdp_handle_neighbor_sum.set(pt_off, n_size);
				}
		}
	}

	return true;
}
