#include "GAS_Hina_CollisionSolve.h"

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		CollisionSolve,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_CollisionSolve::_init()
{

}
void GAS_Hina_CollisionSolve::_makeEqual(const GAS_Hina_CollisionSolve *src)
{

}
bool GAS_Hina_CollisionSolve::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	return false;
}
