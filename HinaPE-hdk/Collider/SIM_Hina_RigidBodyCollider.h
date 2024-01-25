#ifndef HINAPE_SIM_HINA_RIGIDBODYCOLLIDER_H
#define HINAPE_SIM_HINA_RIGIDBODYCOLLIDER_H

#include "HinaHDKClassGenerator.h"
#include "Core/Geometry/RigidBodyCollider.hpp"

NEW_HINA_COLLIDER_CLASS(
		RigidBodyCollider,
		CubbyFlow::RigidBodyCollider3Ptr InnerPtr;
)

#endif //HINAPE_SIM_HINA_RIGIDBODYCOLLIDER_H
