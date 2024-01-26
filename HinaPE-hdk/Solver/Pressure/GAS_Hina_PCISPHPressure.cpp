#include "GAS_Hina_PCISPHPressure.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		PCISPHPressure,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_PCISPHPressure::_init() {}
void GAS_Hina_PCISPHPressure::_makeEqual(const GAS_Hina_PCISPHPressure *src) {}
bool GAS_Hina_PCISPHPressure::_solve(SIM_Engine &, SIM_Object *obj, SIM_Time, SIM_Time)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	return true;
}
