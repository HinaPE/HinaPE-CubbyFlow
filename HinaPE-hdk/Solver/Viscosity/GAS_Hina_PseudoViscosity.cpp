#include "GAS_Hina_PseudoViscosity.h"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		PseudoViscosity,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_PseudoViscosity::_init()
{

}
void GAS_Hina_PseudoViscosity::_makeEqual(const GAS_Hina_PseudoViscosity *src)
{

}
bool GAS_Hina_PseudoViscosity::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	return true;
}
