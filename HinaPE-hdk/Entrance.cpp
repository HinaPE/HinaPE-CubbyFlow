#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include <Collider/SIM_Hina_RigidBodyCollider.h>
#include <Emitter/GAS_Hina_VolumeGridEmitter.h>
#include <Emitter/GAS_Hina_VolumeParticleEmittter.h>
#include <Geometry/SIM_Hina_Box.h>
#include <Geometry/SIM_Hina_Plane.h>
#include <Geometry/SIM_Hina_Sphere.h>
#include <Grid/SIM_Hina_ConfigureForSmoke.h>
#include <Grid/SIM_Hina_GridFluidData.h>
#include <Particle/GAS_Hina_CommitAllCaches.h>
#include <Particle/SIM_Hina_ConfigureForSPH.h>
#include <Particle/SIM_Hina_ParticleFluidData.h>
#include <Particle/SIM_Hina_PCISPHCaches.h>
#include <Particle/SIM_Hina_SPHCaches.h>
#include <Solver/Euler/Common/Advection/GAS_Hina_CubicSemiLagrangian.h>
#include <Solver/Euler/Common/Advection/GAS_Hina_SemiLagrangian.h>
#include <Solver/Lagrangian/Common/Collision/GAS_Hina_CollisionSolve.h>
#include <Solver/Lagrangian/Common/Density/GAS_Hina_UpdateDensity.h>
#include <Solver/Lagrangian/Common/Force/GAS_Hina_ClearForce.h>
#include <Solver/Lagrangian/Common/Force/GAS_Hina_GravityForce.h>
#include <Solver/Lagrangian/Common/Neighbor/GAS_Hina_ReadNeighbor.h>
#include <Solver/Lagrangian/Common/Neighbor/GAS_Hina_UpdateNeighbor.h>
#include <Solver/Lagrangian/Common/Pressure/GAS_Hina_PCISPHPressure.h>
#include <Solver/Lagrangian/Common/Pressure/GAS_Hina_SPHPressure.h>
#include <Solver/Lagrangian/Common/TimeIntegrate/GAS_Hina_SemiImplicitEuler.h>
#include <Solver/Lagrangian/Common/TimeIntegrate/GAS_Hina_SubStep.h>
#include <Solver/Lagrangian/Common/Viscosity/GAS_Hina_PseudoViscosity.h>
#include <Solver/Lagrangian/Common/Viscosity/GAS_Hina_Viscosity.h>
#include <Solver/Lagrangian/DFSPH/GAS_Hina_DFSPHSolver.h>

// Just for test
#include <_GroundTruth/PCISPH/GAS_GT_PCISPHSolver.h>
#include <_GroundTruth/PIC/GAS_GT_PICSolver.h>
#include <_GroundTruth/SMOKE/GAS_GT_SmokeSolver.h>
#include <_GroundTruth/SPH/GAS_GT_SPHSolver.h>

void initializeSIM(void *)
{
	// Collider
	IMPLEMENT_DATAFACTORY(SIM_Hina_RigidBodyCollider)

	// Emitter
	IMPLEMENT_DATAFACTORY(GAS_Hina_VolumeGridEmitter)
	IMPLEMENT_DATAFACTORY(GAS_Hina_VolumeParticleEmittter)

	// Geometry
	IMPLEMENT_DATAFACTORY(SIM_Hina_Box)
	IMPLEMENT_DATAFACTORY(SIM_Hina_Plane)
	IMPLEMENT_DATAFACTORY(SIM_Hina_Sphere)

	// Grid
	IMPLEMENT_DATAFACTORY(SIM_Hina_ConfigureForSmoke)
	IMPLEMENT_DATAFACTORY(SIM_Hina_GridFluidData)

	// Particle Data
	IMPLEMENT_DATAFACTORY(GAS_Hina_CommitAllCaches)
	IMPLEMENT_DATAFACTORY(SIM_Hina_ConfigureForSPH)
	IMPLEMENT_DATAFACTORY(SIM_Hina_ParticleFluidData)
	IMPLEMENT_DATAFACTORY(SIM_Hina_PCISPHCaches)
	IMPLEMENT_DATAFACTORY(SIM_Hina_SPHCaches)

	// Solver
	IMPLEMENT_DATAFACTORY(GAS_Hina_CubicSemiLagrangian)
	IMPLEMENT_DATAFACTORY(GAS_Hina_SemiLagrangian)
	IMPLEMENT_DATAFACTORY(GAS_Hina_CollisionSolve)
	IMPLEMENT_DATAFACTORY(GAS_Hina_UpdateDensity)
	IMPLEMENT_DATAFACTORY(GAS_Hina_ClearForce)
	IMPLEMENT_DATAFACTORY(GAS_Hina_GravityForce)
	IMPLEMENT_DATAFACTORY(GAS_Hina_ReadNeighbor)
	IMPLEMENT_DATAFACTORY(GAS_Hina_UpdateNeighbor)
	IMPLEMENT_DATAFACTORY(GAS_Hina_PCISPHPressure)
	IMPLEMENT_DATAFACTORY(GAS_Hina_SPHPressure)
	IMPLEMENT_DATAFACTORY(GAS_Hina_SemiImplicitEuler)
	IMPLEMENT_DATAFACTORY(GAS_Hina_SubStep)
	IMPLEMENT_DATAFACTORY(GAS_Hina_PseudoViscosity)
	IMPLEMENT_DATAFACTORY(GAS_Hina_Viscosity)
	IMPLEMENT_DATAFACTORY(GAS_Hina_DFSPHSolver)

	// _GroundTruth
	IMPLEMENT_DATAFACTORY(GAS_Hina_GT_PICSolver)
	IMPLEMENT_DATAFACTORY(GAS_Hina_GT_PCISPHSolver)
	IMPLEMENT_DATAFACTORY(GAS_Hina_GT_SmokeSolver)
	IMPLEMENT_DATAFACTORY(GAS_Hina_GT_SPHSolver)
}
