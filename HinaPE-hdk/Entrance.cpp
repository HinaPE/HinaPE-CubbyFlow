#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include <Collider/SIM_Hina_RigidBodyCollider.h>
#include <Emitter/GAS_Hina_VolumeParticleEmittter.h>
#include <Geometry/SIM_Hina_Box.h>
#include <Geometry/SIM_Hina_Plane.h>
#include <Geometry/SIM_Hina_Sphere.h>
#include <Particle/GAS_Hina_ConfigureForSPH.h>
#include <Particle/SIM_Hina_ParticleFluidData.h>
#include <Solver/Collision/GAS_Hina_CollisionSolve.h>
#include <Solver/Density/GAS_Hina_UpdateDensity.h>
#include <Solver/Force/GAS_Hina_ClearForce.h>
#include <Solver/Force/GAS_Hina_GravityForce.h>
#include <Solver/Neighbor/GAS_Hina_UpdateNeighbor.h>
#include <Solver/Pressure/GAS_Hina_PCISPHPressure.h>
#include <Solver/Pressure/GAS_Hina_SPHPressure.h>
#include <Solver/TimeIntegrate/GAS_Hina_SemiImplicitEuler.h>

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

	// Solver
	IMPLEMENT_DATAFACTORY(GAS_Hina_CollisionSolve)
	IMPLEMENT_DATAFACTORY(GAS_Hina_UpdateDensity)
	IMPLEMENT_DATAFACTORY(GAS_Hina_ClearForce)
	IMPLEMENT_DATAFACTORY(GAS_Hina_GravityForce)
	IMPLEMENT_DATAFACTORY(GAS_Hina_UpdateNeighbor)
	IMPLEMENT_DATAFACTORY(GAS_Hina_PCISPHPressure)
	IMPLEMENT_DATAFACTORY(GAS_Hina_SPHPressure)
	IMPLEMENT_DATAFACTORY(GAS_Hina_SemiImplicitEuler)
}
