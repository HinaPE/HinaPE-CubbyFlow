#include "SIM_CF_PCISPHSystemData.h"

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

void SIM_CF_PCISPHSystemData::initializeSubclass()
{
	SIM_CF_SPHSystemData::initializeSubclass();

	/// Implement Initializations of Your Custom Fields
	tempPositions_Cache.Clear();
	tempVelocities_Cache.Clear();
	pressureForces_Cache.Clear();
	densityErrors_Cache.Clear();

// 					[!!! IMPORTANT !!!]
// 		[!!! NEVER CALL GET_SET FUNCTION HERE !!!]
// 					[!!! IMPORTANT !!!]
}

void SIM_CF_PCISPHSystemData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_CF_SPHSystemData::makeEqualSubclass(source);
	const SIM_CF_PCISPHSystemData *src = SIM_DATA_CASTCONST(source, SIM_CF_PCISPHSystemData);

	/// Implement Equal Operator of Your Custom Fields
	this->tempPositions_Cache.Clear(); // We don't need this cache
	this->tempVelocities_Cache.Clear(); // We don't need this cache
	this->pressureForces_Cache.Clear(); // We don't need this cache
	this->densityErrors_Cache.Clear(); // We don't need this cache
}

const char *SIM_CF_PCISPHSystemData::DATANAME = "CF_PCISPHSystemData";
const SIM_DopDescription *SIM_CF_PCISPHSystemData::GetDescription()
{
	/// PCISPHSolver Parameters
	static PRM_Name MaxDensityErrorRatio("MaxDensityErrorRatio", "MaxDensityErrorRatio");
	static PRM_Default MaxDensityErrorRatioDefault{0.01};

	static PRM_Name MaxNumberOfIterations("MaxNumberOfIterations", "MaxNumberOfIterations");
	static PRM_Default MaxNumberOfIterationsDefault{5};

	static std::vector<PRM_Template> PRMS;
	static std::vector<PRM_Template> PRMS_GUIDE;

	// Copy From Parent
	std::vector<PRM_Template> TempPRMs;
	const PRM_Template *parent_templates = SIM_CF_SPHSystemData::GetDescription()->getTemplates();
	TempPRMs.reserve(15);
	for (int i = 0; i < 12; ++i)
		TempPRMs.push_back(parent_templates[i]); // call copy operator=
	TempPRMs.emplace_back(PRM_FLT, 1, &MaxDensityErrorRatio, &MaxDensityErrorRatioDefault);
	TempPRMs.emplace_back(PRM_FLT, 1, &MaxNumberOfIterations, &MaxNumberOfIterationsDefault);
	TempPRMs.emplace_back();
	PRMS = TempPRMs;

	std::vector<PRM_Template> TempGuidePRMs;
	const PRM_Template *parent_guide_templates = SIM_CF_SPHSystemData::GetDescription()->getGuideTemplates();
	TempGuidePRMs.reserve(3);
	for (int i = 0; i < 2; ++i)
		TempGuidePRMs.push_back(parent_guide_templates[i]); // call copy operator=
	TempGuidePRMs.emplace_back();
	PRMS_GUIDE = TempGuidePRMs;

	static SIM_DopDescription DESC(true,
								   "cf_pcisph_system_data",
								   "CF PCISPH System Data",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setGuideTemplates(PRMS_GUIDE.data());
	return &DESC;
}

SIM_Guide *SIM_CF_PCISPHSystemData::createGuideObjectSubclass() const
{
	return SIM_CF_SPHSystemData::createGuideObjectSubclass();
}

void SIM_CF_PCISPHSystemData::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	// Don't Call SIM_CF_SPHSystemData::buildGuideGeometrySubclass()
	// Write Unique Guide Geometry Subclass For Every Child Class.

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
