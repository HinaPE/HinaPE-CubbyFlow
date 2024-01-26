#include "SIM_Hina_ConfigureForSPH.h"
#include <Particle/SIM_Hina_SPHCaches.h>
#include <Particle/SIM_Hina_PCISPHCaches.h>
#include <Collider/SIM_Hina_RigidBodyCollider.h>

#include "Core/Geometry/RigidBodyCollider.hpp"
#include "Core/Geometry/ImplicitSurfaceSet.hpp"
#include "Core/Geometry/TriangleMesh3.hpp"
#include "Core/Geometry/Box.hpp"

NEW_HINA_DATA_IMPLEMENT_NO_NODE(
		ConfigureForSPH,
		false,
)

void SIM_Hina_ConfigureForSPH::_init() {}
void SIM_Hina_ConfigureForSPH::_makeEqual(const SIM_Hina_ConfigureForSPH *src) {}
bool SIM_Hina_ConfigureForSPH::InitConfigure(SIM_Object *obj, SIM_Hina_ParticleFluidData *data)
{
	CHECK_NULL(obj)
	CHECK_NULL(data)

	CubbyFlow::Logging::Mute();

	SIM_GeometryCopy *geo = SIM_DATA_CREATE(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy,
											SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE);
	CHECK_NULL(geo)

	SIM_ObjectArray affectors;
	obj->getAffectors(affectors, "SIM_RelationshipCollide");
	exint num_affectors = affectors.entries();
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		if (!data->Configured)
		{
			// Init GDP Attributes
			GA_RWAttributeRef CF_IDX_ref = gdp.addIntTuple(GA_ATTRIB_POINT, data->getCF_IDX_ATTRIBUTE_NAME(), 1, GA_Defaults(0));
			CF_IDX_ref.setTypeInfo(GA_TYPE_VECTOR);
			GA_RWAttributeRef CF_STATE_ref = gdp.addStringTuple(GA_ATTRIB_POINT, data->getCF_STATE_ATTRIBUTE_NAME(), 1);
			CF_STATE_ref.setTypeInfo(GA_TYPE_VOID);
			GA_RWAttributeRef velocity_ref = gdp.addFloatTuple(GA_ATTRIB_POINT, data->getVELOCITY_ATTRIBUTE_NAME(), 3, GA_Defaults(0));
			velocity_ref.setTypeInfo(GA_TYPE_VECTOR);
			GA_RWAttributeRef force_ref = gdp.addFloatTuple(GA_ATTRIB_POINT, data->getFORCE_ATTRIBUTE_NAME(), 3, GA_Defaults(0));
			force_ref.setTypeInfo(GA_TYPE_VECTOR);
			GA_RWAttributeRef mass_ref = gdp.addFloatTuple(GA_ATTRIB_POINT, data->getMASS_ATTRIBUTE_NAME(), 1, GA_Defaults(0));
			mass_ref.setTypeInfo(GA_TYPE_VOID);
			GA_RWAttributeRef density_ref = gdp.addFloatTuple(GA_ATTRIB_POINT, data->getDENSITY_ATTRIBUTE_NAME(), 1, GA_Defaults(0));
			density_ref.setTypeInfo(GA_TYPE_VOID);
			GA_RWAttributeRef pressure_ref = gdp.addFloatTuple(GA_ATTRIB_POINT, data->getPRESSURE_ATTRIBUTE_NAME(), 1, GA_Defaults(0));
			pressure_ref.setTypeInfo(GA_TYPE_VOID);
			GA_RWAttributeRef neighbor_list_ref = gdp.addIntArray(GA_ATTRIB_POINT, data->getNEIGHBORS_ATTRIBUTE_NAME());
			neighbor_list_ref.setTypeInfo(GA_TYPE_VOID);
			GA_RWAttributeRef neighbor_sum_ref = gdp.addIntTuple(GA_ATTRIB_POINT, data->getNEIGHBORS_SUM_ATTRIBUTE_NAME(), 1, GA_Defaults(0));
			neighbor_sum_ref.setTypeInfo(GA_TYPE_VOID);

			// Init InnerPtr
			double TargetDensity = data->getTargetDensity();
			double TargetSpacing = data->getTargetSpacing();
			double KernelRadiusOverTargetSpacing = data->getKernelRadiusOverTargetSpacing();
			data->InnerPtr = std::make_shared<CubbyFlow::SPHSystemData3>();
			data->InnerPtr->SetTargetDensity(TargetDensity);
			data->InnerPtr->SetTargetSpacing(TargetSpacing);
			data->InnerPtr->SetRelativeKernelRadius(KernelRadiusOverTargetSpacing);

			// Init Custom Variable
			data->scalar_idx_offset = data->InnerPtr->AddScalarData();
			data->scalar_idx_state = data->InnerPtr->AddScalarData();

			// Add Caches
			if (data->getFluidDataType() == 0)
				SIM_DATA_CREATE(*data, SIM_Hina_SPHCaches::DATANAME, SIM_Hina_SPHCaches, SIM_DATA_RETURN_EXISTING |
																						 SIM_DATA_ADOPT_EXISTING_ON_DELETE); // maybe we need just to delete the old and create new
			else if (data->getFluidDataType() == 1)
				SIM_DATA_CREATE(*data, SIM_Hina_PCISPHCaches::DATANAME, SIM_Hina_PCISPHCaches, SIM_DATA_RETURN_EXISTING |
																							   SIM_DATA_ADOPT_EXISTING_ON_DELETE); // maybe we need just to delete the old and create new


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
							for (size_t ii = 1; ii < polyIndices.size() - 1; ++ii)
								point_indices.Append({polyIndices[0], polyIndices[ii +
																				  1], polyIndices[ii]}); // notice the normal
						}
					}
					CubbyFlow::TriangleMesh3Ptr mesh = CubbyFlow::TriangleMesh3::GetBuilder().WithPoints(points).WithPointIndices(point_indices).MakeShared();

					collider_data->InnerPtr = CubbyFlow::RigidBodyCollider3::GetBuilder()
							.WithSurface(mesh)
							.MakeShared();
					collider_data->Configured = true;
				}
			}

			data->Configured = true;
		}
		data->runtime_init_handles(gdp);
	}

	return true;
}
bool SIM_Hina_ConfigureForSPH::UpdateConfigure(SIM_Object *obj, SIM_Hina_ParticleFluidData *data, SIM_Time current_time, SIM_Time timestep)
{
	CHECK_NULL(obj)
	CHECK_NULL(data)

	CubbyFlow::Logging::Mute();

	SIM_GeometryCopy *geo = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy);
	CHECK_NULL(geo)

	SIM_ObjectArray affectors;
	obj->getAffectors(affectors, "SIM_RelationshipCollide");
	exint num_affectors = affectors.entries();
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
			collider_data->InnerPtr->Update(current_time, timestep); // Notice this time and timestep
		}
	}

	return true;
}
