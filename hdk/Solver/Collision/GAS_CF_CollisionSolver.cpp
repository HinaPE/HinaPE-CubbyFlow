#include "GAS_CF_CollisionSolver.h"

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

#include <Particle/SIM_CF_SPHSystemData.h>
#include <Collider/SIM_CF_RigidBodyCollider.h>

bool GAS_CF_CollisionSolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_CollisionSolver::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_CollisionSolver::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_CollisionSolver::DATANAME = "CF_CollisionSolver";
const SIM_DopDescription *GAS_CF_CollisionSolver::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_collision_solver",
								   "CF Collision Solver",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}
bool GAS_CF_CollisionSolver::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_SPHSystemData *sphdata = SIM_DATA_GET(*obj, SIM_CF_SPHSystemData::DATANAME, SIM_CF_SPHSystemData);
	if (!sphdata || !sphdata->Configured)
	{
		error_msg.appendSprintf("CF_ParticleSystemData Is Null or Not Configured, From %s\n", DATANAME);
		return false;
	}

	SIM_ObjectArray affectors;
	obj->getAffectors(affectors, "SIM_RelationshipCollide");
	exint num_affectors = affectors.entries();

	for (exint i = 0; i < num_affectors; ++i)
	{
		SIM_Object *affector = affectors(i);
//		if (!affector->getName().equal(obj->getName())) # Also Collider With Self Fluid Domain Boundary
		{
			SIM_CF_RigidBodyCollider *collider_data = SIM_DATA_GET(*affector, SIM_CF_RigidBodyCollider::DATANAME, SIM_CF_RigidBodyCollider);
			if (!collider_data)
				continue;

			if (!collider_data->Configured)
			{
				error_msg.appendSprintf("Affector %s Collider is not Configured, From %s\n", affector->getName().toStdString().c_str(), DATANAME);
				return false;
			}

			{
				using namespace CubbyFlow;
				auto &m_particleSystemData = sphdata->InnerPtr;
				auto &m_collider = collider_data->InnerPtr;
				auto m_restitutionCoefficient = sphdata->getRestitutionCoefficient();
				auto &newPositions = sphdata->newPositions_Cache;
				auto &newVelocities = sphdata->newVelocities_Cache;

				const size_t numberOfParticles =
						m_particleSystemData->NumberOfParticles();
				const double radius = m_particleSystemData->Radius();

				ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i) {
					m_collider->ResolveCollision(radius, m_restitutionCoefficient,
												 &newPositions[i], &newVelocities[i]);
				});
			}
		}
	}

	SIM_GeometryCopy *geo = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy);
	if (!geo)
	{
		error_msg.appendSprintf("Geometry Is Null, From %s\n", DATANAME);
		return false;
	}

	// Sync Geometry Sheet
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

		GA_RWHandleV3 gdp_handle_new_pos = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEW_POSITION_CACHE_ATTRIBUTE_NAME);
		GA_RWHandleV3 gdp_handle_new_vel = gdp.findPointAttribute(SIM_CF_SPHSystemData::NEW_VELOCITY_CACHE_ATTRIBUTE_NAME);
		GA_RWHandleI gdp_handle_CL_PT_IDX = gdp.findPointAttribute(SIM_CF_SPHSystemData::CL_PT_IDX_ATTRIBUTE_NAME);

		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				int cl_index = gdp_handle_CL_PT_IDX.get(pt_off);
				UT_Vector3 new_pos = UT_Vector3D{sphdata->newPositions_Cache[cl_index].x, sphdata->newPositions_Cache[cl_index].y, sphdata->newPositions_Cache[cl_index].z};
				gdp_handle_new_pos.set(pt_off, new_pos);
				UT_Vector3 new_vel = UT_Vector3D{sphdata->newVelocities_Cache[cl_index].x, sphdata->newVelocities_Cache[cl_index].y, sphdata->newVelocities_Cache[cl_index].z};
				gdp_handle_new_vel.set(pt_off, new_vel);
			}
	}

	return true;
}
