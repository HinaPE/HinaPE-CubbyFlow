#include "GAS_Hina_UpdateDensity.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		UpdateDensity,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_UpdateDensity::_init() {}
void GAS_Hina_UpdateDensity::_makeEqual(const GAS_Hina_UpdateDensity *src) {}
bool GAS_Hina_UpdateDensity::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	data->InnerPtr->UpdateDensities();
	data->sync_density(geo); // sync gdp
	return true;
}
