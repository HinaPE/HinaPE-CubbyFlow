#include "GAS_GT_PCISPHSolver.h"

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
#include <Core/Utils/Logging.hpp>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		GT_PCISPHSolver,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_GT_PCISPHSolver::_init()
{
	this->InnerPtr = nullptr;
	frame = CubbyFlow::Frame(0, 1.0 / 60);
}
void GAS_Hina_GT_PCISPHSolver::_makeEqual(const GAS_Hina_GT_PCISPHSolver *src)
{
	this->InnerPtr = src->InnerPtr;
}
bool GAS_Hina_GT_PCISPHSolver::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	using namespace CubbyFlow;
	if (!this->InnerPtr)
	{
		double targetSpacing = 0.02;

		BoundingBox3D domain(Vector3D(), Vector3D(3, 2, 1.5));
		const double lz = domain.Depth();

		// Build solver
		auto solver = PCISPHSolver3::GetBuilder()
				.WithTargetDensity(1000.0)
				.WithTargetSpacing(targetSpacing)
				.MakeShared();

		solver->SetPseudoViscosityCoefficient(0.0);
		solver->SetTimeStepLimitScale(10.0);

		// Build emitter
		BoundingBox3D sourceBound(domain);
		sourceBound.Expand(-targetSpacing);

		const auto box1 =
				Box3::GetBuilder()
						.WithLowerCorner({ 0, 0, 0 })
						.WithUpperCorner({ 0.5 + 0.001, 0.75 + 0.001, 0.75 * lz + 0.001 })
						.MakeShared();

		const auto box2 =
				Box3::GetBuilder()
						.WithLowerCorner({ 2.5 - 0.001, 0, 0.25 * lz - 0.001 })
						.WithUpperCorner({ 3.5 + 0.001, 0.75 + 0.001, 1.5 * lz + 0.001 })
						.MakeShared();

		const auto boxSet =
				ImplicitSurfaceSet3::GetBuilder()
						.WithExplicitSurfaces(Array1<Surface3Ptr>{ box1, box2 })
						.MakeShared();

		const auto emitter = VolumeParticleEmitter3::GetBuilder()
				.WithSurface(boxSet)
				.WithMaxRegion(sourceBound)
				.WithSpacing(targetSpacing)
				.MakeShared();

		solver->SetEmitter(emitter);

		// Build collider
		const auto cyl1 = Cylinder3::GetBuilder()
				.WithCenter({ 1, 0.375, 0.375 })
				.WithRadius(0.1)
				.WithHeight(0.75)
				.MakeShared();

		const auto cyl2 = Cylinder3::GetBuilder()
				.WithCenter({ 1.5, 0.375, 0.75 })
				.WithRadius(0.1)
				.WithHeight(0.75)
				.MakeShared();

		const auto cyl3 = Cylinder3::GetBuilder()
				.WithCenter({ 2, 0.375, 1.125 })
				.WithRadius(0.1)
				.WithHeight(0.75)
				.MakeShared();

		const auto box = Box3::GetBuilder()
				.WithIsNormalFlipped(true)
				.WithBoundingBox(domain)
				.MakeShared();

		const auto surfaceSet =
				ImplicitSurfaceSet3::GetBuilder()
						.WithExplicitSurfaces(Array1<Surface3Ptr>{ cyl1, cyl2, cyl3, box })
						.MakeShared();

		const auto collider =
				RigidBodyCollider3::GetBuilder().WithSurface(surfaceSet).MakeShared();

		solver->SetCollider(collider);

		this->InnerPtr = solver;

		// Print simulation info
		printf("Running example 3 (dam-breaking with PCISPH)\n");
	}

	this->InnerPtr->Update(++frame);
	return true;
}
