#include "SIM_CF_RigidBodyCollider.h"

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

void SIM_CF_RigidBodyCollider::initializeSubclass()
{
	SIM_Data::initializeSubclass();
	InnerPtr = nullptr;
}

void SIM_CF_RigidBodyCollider::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
	const SIM_CF_RigidBodyCollider *src = SIM_DATA_CASTCONST(source, SIM_CF_RigidBodyCollider);

	/// Implement Equal Operator of Your Custom Fields
	this->InnerPtr = src->InnerPtr;
}

const SIM_DopDescription *SIM_CF_RigidBodyCollider::GetDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_rigidbody_collider",
								   "CF RigidBody Collider",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	return &DESC;
}
