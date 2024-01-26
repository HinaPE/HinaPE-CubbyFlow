#include "GAS_Hina_ConfigureForSPH.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>
#include <Collider/SIM_Hina_RigidBodyCollider.h>

#include "Core/Geometry/RigidBodyCollider.hpp"
#include "Core/Geometry/ImplicitSurfaceSet.hpp"
#include "Core/Geometry/TriangleMesh3.hpp"
#include "Core/Geometry/Box.hpp"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		ConfigureForSPH,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_ConfigureForSPH::_init() {}
void GAS_Hina_ConfigureForSPH::_makeEqual(const GAS_Hina_ConfigureForSPH *src) {}
bool GAS_Hina_ConfigureForSPH::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	SIM_ObjectArray affectors;
	obj->getAffectors(affectors, "SIM_RelationshipCollide");
	exint num_affectors = affectors.entries();
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		if (!data->Configured)
		{
			data->configure_init(gdp);
			data->Configured = true;

			// Init Colliders
			if (data->getActivateFluidDomainCollider())
			{
				// Activate Fluid Domain Collider
				SIM_Hina_RigidBodyCollider *fluid_domain_collider_data = SIM_DATA_CREATE(*obj, SIM_Hina_RigidBodyCollider::DATANAME, SIM_Hina_RigidBodyCollider,
																						 SIM_DATA_RETURN_EXISTING |
																						 SIM_DATA_ADOPT_EXISTING_ON_DELETE);

				// TODO: rewrite following with SIM_Hina_Box
				UT_Vector3D MaxRegion = data->getFluidDomainD();
				CubbyFlow::BoundingBox3D fluid_domain(
						CubbyFlow::Vector3D(-MaxRegion.x() / 2, -MaxRegion.y() / 2, -MaxRegion.z() / 2),
						CubbyFlow::Vector3D(MaxRegion.x() / 2, MaxRegion.y() / 2, MaxRegion.z() / 2)
				);
				const auto box = CubbyFlow::Box3::GetBuilder()
						.WithBoundingBox(fluid_domain)
						.WithIsNormalFlipped(true)
						.MakeShared();
				fluid_domain_collider_data->InnerPtr = CubbyFlow::RigidBodyCollider3::GetBuilder()
						.WithSurface(box)
						.MakeShared();
				fluid_domain_collider_data->Configured = true;
			}
			for (exint i = 0; i < num_affectors; ++i)
			{
				SIM_Object *affector = affectors(i);
				if (!affector->getName().equal(obj->getName()))
				{
					SIM_Hina_RigidBodyCollider *collider_data = SIM_DATA_GET(*affector, SIM_Hina_RigidBodyCollider::DATANAME, SIM_Hina_RigidBodyCollider);
					if (!collider_data)
						continue;
					if (collider_data->Configured)
						continue;
					SIM_Geometry *collider_geo = SIM_DATA_GET(*affector, SIM_GEOMETRY_DATANAME, SIM_Geometry);
					CHECK_NULL(collider_geo)

					SIM_GeometryAutoReadLock lock(collider_geo);
					const GU_Detail *gdp_source = lock.getGdp();
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
								point_indices.Append({polyIndices[0], polyIndices[i +
																				  1], polyIndices[i]}); // notice the normal
						}
					}
					CubbyFlow::TriangleMesh3Ptr mesh = CubbyFlow::TriangleMesh3::GetBuilder().WithPoints(points).WithPointIndices(point_indices).MakeShared();

					collider_data->InnerPtr = CubbyFlow::RigidBodyCollider3::GetBuilder()
							.WithSurface(mesh)
							.MakeShared();
					collider_data->Configured = true;
				}
			}
		}
		data->runtime_init_handles(gdp);
		data->update_dynamic_dt(); // Compute desired time step

		// Update Collider
		for (exint i = 0; i < num_affectors; ++i)
		{
			SIM_Object *affector = affectors(i);
			{
				SIM_Hina_RigidBodyCollider *collider_data = SIM_DATA_GET(*affector, SIM_Hina_RigidBodyCollider::DATANAME, SIM_Hina_RigidBodyCollider);
				if (!collider_data)
					continue;
				if (collider_data->Configured)
					continue;
				collider_data->InnerPtr->Update(time, timestep); // Notice this time and timestep
			}
		}
	}

	return true;
}
