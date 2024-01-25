#include "SIM_Hina_RigidBodyCollider.h"

NEW_HINA_COLLIDER_IMPLEMENT(
		RigidBodyCollider,
		false,
)

void SIM_Hina_RigidBodyCollider::_init()
{
	this->InnerPtr = nullptr;
}
void SIM_Hina_RigidBodyCollider::_makeEqual(const SIM_Hina_RigidBodyCollider *src)
{
	this->InnerPtr = src->InnerPtr;
}
