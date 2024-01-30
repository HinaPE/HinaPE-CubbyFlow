#include "GAS_GT_PICSolver.h"

#include <Core/Array/ArrayUtils.hpp>
#include <Core/Emitter/ParticleEmitterSet3.hpp>
#include <Core/Emitter/VolumeParticleEmitter3.hpp>
#include <Core/Geometry/Box.hpp>
#include <Core/Geometry/Cylinder3.hpp>
#include <Core/Geometry/ImplicitSurfaceSet.hpp>
#include <Core/Geometry/Plane.hpp>
#include <Core/Geometry/RigidBodyCollider.hpp>
#include <Core/Geometry/Sphere.hpp>
#include <Core/Particle/ParticleSystemData.hpp>
#include <Core/PointGenerator/BccLatticePointGenerator.hpp>
#include <Core/PointGenerator/GridPointGenerator3.hpp>
#include <Core/Solver/Hybrid/APIC/APICSolver3.hpp>
#include <Core/Solver/Hybrid/FLIP/FLIPSolver3.hpp>
#include <Core/Solver/Hybrid/PIC/PICSolver3.hpp>
#include <Core/Utils/Logging.hpp>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		GT_PICSolver,
		false,
		ACTIVATE_GAS_GEOMETRY \
		ACTIVATE_GAS_DENSITY
)

void GAS_Hina_GT_PICSolver::_init()
{
	this->InnerPtr = nullptr;
	frame = CubbyFlow::Frame(0, 1.0 / 60);
}
void GAS_Hina_GT_PICSolver::_makeEqual(const GAS_Hina_GT_PICSolver *src)
{
	this->InnerPtr = src->InnerPtr;
	this->frame = src->frame;
}
bool GAS_Hina_GT_PICSolver::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	return true;
}
