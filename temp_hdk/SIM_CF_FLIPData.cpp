#include "SIM_CF_FLIPData.h"

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_Position.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_GuideShared.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>
#include <PRM/PRM_ChoiceList.h>

#include <GEO/GEO_PrimPoly.h>

void SIM_CF_FLIPData::initializeSubclass()
{
	SIM_Data::initializeSubclass();

	/// Implement Initializations of Your Custom Fields
	Configured = false;
	InnerPtr = nullptr;

// 					[!!! IMPORTANT !!!]
// 		[!!! NEVER CALL GET_SET FUNCTION HERE !!!]
// 					[!!! IMPORTANT !!!]
}
void SIM_CF_FLIPData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
	const SIM_CF_GridSystemData *src = SIM_DATA_CASTCONST(source, SIM_CF_GridSystemData);

	/// Implement Equal Operator of Your Custom Fields
	this->Configured = src->Configured;
	this->InnerPtr = src->InnerPtr;
}

const char *SIM_CF_FLIPData::DATANAME = "CF_GridSystemData";
const SIM_DopDescription *SIM_CF_FLIPData::GetDescription()
{
	static std::array<PRM_Template, OPTIONS_SIZE> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_FLIP_system_data",
								   "CF FLIP System Data",
								   DATANAME,
								   classname(),
								   PRMS.data());
	return &DESC;
}

SIM_Guide *SIM_CF_FLIPData::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}

void SIM_CF_FLIPData::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	SIM_Data::buildGuideGeometrySubclass(root, options, gdh, xform, t);
}
