#include "GAS_Hina_SemiLagrangian.h"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		SemiLagrangian,
		false,
)

void GAS_Hina_SemiLagrangian::_init() {}
void GAS_Hina_SemiLagrangian::_makeEqual(const GAS_Hina_SemiLagrangian *src) {}
bool GAS_Hina_SemiLagrangian::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	return true;
}
