#ifndef HINAPE_CUBBYFLOW_SIM_CF_RIGIDBODYCOLLIDER_H
#define HINAPE_CUBBYFLOW_SIM_CF_RIGIDBODYCOLLIDER_H

#include <SIM/SIM_Collider.h>
#include <PRM/PRM_Default.h>

#include "Core/Geometry/RigidBodyCollider.hpp"

class SIM_CF_RigidBodyCollider : public SIM_Collider
{
public:
	static const char *DATANAME;
	CubbyFlow::RigidBodyCollider3Ptr InnerPtr;

protected:
	SIM_CF_RigidBodyCollider(const SIM_DataFactory *factory) : SIM_Collider(factory) {}
	~SIM_CF_RigidBodyCollider() override = default;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *GetDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_CF_RigidBodyCollider, SIM_Collider, "CF_RigidBodyCollider", GetDescription());
};

#endif //HINAPE_CUBBYFLOW_SIM_CF_RIGIDBODYCOLLIDER_H
