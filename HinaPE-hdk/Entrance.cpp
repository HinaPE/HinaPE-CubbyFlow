#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "Particle/SIM_Hina_ParticleFluidData.h"

#include "Emitter/GAS_Hina_VolumeParticleEmittter.h"

void initializeSIM(void *)
{
	// Particle Data
	IMPLEMENT_DATAFACTORY(SIM_Hina_ParticleFluidData)

	// Emitter
	IMPLEMENT_DATAFACTORY(GAS_Hina_VolumeParticleEmittter)
}
