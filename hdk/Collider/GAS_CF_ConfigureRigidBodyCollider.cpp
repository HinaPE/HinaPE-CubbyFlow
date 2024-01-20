#include "GAS_CF_ConfigureRigidBodyCollider.h"

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

#include <Collider/SIM_CF_RigidBodyCollider.h>

#include <GEO/GEO_PrimPoly.h>

#include "Core/Geometry/RigidBodyCollider.hpp"
#include "Core/Geometry/ImplicitSurfaceSet.hpp"
#include "Core/Geometry/TriangleMesh3.hpp"

bool GAS_CF_ConfigureRigidBodyCollider::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_ConfigureRigidBodyCollider::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_ConfigureRigidBodyCollider::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_ConfigureRigidBodyCollider::DATANAME = "CF_ConfigureRigidBodyCollider";
const SIM_DopDescription *GAS_CF_ConfigureRigidBodyCollider::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_configure_rigidbody_collider",
								   "CF Configure RigidBody Collider",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_ConfigureRigidBodyCollider::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_ObjectArray affectors;
	obj->getAffectors(affectors, "SIM_RelationshipCollide");
	exint num_affectors = affectors.entries();

	for (exint i = 0; i < num_affectors; ++i)
	{
		SIM_Object *affector = affectors(i);
		if (!affector->getName().equal(obj->getName()))
		{
			SIM_CF_RigidBodyCollider *collider_data = SIM_DATA_GET(*affector, SIM_CF_RigidBodyCollider::DATANAME, SIM_CF_RigidBodyCollider);
			if (!collider_data)
			{
				error_msg.appendSprintf("Affector %s No Valid Collider Data, From %s\n", affector->getName().toStdString().c_str(), DATANAME);
				return false;
			}

			if (collider_data->Configured)
				continue;

			SIM_Geometry *collider_geo = SIM_DATA_GET(*affector, SIM_GEOMETRY_DATANAME, SIM_Geometry);
			if (!collider_geo)
			{
				error_msg.appendSprintf("Affector %s Collider Geometry Is Null, From %s\n", affector->getName().toStdString().c_str(), DATANAME);
				return false;
			}

			SIM_GeometryAutoReadLock lock(collider_geo);
			const GU_Detail *gdp_source = lock.getGdp();
			if (!gdp_source)
			{
				error_msg.appendSprintf("Affector %s Source Geometry GDP is nullptr, From %s\n", affector->getName().toStdString().c_str(), DATANAME);
				return false;
			}
			CubbyFlow::TriangleMesh3::PointArray points;
			CubbyFlow::TriangleMesh3::IndexArray point_indices;
			{
				GA_Offset pt_off;
				GA_FOR_ALL_PTOFF(gdp_source, pt_off)
					{
						const UT_Vector3 pos = gdp_source->getPos3(pt_off);
						points.Append({pos.x(), pos.y(), pos.z()});
					}

				const GEO_Primitive *prim;
				GA_FOR_ALL_PRIMITIVES(gdp_source, prim)
				{
					const auto *poly = dynamic_cast<const GEO_PrimPoly *>(prim);
					if (!poly)
					{
						error_msg.appendSprintf("ERROR ON CONVERT PRIM TO POLY, From %s\n", DATANAME);
						return false;
					}

					// Triangulate Polygon
					std::vector<size_t> polyIndices;
					for (int vi = 0; vi < poly->getVertexCount(); ++vi)
						polyIndices.push_back(poly->getPointIndex(vi));
					for (size_t i = 1; i < polyIndices.size() - 1; ++i)
						point_indices.Append({polyIndices[0], polyIndices[i + 1], polyIndices[i]}); // notice the normal

				}
			}
			CubbyFlow::TriangleMesh3Ptr mesh = CubbyFlow::TriangleMesh3::GetBuilder().WithPoints(points).WithPointIndices(point_indices).MakeShared();

			collider_data->InnerPtr = CubbyFlow::RigidBodyCollider3::GetBuilder()
					.WithSurface(mesh)
					.MakeShared();
		}
	}
	return true;
}
