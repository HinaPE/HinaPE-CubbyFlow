#ifndef HINAPE_CUBBYFLOW_SIM_HINA_RIGIDBODYCOLLIDER_H
#define HINAPE_CUBBYFLOW_SIM_HINA_RIGIDBODYCOLLIDER_H

#include "HinaHDKClassGenerator.h"
#include "Core/Geometry/RigidBodyCollider.hpp"

NEW_HINA_COLLIDER_CLASS(
		RigidBodyCollider,
		CubbyFlow::RigidBodyCollider3Ptr InnerPtr;
)

#endif //HINAPE_CUBBYFLOW_SIM_HINA_RIGIDBODYCOLLIDER_H
