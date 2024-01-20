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
	InnerPtr = nullptr;

// 					[!!! IMPORTANT !!!]
// 		[!!! NEVER CALL GET_SET FUNCTION HERE !!!]
// 					[!!! IMPORTANT !!!]
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
}

const char *SIM_CF_SPHSystemData::DATANAME = "CF_SPHSystemData";
const char *SIM_CF_SPHSystemData::FORCE_ATTRIBUTE_NAME = "force";
const char *SIM_CF_SPHSystemData::DENSITY_ATTRIBUTE_NAME = "density";
const char *SIM_CF_SPHSystemData::PRESSURE_ATTRIBUTE_NAME = "pressure";
const SIM_DopDescription *SIM_CF_SPHSystemData::GetDescription()
{
	/// ParticleSystemData Parameters
	static PRM_Name ParticleRadius("ParticleRadius", "ParticleRadius");
	static PRM_Default ParticleRadiusDefault{1e-3};


	/// SPHSystemData Parameters
	static PRM_Name TargetDensity("TargetDensity", "TargetDensity");
	static PRM_Default TargetDensityDefault{1000.};

	static PRM_Name TargetSpacing("TargetSpacing", "TargetSpacing");
	static PRM_Default TargetSpacingDefault{.1};

	static PRM_Name KernelRadiusOverTargetSpacing("KernelRadiusOverTargetSpacing", "KernelRadiusOverTargetSpacing");
	static PRM_Default KernelRadiusOverTargetSpacingDefault{1.8};

	static PRM_Name KernelRadius("KernelRadius", "KernelRadius");
	static PRM_Default KernelRadiusDefault{.1};


	/// ParticleSystemSolver Parameters
	static PRM_Name Gravity("Gravity", "Gravity");
	static std::array<PRM_Default, 3> GravityDefault{0, -9.8, 0};

	static PRM_Name RestitutionCoefficient("RestitutionCoefficient", "RestitutionCoefficient");
	static PRM_Default RestitutionCoefficientDefault{0.};


	/// SPHSolver Parameters
	static PRM_Name EosExponent("EosExponent", "EosExponent");
	static PRM_Default EosExponentDefault{7.};

	static PRM_Name NegativePressureScale("NegativePressureScale", "NegativePressureScale");
	static PRM_Default NegativePressureScaleDefault{0.};

	static PRM_Name ViscosityCoefficient("ViscosityCoefficient", "ViscosityCoefficient");
	static PRM_Default ViscosityCoefficientDefault{0.01};

	static PRM_Name PseudoViscosityCoefficient("PseudoViscosityCoefficient", "PseudoViscosityCoefficient");
	static PRM_Default PseudoViscosityCoefficientDefault{10};

	//! Speed of sound in medium to determine the stiffness of the system.
	//! Ideally, it should be the actual speed of sound in the fluid, but in
	//! practice, use lower value to trace-off performance and compressibility.
	static PRM_Name SpeedOfSound("SpeedOfSound", "SpeedOfSound");
	static PRM_Default SpeedOfSoundDefault{100};

	static PRM_Name TimeStepLimitScale("TimeStepLimitScale", "TimeStepLimitScale");
	static PRM_Default TimeStepLimitScaleDefault{1.};


	static std::array<PRM_Template, 14> PRMS{
			PRM_Template(PRM_FLT, 1, &ParticleRadius, &ParticleRadiusDefault),
			PRM_Template(PRM_FLT, 1, &TargetDensity, &TargetDensityDefault),
			PRM_Template(PRM_FLT, 1, &TargetSpacing, &TargetSpacingDefault),
			PRM_Template(PRM_FLT, 1, &KernelRadiusOverTargetSpacing, &KernelRadiusOverTargetSpacingDefault),
			PRM_Template(PRM_FLT, 1, &KernelRadius, &KernelRadiusDefault),
			PRM_Template(PRM_FLT, 3, &Gravity, GravityDefault.data()),
			PRM_Template(PRM_FLT, 1, &RestitutionCoefficient, &RestitutionCoefficientDefault),
			PRM_Template(PRM_FLT, 1, &EosExponent, &EosExponentDefault),
			PRM_Template(PRM_FLT, 1, &NegativePressureScale, &NegativePressureScaleDefault),
			PRM_Template(PRM_FLT, 1, &ViscosityCoefficient, &ViscosityCoefficientDefault),
			PRM_Template(PRM_FLT, 1, &PseudoViscosityCoefficient, &PseudoViscosityCoefficientDefault),
			PRM_Template(PRM_FLT, 1, &SpeedOfSound, &SpeedOfSoundDefault),
			PRM_Template(PRM_FLT, 1, &TimeStepLimitScale, &TimeStepLimitScaleDefault),
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
	if (!Configured)
	{
		error_msg.appendSprintf("ParticleSystemData Not Configured, Please use GAS_CF_ConfigureSPHSystemData, From %s\n", DATANAME);
		return -1;
	}

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
	if (!Configured)
	{
		error_msg.appendSprintf("ParticleSystemData Not Configured, Please use GAS_CF_ConfigureSPHSystemData, From %s\n", DATANAME);
		return;
	}

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
	if (!Configured)
	{
		error_msg.appendSprintf("ParticleSystemData Not Configured, Please use GAS_CF_ConfigureSPHSystemData, From %s\n", DATANAME);
		return PARTICLE_INVALID;
	}

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
	if (!Configured)
	{
		error_msg.appendSprintf("ParticleSystemData Not Configured, Please use GAS_CF_ConfigureSPHSystemData, From %s\n", DATANAME);
		return;
	}

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
