#include "GAS_Hina_CommitAllCaches.h"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		CommitAllCaches,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_CommitAllCaches::_init() {}
void GAS_Hina_CommitAllCaches::_makeEqual(const GAS_Hina_CommitAllCaches *src) {}
bool GAS_Hina_CommitAllCaches::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	return true;
}
