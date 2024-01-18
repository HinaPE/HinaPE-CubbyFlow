#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "Particle/SIM_CF_ParticleSystemData.h"
#include "Particle/SIM_CF_SPHSystemData.h"
#include "Particle/GAS_CF_ConfigureParticleSystemData.h"
#include "Particle/GAS_CF_ConfigureSPHSystemData.h"
#include "Particle/GAS_CF_UpdateToGeometrySheet.h"

void initializeSIM(void *)
{
	IMPLEMENT_DATAFACTORY(SIM_CF_ParticleSystemData);
	IMPLEMENT_DATAFACTORY(SIM_CF_SPHSystemData);
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureParticleSystemData);
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureSPHSystemData);
	IMPLEMENT_DATAFACTORY(GAS_CF_UpdateToGeometrySheet);
}
