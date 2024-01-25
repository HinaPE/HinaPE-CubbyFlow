#include "GAS_Hina_ConfigureForSPH.h"

#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		ConfigureForSPH,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_ConfigureForSPH::_init() {}
void GAS_Hina_ConfigureForSPH::_makeEqual(const GAS_Hina_ConfigureForSPH *src) {}
bool GAS_Hina_ConfigureForSPH::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)

	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		if (!data->Configured)
		{
			data->configure_init(gdp);
			data->Configured = true;
		}
		data->runtime_init_handles(gdp);
	}

	return true;
}
