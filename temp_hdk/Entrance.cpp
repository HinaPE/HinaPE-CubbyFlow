#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include <GAS_CF_ConfigureFLIPData.h>
#include <GAS_CF_FLIPSolver.h>
#include <SIM_CF_FLIPData.h>

void initializeSIM(void *)
{
	// FLIP
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureFLIPData)
	IMPLEMENT_DATAFACTORY(GAS_CF_FLIPSolver)
	IMPLEMENT_DATAFACTORY(SIM_CF_FLIPData)
}
