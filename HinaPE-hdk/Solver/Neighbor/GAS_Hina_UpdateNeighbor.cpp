#include "GAS_Hina_UpdateNeighbor.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		UpdateNeighbor,
		false,
		ACTIVATE_GAS_GEOMETRY \
)

void GAS_Hina_UpdateNeighbor::_init() {}
void GAS_Hina_UpdateNeighbor::_makeEqual(const GAS_Hina_UpdateNeighbor *src) {}
bool GAS_Hina_UpdateNeighbor::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	data->InnerPtr->BuildNeighborSearcher();
	data->InnerPtr->BuildNeighborLists();
	data->sync_neighbors(geo); // sync gdp

	return true;
}
