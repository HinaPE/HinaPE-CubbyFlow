#include "SIM_CF_SPHSystemData.h"

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

void SIM_CF_SPHSystemData::initializeSubclass()
{
	SIM_Data::initializeSubclass();

	/// Implement Initializations of Your Custom Fields
	Configured = false;
}

void SIM_CF_SPHSystemData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
	const SIM_CF_SPHSystemData *src = SIM_DATA_CASTCONST(source, SIM_CF_SPHSystemData);

	/// Implement Equal Operator of Your Custom Fields
	static_cast<CubbyFlow::SPHSystemData3 &>(*this) = static_cast<const CubbyFlow::SPHSystemData3 &>(*src);

	/// Implement Equal Operator of Your Custom Fields
	this->Configured = src->Configured;
}

const char *SIM_CF_SPHSystemData::DATANAME = "CF_SPHSystemData";
const char *SIM_CF_SPHSystemData::FORCE_ATTRIBUTE_NAME = "force";
const char *SIM_CF_SPHSystemData::DENSITY_ATTRIBUTE_NAME = "density";
const char *SIM_CF_SPHSystemData::PRESSURE_ATTRIBUTE_NAME = "pressure";
const SIM_DopDescription *SIM_CF_SPHSystemData::GetDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_sph_system_data",
								   "CF SPH System Data",
								   DATANAME,
								   classname(),
								   PRMS.data());
	return &DESC;
}
