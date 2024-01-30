#include "GAS_Hina_ReadNeighbor.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		ReadNeighbor,
		false,
		ACTIVATE_GAS_GEOMETRY \
)

void GAS_Hina_ReadNeighbor::_init() {}
void GAS_Hina_ReadNeighbor::_makeEqual(const GAS_Hina_ReadNeighbor *src) {}
bool GAS_Hina_ReadNeighbor::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	// First we should build Searcher as normal.
	data->InnerPtr->BuildNeighborSearcher();

	// Then, directly read in neighbor lists without calling [sphdata->InnerPtr->BuildNeighborLists()]
	// Drop "const" modifier (though it is dangerous, we can do this in current situation)
	int p_size = data->InnerPtr->NumberOfParticles();
	auto &lists = const_cast<CubbyFlow::Array1<CubbyFlow::Array1<size_t>> &>(data->InnerPtr->NeighborLists());
	lists.Resize(p_size);

	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		data->runtime_init_handles(gdp);

		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				size_t cl_index = data->gdp_index(pt_off);

				UT_Int32Array nArray = data->gdp_neighbors(cl_index);
				int n_size = nArray.size();
				lists[cl_index].Resize(n_size);
				for (int i = 0; i < n_size; ++i)
					lists[cl_index][i] = nArray[i];
				data->set_gdp_neighbor_sum(cl_index, n_size);
			}
	}

	return true;
}
