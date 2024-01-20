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

#include <GEO/GEO_PrimPoly.h>

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
const char *SIM_CF_SPHSystemData::CL_PT_IDX_ATTRIBUTE_NAME = "CL_PT_IDX";
const char *SIM_CF_SPHSystemData::FORCE_ATTRIBUTE_NAME = "force";
const char *SIM_CF_SPHSystemData::DENSITY_ATTRIBUTE_NAME = "density";
const char *SIM_CF_SPHSystemData::PRESSURE_ATTRIBUTE_NAME = "pressure";
const char *SIM_CF_SPHSystemData::NEIGHBOR_SUM_ATTRIBUTE_NAME = "neighbor_sum";
const SIM_DopDescription *SIM_CF_SPHSystemData::GetDescription()
{
	static PRM_Name FluidDomain("FluidDomain", "FluidDomain");
	static std::array<PRM_Default, 3> FluidDomainDefault{4, 4, 4};


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

	static std::array<PRM_Template, 15> PRMS{
			PRM_Template(PRM_FLT, 3, &FluidDomain, FluidDomainDefault.data()),
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

	static PRM_Name ShowGuideGeometry(SIM_NAME_SHOWGUIDE, "ShowGuideGeometry");

	static PRM_Name DomainColor("DomainColor", "DomainColor");
	static std::array<PRM_Default, 3> DomainColorDefault{.0156356, 0, .5};

	static std::array<PRM_Template, 3> PRMS_GUIDE{
			PRM_Template(PRM_TOGGLE, 1, &ShowGuideGeometry, PRMoneDefaults),
			PRM_Template(PRM_RGBA, 3, &DomainColor, DomainColorDefault.data()),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_sph_system_data",
								   "CF SPH System Data",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setGuideTemplates(PRMS_GUIDE.data());
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

	InnerPtr->ScalarDataAt(scalar_idx_state)[index] = state;
}

SIM_Guide *SIM_CF_SPHSystemData::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}

void SIM_CF_SPHSystemData::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	if (gdh.isNull())
		return;

	if (!getShowGuideGeometry(options))
		return;

	UT_Vector3 color = getDomainColor(options);

	GU_DetailHandleAutoWriteLock gdl(gdh);
	GU_Detail *gdp = gdl.getGdp();
	gdp->clearAndDestroy();

	UT_Vector3 Center = UT_Vector3(0.);
	UT_Vector3 Extent = getFluidDomain();

	std::array<UT_Vector3, 8> vertices{};
	for (int i = 0; i < 8; i++)
	{
		vertices[i] = UT_Vector3(
				Center.x() + Extent.x() * ((i & 1) ? 0.5 : -0.5),
				Center.y() + Extent.y() * ((i & 2) ? 0.5 : -0.5),
				Center.z() + Extent.z() * ((i & 4) ? 0.5 : -0.5)
		);
	}

	std::array<GA_Offset, 8> pt_off{};
	for (int i = 0; i < 8; i++)
	{
		pt_off[i] = gdp->appendPointOffset();
		gdp->setPos3(pt_off[i], vertices[i]);

		GA_RWHandleV3 gdp_handle_cd(gdp->addFloatTuple(GA_ATTRIB_POINT, "Cd", 3));
		gdp_handle_cd.set(pt_off[i], color);
	}

	static const int edges[12][2] = {
			{0, 1},
			{0, 4},
			{1, 3},
			{1, 5},
			{2, 0},
			{2, 3},
			{2, 6},
			{3, 7},
			{4, 5},
			{4, 6},
			{5, 7},
			{6, 7},
	};

	for (int i = 0; i < 12; i++)
	{
		GEO_PrimPoly *line = GEO_PrimPoly::build(gdp, 2, GU_POLY_OPEN);
		for (int j = 0; j < 2; j++)
			line->setVertexPoint(j, pt_off[edges[i][j]]);
	}
}
