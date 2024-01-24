#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "Collider/GAS_CF_ConfigureRigidBodyCollider.h"
#include <Collider/GAS_CF_UpdateCollider.h>
#include <Collider/SIM_CF_RigidBodyCollider.h>

#include <Emitter/GAS_CF_PointEmitter.h>
#include <Emitter/GAS_CF_VolumeParticleEmitter.h>

#include <Geometry/SIM_CF_Sphere.h>
#include <Geometry/SIM_CF_Box.h>
#include <Geometry/SIM_CF_Plane.h>

#include <Particle/GAS_CF_CommitCache.h>
#include <Particle/GAS_CF_UpdateToGeometrySheet.h>
#include <Particle/ParticleSystemData/GAS_CF_ConfigureParticleSystemData.h>
#include <Particle/ParticleSystemData/SIM_CF_ParticleSystemData.h>
#include <Particle/SPHSystemData/GAS_CF_ConfigureSPHSystemData.h>
#include <Particle/SPHSystemData/SIM_CF_SPHSystemData.h>
#include <Particle/PCISPHSystemData/GAS_CF_ConfigurePCISPHSystemData.h>
#include <Particle/PCISPHSystemData/SIM_CF_PCISPHSystemData.h>


#include <Solver/Advection/GAS_CF_Semi_Implicit_Euler.h>
#include <Solver/Advection/GAS_CF_SemiLagrangianAdvector.h>
#include <Solver/Collision/GAS_CF_CollisionSolver.h>
#include <Solver/Density/GAS_CF_UpdateDensitySolver.h>
#include <Solver/Force/GAS_CF_ClearForce.h>
#include <Solver/Force/GAS_CF_ActivateGravityForce.h>
#include <Solver/Neighbor/GAS_CF_BuildNeighborLists.h>
#include <Solver/Neighbor/GAS_CF_ReadNeighborLists.h>
#include <Solver/Pressure/GAS_CF_PCISPHPressureForceSolver.h>
#include <Solver/Pressure/GAS_CF_PressureForceSolver.h>
#include <Solver/Viscosity/GAS_CF_PseudoViscosityForceSolver.h>
#include <Solver/Viscosity/GAS_CF_ViscosityForceSolver.h>

#include "Utils/SIM_CL_NoLog.h"

#include "HOW_TO_USE_GENERATOR.h"

void initializeSIM(void *)
{
	// Collider
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureRigidBodyCollider)
	IMPLEMENT_DATAFACTORY(GAS_CF_UpdateCollider)
	IMPLEMENT_DATAFACTORY(SIM_CF_RigidBodyCollider)

	// Emitter
	IMPLEMENT_DATAFACTORY(GAS_CF_PointEmitter)
	IMPLEMENT_DATAFACTORY(GAS_CF_VolumeParticleEmitter)

	// Geometry
	IMPLEMENT_DATAFACTORY(SIM_CF_Sphere)
	IMPLEMENT_DATAFACTORY(SIM_CF_Box)
	IMPLEMENT_DATAFACTORY(SIM_CF_Plane)

	// Particle
	IMPLEMENT_DATAFACTORY(GAS_CF_CommitCache)
	IMPLEMENT_DATAFACTORY(GAS_CF_UpdateToGeometrySheet)
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureParticleSystemData)
	IMPLEMENT_DATAFACTORY(SIM_CF_ParticleSystemData)
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureSPHSystemData)
	IMPLEMENT_DATAFACTORY(SIM_CF_SPHSystemData)
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigurePCISPHSystemData)
	IMPLEMENT_DATAFACTORY(SIM_CF_PCISPHSystemData)

	// Solver
	IMPLEMENT_DATAFACTORY(GAS_CF_Semi_Implicit_Euler)
	IMPLEMENT_DATAFACTORY(GAS_CF_SemiLagrangianAdvector)
	IMPLEMENT_DATAFACTORY(GAS_CF_CollisionSolver)
	IMPLEMENT_DATAFACTORY(GAS_CF_UpdateDensitySolver)
	IMPLEMENT_DATAFACTORY(GAS_CF_ClearForce)
	IMPLEMENT_DATAFACTORY(GAS_CF_ActivateGravityForce)
	IMPLEMENT_DATAFACTORY(GAS_CF_BuildNeighborLists)
	IMPLEMENT_DATAFACTORY(GAS_CF_ReadNeighborLists);
	IMPLEMENT_DATAFACTORY(GAS_CF_PCISPHPressureForceSolver)
	IMPLEMENT_DATAFACTORY(GAS_CF_PressureForceSolver)
	IMPLEMENT_DATAFACTORY(GAS_CF_PseudoViscosityForceSolver)
	IMPLEMENT_DATAFACTORY(GAS_CF_ViscosityForceSolver)

	// Utils
	IMPLEMENT_DATAFACTORY(SIM_CL_NoLog)

	// JEST FOR TEST
	IMPLEMENT_DATAFACTORY(GAS_CF_NEW_SOLVER)
	IMPLEMENT_DATAFACTORY(SIM_CF_NEW_DATA)
}
