#include "GAS_Hina_CubicSemiLagrangian.h"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		CubicSemiLagrangian,
		false,
)

void GAS_Hina_CubicSemiLagrangian::_init() {}
void GAS_Hina_CubicSemiLagrangian::_makeEqual(const GAS_Hina_CubicSemiLagrangian *src) {}
bool GAS_Hina_CubicSemiLagrangian::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	return true;
}
