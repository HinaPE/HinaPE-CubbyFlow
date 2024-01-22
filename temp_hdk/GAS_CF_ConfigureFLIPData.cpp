#include "GAS_CF_ConfigureFLIPData.h"

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_GuideShared.h>
#include <SIM/SIM_ColliderLabel.h>
#include <SIM/SIM_ForceGravity.h>
#include <SIM/SIM_Time.h>
#include <SIM/SIM_Utils.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#include <SIM_CF_FLIPData.h>

#include "Core/Solver/Hybrid/FLIP/FLIPSolver3.hpp"
#include "Core/Geometry/Plane.hpp"
#include "Core/Geometry/Sphere.hpp"
#include "Core/Emitter/VolumeParticleEmitter3.hpp"
#include "Core/PointGenerator/GridPointGenerator3.hpp"
#include "Core/Emitter/ParticleEmitterSet3.hpp"
#include "Core/Geometry/Box.hpp"
#include "Core/Geometry/ImplicitSurfaceSet.hpp"
#include "Core/Geometry/Cylinder3.hpp"
#include "Core/Geometry/RigidBodyCollider.hpp"

bool GAS_CF_ConfigureFLIPData::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_ConfigureFLIPData::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_ConfigureFLIPData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_ConfigureFLIPData::DATANAME = "CF_ConfigureFLIPData";
const SIM_DopDescription *GAS_CF_ConfigureFLIPData::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_configure_FLIPdata",
								   "CF Configure FLIP Data",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_ConfigureFLIPData::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_FLIPData *flipdata = SIM_DATA_GET(*obj, SIM_CF_FLIPData::DATANAME, SIM_CF_FLIPData);
	if (!flipdata)
	{
		error_msg.appendSprintf("SIM_CF_FLIPData Is Null, From %s\n", DATANAME);
		return false;
	}

	if (flipdata->Configured)
		return true;

	{
		using namespace CubbyFlow;

		size_t resolutionX = 50;

		auto &solver = flipdata->SolverPtr;

		// Build solver
		const Vector3UZ resolution{ 3 * resolutionX, 2 * resolutionX,
									(3 * resolutionX) / 2 };
		solver = FLIPSolver3::Builder()
				.WithResolution(resolution)
				.WithDomainSizeX(3.0)
				.MakeShared();
		solver->SetUseCompressedLinearSystem(true);

		const auto grids = solver->GetGridSystemData();
		const double dx = grids->GridSpacing().x;
		const BoundingBox3D domain = grids->GetBoundingBox();
		const double lz = domain.Depth();

		// Build emitter
		const auto box1 =
				Box3::Builder()
						.WithLowerCorner({ 0, 0, 0 })
						.WithUpperCorner({ 0.5 + 0.001, 0.75 + 0.001, 0.75 * lz + 0.001 })
						.MakeShared();

		const auto box2 =
				Box3::Builder()
						.WithLowerCorner({ 2.5 - 0.001, 0, 0.25 * lz - 0.001 })
						.WithUpperCorner({ 3.5 + 0.001, 0.75 + 0.001, 1.5 * lz + 0.001 })
						.MakeShared();

		const auto boxSet =
				ImplicitSurfaceSet3::Builder()
						.WithExplicitSurfaces(Array1<Surface3Ptr>{ box1, box2 })
						.MakeShared();

		auto emitter = VolumeParticleEmitter3::Builder()
				.WithSurface(boxSet)
				.WithMaxRegion(domain)
				.WithSpacing(0.5 * dx)
				.MakeShared();

		emitter->SetPointGenerator(std::make_shared<GridPointGenerator3>());
		solver->SetParticleEmitter(emitter);

		// Build collider
		const auto cyl1 = Cylinder3::Builder()
				.WithCenter({ 1, 0.375, 0.375 })
				.WithRadius(0.1)
				.WithHeight(0.75)
				.MakeShared();

		const auto cyl2 = Cylinder3::Builder()
				.WithCenter({ 1.5, 0.375, 0.75 })
				.WithRadius(0.1)
				.WithHeight(0.75)
				.MakeShared();

		const auto cyl3 = Cylinder3::Builder()
				.WithCenter({ 2, 0.375, 1.125 })
				.WithRadius(0.1)
				.WithHeight(0.75)
				.MakeShared();

		const auto cylSet =
				ImplicitSurfaceSet3::Builder()
						.WithExplicitSurfaces(Array1<Surface3Ptr>{ cyl1, cyl2, cyl3 })
						.MakeShared();

		const auto collider =
				RigidBodyCollider3::Builder().WithSurface(cylSet).MakeShared();

		solver->SetCollider(collider);
	}

	flipdata->Configured = true;

	return true;
}
