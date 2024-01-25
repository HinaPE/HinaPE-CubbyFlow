#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include <Emitter/GAS_Hina_VolumeParticleEmittter.h>
#include <Geometry/SIM_Hina_Box.h>
#include <Geometry/SIM_Hina_Plane.h>
#include <Geometry/SIM_Hina_Sphere.h>
#include <Particle/GAS_Hina_ConfigureForSPH.h>
#include <Particle/SIM_Hina_ParticleFluidData.h>

void initializeSIM(void *)
{
	// Emitter
	IMPLEMENT_DATAFACTORY(GAS_Hina_VolumeParticleEmittter)

	// Geometry
	IMPLEMENT_DATAFACTORY(SIM_Hina_Box)
	IMPLEMENT_DATAFACTORY(SIM_Hina_Plane)
	IMPLEMENT_DATAFACTORY(SIM_Hina_Sphere)

	// Particle Data
	IMPLEMENT_DATAFACTORY(GAS_Hina_ConfigureForSPH)
	IMPLEMENT_DATAFACTORY(SIM_Hina_ParticleFluidData)
}
