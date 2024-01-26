#include "GAS_Hina_CollisionSolve.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>
#include <Collider/SIM_Hina_RigidBodyCollider.h>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		CollisionSolve,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_CollisionSolve::_init() {}
void GAS_Hina_CollisionSolve::_makeEqual(const GAS_Hina_CollisionSolve *src) {}
bool GAS_Hina_CollisionSolve::_solve(SIM_Engine &, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)

	SIM_ObjectArray affectors;
	obj->getAffectors(affectors, "SIM_RelationshipCollide");
	exint num_affectors = affectors.entries();
//	for (exint i = 0; i < num_affectors; ++i)
//	{
//		SIM_Object *affector = affectors(i);
//		{
//			SIM_Hina_RigidBodyCollider *collider_data = SIM_DATA_GET(*affector, SIM_Hina_RigidBodyCollider::DATANAME, SIM_Hina_RigidBodyCollider);
//			if (!collider_data)
//				continue;
//			if (collider_data->Configured)
//				continue;
//			collider_data->InnerPtr->Update(time, timestep); // Notice this time and timestep
//
//			{
//				using namespace CubbyFlow;
//				auto &m_particleSystemData = data->InnerPtr;
//				auto &m_collider = collider_data->InnerPtr;
//				auto m_restitutionCoefficient = data->getRestitutionCoefficient();
//				auto &newPositions = data->newPositions_Cache;
//				auto &newVelocities = data->newVelocities_Cache;
//
//				const size_t numberOfParticles =
//						m_particleSystemData->NumberOfParticles();
//				const double radius = m_particleSystemData->Radius();
//
//				ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i)
//				{
//					m_collider->ResolveCollision(radius, m_restitutionCoefficient,
//												 &newPositions[i], &newVelocities[i]);
//				});
//			}
//		}
//	}

	return true;
}
