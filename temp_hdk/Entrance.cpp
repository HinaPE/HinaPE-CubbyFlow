#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include <FLIP/GAS_CF_ConfigureFLIPData.h>
#include <FLIP/GAS_CF_FLIPSolver.h>
#include <FLIP/SIM_CF_FLIPData.h>
#include <VDB/GAS_VDB_TEST.h>

void initializeSIM(void *)
{
	// FLIP
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureFLIPData)
	IMPLEMENT_DATAFACTORY(GAS_CF_FLIPSolver)
	IMPLEMENT_DATAFACTORY(SIM_CF_FLIPData)

	// VDB
	IMPLEMENT_DATAFACTORY(GAS_VDB_TEST)
}
