#include "GAS_Hina_ConfigureForSPH.h"

#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		ConfigureForSPH,
		false,
)

void GAS_Hina_ConfigureForSPH::_init() {}
void GAS_Hina_ConfigureForSPH::_makeEqual(const GAS_Hina_ConfigureForSPH *src) {}
bool GAS_Hina_ConfigureForSPH::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)

	if (data->Configured)
		return true;

	SIM_GeometryCopy *geo = SIM_DATA_CREATE(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy,
											SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE);
	CHECK_NULL(geo)

	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		data->configure_init(gdp);
	}

	return true;
}
