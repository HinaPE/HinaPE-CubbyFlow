#include "GAS_Hina_ComputeSubsteps.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		ComputeSubsteps,
		false,
)

void GAS_Hina_ComputeSubsteps::_init() {}
void GAS_Hina_ComputeSubsteps::_makeEqual(const GAS_Hina_ComputeSubsteps *src) {}
bool GAS_Hina_ComputeSubsteps::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)

	return true;
}
