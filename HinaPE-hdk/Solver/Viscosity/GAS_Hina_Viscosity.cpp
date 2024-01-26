#include "GAS_Hina_Viscosity.h"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		Viscosity,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_Viscosity::_init()
{

}
void GAS_Hina_Viscosity::_makeEqual(const GAS_Hina_Viscosity *src)
{

}
bool GAS_Hina_Viscosity::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	return false;
}
