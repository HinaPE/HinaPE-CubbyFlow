#include "GAS_Hina_DFSPHSolver.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		DFSPHSolver,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_DFSPHSolver::_init()
{

}
void GAS_Hina_DFSPHSolver::_makeEqual(const GAS_Hina_DFSPHSolver *src)
{

}
bool GAS_Hina_DFSPHSolver::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	return true;
}
