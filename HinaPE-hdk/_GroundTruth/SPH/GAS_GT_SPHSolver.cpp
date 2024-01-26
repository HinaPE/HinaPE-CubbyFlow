#include "GAS_GT_SPHSolver.h"

#include <Core/Array/ArrayUtils.hpp>
#include <Core/Emitter/VolumeParticleEmitter3.hpp>
#include <Core/Geometry/Box.hpp>
#include <Core/Geometry/Cylinder3.hpp>
#include <Core/Geometry/ImplicitSurfaceSet.hpp>
#include <Core/Geometry/Plane.hpp>
#include <Core/Geometry/RigidBodyCollider.hpp>
#include <Core/Geometry/Sphere.hpp>
#include <Core/Particle/ParticleSystemData.hpp>
#include <Core/Solver/Particle/PCISPH/PCISPHSolver3.hpp>
#include <Core/Solver/Particle/SPH/SPHSolver3.hpp>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		GT_SPHSolver,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_GT_SPHSolver::_init()
{
	this->InnerPtr = nullptr;
	frame = CubbyFlow::Frame(0, 1.0 / 60);
}
void GAS_Hina_GT_SPHSolver::_makeEqual(const GAS_Hina_GT_SPHSolver *src)
{
	this->InnerPtr = src->InnerPtr;
}
bool GAS_Hina_GT_SPHSolver::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	using namespace CubbyFlow;
	if (!this->InnerPtr)
	{
		double targetSpacing = 0.02;

		BoundingBox3D domain(Vector3D(), Vector3D(1, 2, 1));

		auto solver = SPHSolver3::GetBuilder()
				.WithTargetDensity(1000.0)
				.WithTargetSpacing(targetSpacing)
				.MakeShared();

		solver->SetPseudoViscosityCoefficient(0.0);

		// Build emitter
		BoundingBox3D sourceBound(domain);
		sourceBound.Expand(-targetSpacing);

		const auto plane = Plane3::GetBuilder()
				.WithNormal({ 0, 1, 0 })
				.WithPoint({ 0, 0.25 * domain.Height(), 0 })
				.MakeShared();

		const auto sphere = Sphere3::GetBuilder()
				.WithCenter(domain.MidPoint())
				.WithRadius(0.15 * domain.Width())
				.MakeShared();

		const auto surfaceSet =
				ImplicitSurfaceSet3::GetBuilder()
						.WithExplicitSurfaces(Array1<Surface3Ptr>{ plane, sphere })
						.MakeShared();

		const auto emitter = VolumeParticleEmitter3::GetBuilder()
				.WithImplicitSurface(surfaceSet)
				.WithSpacing(targetSpacing)
				.WithMaxRegion(sourceBound)
				.WithIsOneShot(true)
				.MakeShared();

		solver->SetEmitter(emitter);

		// Build collider
		const auto box = Box3::GetBuilder()
				.WithIsNormalFlipped(true)
				.WithBoundingBox(domain)
				.MakeShared();

		const auto collider =
				RigidBodyCollider3::GetBuilder().WithSurface(box).MakeShared();

		solver->SetCollider(collider);

		this->InnerPtr = solver;

		// Print simulation info
		printf("Running example 2 (water-drop with SPH)\n");
	}

	this->InnerPtr->Update(++frame);

	return true;
}
