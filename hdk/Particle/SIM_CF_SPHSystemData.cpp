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
	InnerPtr = std::make_shared<CubbyFlow::SPHSystemData3>();
	scalar_idx_offset = InnerPtr->AddScalarData();
	scalar_idx_state = InnerPtr->AddScalarData();
	scalar_idx_density = InnerPtr->AddScalarData();
	scalar_idx_pressure = InnerPtr->AddScalarData();
}

void SIM_CF_SPHSystemData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
	const SIM_CF_SPHSystemData *src = SIM_DATA_CASTCONST(source, SIM_CF_SPHSystemData);

	/// Implement Equal Operator of Your Custom Fields
	this->Configured = src->Configured;
	this->InnerPtr = src->InnerPtr;
	this->scalar_idx_offset = src->scalar_idx_offset;
	this->scalar_idx_state = src->scalar_idx_state;
	this->scalar_idx_density = src->scalar_idx_density;
	this->scalar_idx_pressure = src->scalar_idx_pressure;
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

GA_Offset SIM_CF_SPHSystemData::GetParticleOffset(size_t index, UT_WorkBuffer &error_msg) const
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

void SIM_CF_SPHSystemData::SetParticleOffset(size_t index, GA_Offset offset, UT_WorkBuffer &error_msg) const
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

SIM_CF_SPHSystemData::ParticleState SIM_CF_SPHSystemData::GetParticleState(size_t index, UT_WorkBuffer &error_msg) const
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

void SIM_CF_SPHSystemData::SetParticleState(size_t index, SIM_CF_SPHSystemData::ParticleState state, UT_WorkBuffer &error_msg) const
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
