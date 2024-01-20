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

#include <GEO/GEO_PrimPoly.h>

void SIM_CF_ParticleSystemData::initializeSubclass()
{
	SIM_Data::initializeSubclass(); // Always remember to call this parent implementation

	/// Implement Initializations of Your Custom Fields
	Configured = false;
	InnerPtr = std::make_shared<CubbyFlow::ParticleSystemData3>();
	scalar_idx_offset = InnerPtr->AddScalarData();
	scalar_idx_state = InnerPtr->AddScalarData();

// 					[!!! IMPORTANT !!!]
// 		[!!! NEVER CALL GET_SET FUNCTION HERE !!!]
// 					[!!! IMPORTANT !!!]
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
const char *SIM_CF_ParticleSystemData::CL_PT_IDX_ATTRIBUTE_NAME = "CL_PT_IDX";
const char *SIM_CF_ParticleSystemData::FORCE_ATTRIBUTE_NAME = "force";
const SIM_DopDescription *SIM_CF_ParticleSystemData::GetDescription()
{
	static PRM_Name ParticlesDomain("ParticlesDomain", "ParticlesDomain");
	static std::array<PRM_Default, 3> ParticlesDomainDefault{4, 4, 4};

	static PRM_Name ParticleRadius("ParticleRadius", "ParticleRadius");
	static PRM_Default ParticleRadiusDefault{1e-3};

	static PRM_Name TargetSpacing("TargetSpacing", "TargetSpacing");
	static PRM_Default TargetSpacingDefault{.1};

	static std::array<PRM_Template, 4> PRMS{
			PRM_Template(PRM_FLT, 3, &ParticlesDomain, ParticlesDomainDefault.data()),
			PRM_Template(PRM_FLT, 1, &ParticleRadius, &ParticleRadiusDefault),
			PRM_Template(PRM_FLT, 1, &TargetSpacing, &TargetSpacingDefault),
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
								   "cf_particle_system_data",
								   "CF Particle System Data",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setGuideTemplates(PRMS_GUIDE.data());
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

	InnerPtr->ScalarDataAt(scalar_idx_state)[index] = state;
}

SIM_Guide *SIM_CF_ParticleSystemData::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}

void SIM_CF_ParticleSystemData::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
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
	UT_Vector3 Extent = getParticlesDomain();

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
