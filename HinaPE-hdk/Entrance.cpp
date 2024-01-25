#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include <Collider/SIM_Hina_RigidBodyCollider.h>
#include <Emitter/GAS_Hina_VolumeParticleEmittter.h>
#include <Geometry/SIM_Hina_Box.h>
#include <Geometry/SIM_Hina_Plane.h>
#include <Geometry/SIM_Hina_Sphere.h>
#include <Particle/GAS_Hina_ConfigureForSPH.h>
#include <Particle/SIM_Hina_ParticleFluidData.h>

#include <SIM/SIM_DataFactory.h>

void initializeSIM(void *)
{
	// Collider
	IMPLEMENT_DATAFACTORY(SIM_Hina_RigidBodyCollider)

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
