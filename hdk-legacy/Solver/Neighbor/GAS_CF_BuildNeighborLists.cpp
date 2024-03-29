#include "GAS_CF_BuildNeighborLists.h"

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

bool GAS_CF_BuildNeighborLists::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_BuildNeighborLists::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_BuildNeighborLists::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_BuildNeighborLists::DATANAME = "CF_BuildNeighborLists";
const SIM_DopDescription *GAS_CF_BuildNeighborLists::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_build_neighbor_lists",
								   "CF Build Neighbor Lists",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

/**
 * Now, We Only BuildNeighborLists for
 * - SIM_CF_SPHSystemData
 */
bool GAS_CF_BuildNeighborLists::Solve(SIM_Engine &, SIM_Object *obj, SIM_Time, SIM_Time, UT_WorkBuffer &error_msg) const
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
//		psdata->InnerPtr->BuildNeighborSearcher();
//		psdata->InnerPtr->BuildNeighborLists();
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

		sphdata->InnerPtr->BuildNeighborSearcher();
		sphdata->InnerPtr->BuildNeighborLists();

		// Update Neighbor Sum To Geometry Sheet
		const auto &cf_array_neighbor_list = sphdata->InnerPtr->NeighborLists();
		size_t p_size = sphdata->InnerPtr->NumberOfParticles();
		if (p_size != cf_array_neighbor_list.Size().x)
		{
			error_msg.appendSprintf("Error Array Size::cf_array_neighbor_list, From %s\n", DATANAME);
			return false;
		}

		{
			SIM_GeometryAutoWriteLock lock(geo);
			GU_Detail &gdp = lock.getGdp();
			GA_RWHandleI gdp_handle_neighbor_sum = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEIGHBOR_SUM_ATTRIBUTE_NAME);
			GA_RWHandleIA gdp_handle_neighbor_list = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEIGHBOR_LIST_ATTRIBUTE_NAME);
			GA_ROHandleI gdp_handle_CL_PT_IDX = gdp.findPointAttribute(SIM_CF_SPHSystemData::CL_PT_IDX_ATTRIBUTE_NAME);

			GA_Offset pt_off;
			GA_FOR_ALL_PTOFF(&gdp, pt_off)
				{
					int cl_index = gdp_handle_CL_PT_IDX.get(pt_off);
					const auto &neighbor_list = cf_array_neighbor_list[cl_index];
					gdp_handle_neighbor_sum.set(pt_off, neighbor_list.Size().x);

					UT_Int32Array nArray;
					nArray.setSize(neighbor_list.Size().x);
					for (int nidx = 0; nidx < neighbor_list.Size().x; ++nidx)
						nArray[nidx] = neighbor_list[nidx]; // TODO: this is index, not offset
					gdp_handle_neighbor_list.set(pt_off, nArray);
				}
		}
	}

	return true;
}
