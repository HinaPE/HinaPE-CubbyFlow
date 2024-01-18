#include "SIM_CF_ParticleSystemData.h"

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

/**
 * Run Only Once at First Frame in Houdini
 */
void SIM_CF_ParticleSystemData::initializeSubclass()
{
	SIM_Data::initializeSubclass(); // Always remember to call this parent implementation

	/// Implement Initializations of Your Custom Fields
	Configured = false;
}

/**
 * Run Every Time the Frame advanced, Copy the SIM_CF_ParticleSystemData of previous frame to current frame
 */
void SIM_CF_ParticleSystemData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source); // Always remember to call this parent implementation
	const SIM_CF_ParticleSystemData *src = SIM_DATA_CASTCONST(source, SIM_CF_ParticleSystemData);

	/// Implement Equal Operator of Your Custom Fields
	static_cast<CubbyFlow::ParticleSystemData3 &>(*this) = static_cast<const CubbyFlow::ParticleSystemData3 &>(*src);

	/// Implement Equal Operator of Your Custom Fields
	this->Configured = src->Configured;
}

const char *SIM_CF_ParticleSystemData::DATANAME = "CF_ParticleSystemData";
const char *SIM_CF_ParticleSystemData::FORCE_ATTRIBUTE_NAME = "force";
const SIM_DopDescription *SIM_CF_ParticleSystemData::GetDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_particle_system_data",
								   "CF Particle System Data",
								   DATANAME,
								   classname(),
								   PRMS.data());
	return &DESC;
}
