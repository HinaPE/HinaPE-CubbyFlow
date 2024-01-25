#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "Particle/SIM_Hina_ParticleFluidData.h"
#include "Particle/GAS_Hina_ConfigureForSPH.h"

#include "Emitter/GAS_Hina_VolumeParticleEmittter.h"

void initializeSIM(void *)
{
	// Particle Data
	IMPLEMENT_DATAFACTORY(SIM_Hina_ParticleFluidData)
	IMPLEMENT_DATAFACTORY(GAS_Hina_ConfigureForSPH)

	// Emitter
//	IMPLEMENT_DATAFACTORY(GAS_Hina_VolumeParticleEmittter)
}
