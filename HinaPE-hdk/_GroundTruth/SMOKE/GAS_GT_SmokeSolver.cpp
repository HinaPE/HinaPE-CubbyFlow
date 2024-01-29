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
#include <SIM/SIM_RawField.h>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		GT_SmokeSolver,
		false,
		ACTIVATE_GAS_DENSITY
)

const size_t EDGE_BLUR = 3;
const float EDGE_BLUR_F = 3.f;
const double TGA_SCALE = 10.0;
inline float SmoothStep(float edge0, float edge1, float x)
{
	const float t = CubbyFlow::Clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
	return t * t * (3.f - 2.f * t);
}
void GAS_Hina_GT_SmokeSolver::_init()
{
	this->InnerPtr = nullptr;
	frame = CubbyFlow::Frame(0, 1.0 / 60);
}
void GAS_Hina_GT_SmokeSolver::_makeEqual(const GAS_Hina_GT_SmokeSolver *src)
{
	this->InnerPtr = src->InnerPtr;
	this->frame = src->frame;
}
bool GAS_Hina_GT_SmokeSolver::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	// Update to HDK Scalar Field
	SIM_ScalarField *density_SF = getScalarField(obj, GAS_NAME_DENSITY);
	SIM_RawField *field = density_SF->getField();
	UT_Vector3 origin_SF = density_SF->getOrig();
	UT_Vector3I resolution_SF = field->getVoxelRes();
	double DomainSizeX = density_SF->getSize().x();
	UT_Vector3 center_SF = density_SF->getCenter();

	using namespace CubbyFlow;
	if (!this->InnerPtr)
	{
		Vector3D Origin = Vector3D(origin_SF.x(), origin_SF.y(), origin_SF.z());
		Vector3D Center = Vector3D(center_SF.x(), center_SF.y(), center_SF.z());
		Vector3D Delta = Vector3D(DomainSizeX / 4., DomainSizeX / 4., DomainSizeX / 4.);

		auto solver =
				GridSmokeSolver3::Builder()
						.WithOrigin(Origin)
						.WithResolution(Vector3UZ(resolution_SF.x(), resolution_SF.y(), resolution_SF.z()))
						.WithDomainSizeX(DomainSizeX)
						.MakeShared();

		solver->SetAdvectionSolver(std::make_shared<CubicSemiLagrangian3>());

		const auto grids = solver->GetGridSystemData();
		BoundingBox3D domain = grids->GetBoundingBox();

		// Build emitter
		const auto box = Box3::Builder()
				.WithLowerCorner(Center - Delta)
				.WithUpperCorner(Center + Delta)
				.MakeShared();

		const auto sphere = Sphere3::Builder()
				.WithCenter(Center - Vector3D(0, DomainSizeX / 2, 0))
				.WithRadius(DomainSizeX / 8)
				.MakeShared();

		auto emitter = VolumeGridEmitter3::Builder()
				.WithSourceRegion(sphere)
				.WithIsOneShot(false)
				.MakeShared();

		solver->SetEmitter(emitter);
		emitter->AddStepFunctionTarget(solver->GetSmokeDensity(), 0, 1);
		emitter->AddStepFunctionTarget(solver->GetTemperature(), 0, 1);

		// Build collider
//		const auto sphere = Sphere3::Builder()
//				.WithCenter({0.5, 0.3, 0.5})
//				.WithRadius(0.075 * domain.Width())
//				.MakeShared();
//
//		const auto collider =
//				RigidBodyCollider3::Builder().WithSurface(sphere).MakeShared();
//
//		solver->SetCollider(collider);

		this->InnerPtr = solver;

		// Print simulation info
		printf("Running example 1 (rising smoke with cubic-spline advection)\n");
	}

	this->InnerPtr->Update(++frame);

	const auto density = this->InnerPtr->GetSmokeDensity();
	/*
	 * Cubby vs HDK
	 * Resolution = Divisions
	 *
	 */

//	std::cout << density->GetBoundingBox().Width() << " " << density->GetBoundingBox().Height() << " "
//			  << density->GetBoundingBox().Depth() << std::endl;
//	std::cout << density_SF->getSize() << std::endl;
//	std::cout << density->Resolution().x << " " << density->Resolution().y << " " << density->Resolution().z
//			  << std::endl;
//	std::cout << density_SF->getDivisions() << std::endl;
//	std::cout << density->DataOrigin().x << " " << density->DataOrigin().y << " " << density->DataOrigin().z
//			  << std::endl;
//	std::cout << density_SF->getOrig() << std::endl;
//	CubbyFlow::Vector3D grid111 = density->DataPosition()(0, 0, 0);
//	UT_Vector3 grid111_SF;
//	density_SF->indexToPos(0, 0, 0, grid111_SF);
//	std::cout << grid111_SF << std::endl;
//	std::cout << grid111.x << " " << grid111.y << " " << grid111.z
//			  << std::endl;

//	std::cout << density_SF->getTotalVoxelRes() << "\n";
//	std::cout << density->Resolution().x << " " << density->Resolution().y << " " << density->Resolution().z;

	for (int ix = 0; ix < resolution_SF.x(); ++ix)
	{
		for (int iy = 0; iy < resolution_SF.y(); ++iy)
		{
			for (int iz = 0; iz < resolution_SF.z(); ++iz)
			{
				int i = ix;
				int j = iy;
				int k = iz;

				float d = static_cast<float>((*density)(i, j, k));

				// Blur the edge for less-noisy rendering
				if (i < EDGE_BLUR)
				{
					d *= SmoothStep(0.f, EDGE_BLUR_F, static_cast<float>(i));
				}
				if (i > resolution_SF.x() - 1 - EDGE_BLUR)
				{
					d *= SmoothStep(0.f, EDGE_BLUR_F,
									static_cast<float>((resolution_SF.x() - 1) - i));
				}
				if (j < EDGE_BLUR)
				{
					d *= SmoothStep(0.f, EDGE_BLUR_F, static_cast<float>(j));
				}
				if (j > resolution_SF.y() - 1 - EDGE_BLUR)
				{
					d *= SmoothStep(0.f, EDGE_BLUR_F,
									static_cast<float>((resolution_SF.y() - 1) - j));
				}
				if (k < EDGE_BLUR)
				{
					d *= SmoothStep(0.f, EDGE_BLUR_F, static_cast<float>(k));
				}
				if (k > resolution_SF.z() - 1 - EDGE_BLUR)
				{
					d *= SmoothStep(0.f, EDGE_BLUR_F,
									static_cast<float>((resolution_SF.z() - 1) - k));
				}

				field->setCellValue(ix, iy, iz, d);
			}
		}
	}
	return true;
}
