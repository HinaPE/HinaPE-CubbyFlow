#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "Collider/GAS_CL_UpdateCollider.h"
#include "Collider/SIM_CF_RigidBodyCollider.h"

#include "Emitter/GAS_CF_PointEmitter.h"
#include "Emitter/GAS_CF_VolumeParticleEmitter.h"

#include "Geometry/SIM_CF_Sphere.h"
#include "Geometry/SIM_CF_Box.h"
#include "Geometry/SIM_CF_Plane.h"

#include "Particle/GAS_CF_ConfigureParticleSystemData.h"
#include "Particle/GAS_CF_ConfigureSPHSystemData.h"
#include "Particle/GAS_CF_UpdateToGeometrySheet.h"
#include "Particle/SIM_CF_ParticleSystemData.h"
#include "Particle/SIM_CF_SPHSystemData.h"

#include "Solver/Advection/GAS_CF_Semi_Implicit_Euler.h"
#include "Solver/Advection/GAS_CF_SemiLagrangianAdvector.h"
#include "Solver/Collision/GAS_CF_CollisionSolver.h"
#include "Solver/Density/GAS_CF_UpdateDensitySolver.h"
#include "Solver/Force/GAS_CF_ClearForce.h"
#include "Solver/Force/GAS_CF_ActivateGravityForce.h"
#include "Solver/Neighbor/GAS_CF_BuildNeighborLists.h"
#include "Solver/Viscosity/GAS_CF_ViscosityForceSolver.h"


void initializeSIM(void *)
{
	// Collider
	IMPLEMENT_DATAFACTORY(GAS_CL_UpdateCollider)
	IMPLEMENT_DATAFACTORY(SIM_CF_RigidBodyCollider)

	// Emitter
	IMPLEMENT_DATAFACTORY(GAS_CF_PointEmitter)
	IMPLEMENT_DATAFACTORY(GAS_CF_VolumeParticleEmitter)

	// Geometry
	IMPLEMENT_DATAFACTORY(SIM_CF_Sphere)
	IMPLEMENT_DATAFACTORY(SIM_CF_Box)
	IMPLEMENT_DATAFACTORY(SIM_CF_Plane)

	// Particle
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureParticleSystemData)
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureSPHSystemData)
	IMPLEMENT_DATAFACTORY(GAS_CF_UpdateToGeometrySheet)
	IMPLEMENT_DATAFACTORY(SIM_CF_ParticleSystemData)
	IMPLEMENT_DATAFACTORY(SIM_CF_SPHSystemData)

	// Solver
	IMPLEMENT_DATAFACTORY(GAS_CF_Semi_Implicit_Euler)
	IMPLEMENT_DATAFACTORY(GAS_CF_SemiLagrangianAdvector)
	IMPLEMENT_DATAFACTORY(GAS_CF_CollisionSolver)
	IMPLEMENT_DATAFACTORY(GAS_CF_UpdateDensitySolver)
	IMPLEMENT_DATAFACTORY(GAS_CF_ClearForce)
	IMPLEMENT_DATAFACTORY(GAS_CF_ActivateGravityForce)
	IMPLEMENT_DATAFACTORY(GAS_CF_BuildNeighborLists)
	IMPLEMENT_DATAFACTORY(GAS_CF_ViscosityForceSolver)
}
