#include "GAS_GT_SmokeSolver.h"

#include <Core/Array/Array.hpp>
#include <Core/Emitter/VolumeGridEmitter3.hpp>
#include <Core/Geometry/Box.hpp>
#include <Core/Geometry/ImplicitTriangleMesh3.hpp>
#include <Core/Geometry/RigidBodyCollider.hpp>
#include <Core/Geometry/Sphere.hpp>
#include <Core/Geometry/TriangleMesh3.hpp>
#include <Core/Grid/ScalarGrid.hpp>
#include <Core/Math/MathUtils.hpp>
#include <Core/Solver/Advection/CubicSemiLagrangian3.hpp>
#include <Core/Solver/Advection/SemiLagrangian3.hpp>
#include <Core/Solver/Grid/GridSmokeSolver3.hpp>
#include <Core/Utils/Logging.hpp>

#include <SIM/SIM_ScalarField.h>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		GT_SmokeSolver,
		false,
		ACTIVATE_GAS_DENSITY
)

void GAS_Hina_GT_SmokeSolver::_init()
{

}
void GAS_Hina_GT_SmokeSolver::_makeEqual(const GAS_Hina_GT_SmokeSolver *src)
{

}
bool GAS_Hina_GT_SmokeSolver::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	size_t resolutionX = 20;
	using namespace CubbyFlow;
	if (!this->InnerPtr)
	{
		auto solver =
				GridSmokeSolver3::Builder()
						.WithResolution({resolutionX, 2 * resolutionX, resolutionX})
						.WithDomainSizeX(1.0)
						.MakeShared();

		solver->SetAdvectionSolver(std::make_shared<CubicSemiLagrangian3>());

		const auto grids = solver->GetGridSystemData();
		BoundingBox3D domain = grids->GetBoundingBox();

		// Build emitter
		const auto box = Box3::Builder()
				.WithLowerCorner({0.45, -1, 0.45})
				.WithUpperCorner({0.55, 0.05, 0.55})
				.MakeShared();

		auto emitter = VolumeGridEmitter3::Builder()
				.WithSourceRegion(box)
				.WithIsOneShot(false)
				.MakeShared();

		solver->SetEmitter(emitter);
		emitter->AddStepFunctionTarget(solver->GetSmokeDensity(), 0, 1);
		emitter->AddStepFunctionTarget(solver->GetTemperature(), 0, 1);

		// Build collider
		const auto sphere = Sphere3::Builder()
				.WithCenter({0.5, 0.3, 0.5})
				.WithRadius(0.075 * domain.Width())
				.MakeShared();

		const auto collider =
				RigidBodyCollider3::Builder().WithSurface(sphere).MakeShared();

		solver->SetCollider(collider);

		this->InnerPtr = solver;

		// Print simulation info
		printf("Running example 1 (rising smoke with cubic-spline advection)\n");
	}

	this->InnerPtr->Update(++frame);

	// Update to HDK Scalar Field
	SIM_ScalarField *density = getScalarField(obj, GAS_NAME_DENSITY);
//	density->setSize();

	return true;
}
