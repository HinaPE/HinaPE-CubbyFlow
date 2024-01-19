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
 *
 * 					[!!! IMPORTANT !!!]
 * 		[!!! NEVER CALL GET_SET FUNCTION HERE !!!]
 *
 */
void SIM_CF_ParticleSystemData::initializeSubclass()
{
	SIM_Data::initializeSubclass(); // Always remember to call this parent implementation

	/// Implement Initializations of Your Custom Fields
	Configured = false;
	InnerPtr = std::make_shared<CubbyFlow::ParticleSystemData3>();
	scalar_idx_offset = InnerPtr->AddScalarData();
	scalar_idx_state = InnerPtr->AddScalarData();
}

/**
 * Run Every Time the Frame advanced, Copy the SIM_CF_ParticleSystemData of previous frame to current frame
 */
void SIM_CF_ParticleSystemData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source); // Always remember to call this parent implementation
	const SIM_CF_ParticleSystemData *src = SIM_DATA_CASTCONST(source, SIM_CF_ParticleSystemData);

	/// Implement Equal Operator of Your Custom Fields
	this->Configured = src->Configured;
	this->InnerPtr = src->InnerPtr;
	this->scalar_idx_offset = src->scalar_idx_offset;
	this->scalar_idx_state = src->scalar_idx_state;
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

GA_Offset SIM_CF_ParticleSystemData::GetParticleOffset(size_t index, UT_WorkBuffer &error_msg) const
{
	if (!InnerPtr)
	{
		error_msg.appendSprintf("ParticleSystemData InnerPtr is nullptr, From %s\n", DATANAME);
		return -1;
	}

	if (index >= InnerPtr->NumberOfParticles())
	{
		error_msg.appendSprintf("INVALID Index, From %s\n", DATANAME);
		return -1;
	}

	return (GA_Offset) InnerPtr->ScalarDataAt(scalar_idx_offset)[index];
}

void SIM_CF_ParticleSystemData::SetParticleOffset(size_t index, GA_Offset offset, UT_WorkBuffer &error_msg) const
{
	if (!InnerPtr)
	{
		error_msg.appendSprintf("ParticleSystemData InnerPtr is nullptr, From %s\n", DATANAME);
		return;
	}

	if (index >= InnerPtr->NumberOfParticles())
	{
		error_msg.appendSprintf("INVALID Index, From %s\n", DATANAME);
		return;
	}

	InnerPtr->ScalarDataAt(scalar_idx_offset)[index] = offset;
}

SIM_CF_ParticleSystemData::ParticleState SIM_CF_ParticleSystemData::GetParticleState(size_t index, UT_WorkBuffer &error_msg) const
{
	if (!InnerPtr)
	{
		error_msg.appendSprintf("ParticleSystemData InnerPtr is nullptr, From %s\n", DATANAME);
		return PARTICLE_INVALID;
	}

	if (index >= InnerPtr->NumberOfParticles())
	{
		error_msg.appendSprintf("INVALID Index, From %s\n", DATANAME);
		return PARTICLE_INVALID;
	}

	return (ParticleState) InnerPtr->ScalarDataAt(scalar_idx_state)[index];
}

void SIM_CF_ParticleSystemData::SetParticleState(size_t index, SIM_CF_ParticleSystemData::ParticleState state, UT_WorkBuffer &error_msg) const
{
	if (!InnerPtr)
	{
		error_msg.appendSprintf("ParticleSystemData InnerPtr is nullptr, From %s\n", DATANAME);
		return;
	}

	if (index >= InnerPtr->NumberOfParticles())
	{
		error_msg.appendSprintf("INVALID Index, From %s\n", DATANAME);
		return;
	}

	InnerPtr->ScalarDataAt(scalar_idx_offset)[index] = state;
}
