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


	static std::array<PRM_Template, 15> PRMS{
			PRM_Template(PRM_FLT, 1, &MaxDensityErrorRatio, &MaxDensityErrorRatioDefault),
			PRM_Template(PRM_FLT, 1, &MaxNumberOfIterations, &MaxNumberOfIterationsDefault),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_pcisph_system_data",
								   "CF PCISPH System Data",
								   DATANAME,
								   classname(),
								   PRMS.data());
	return &DESC;
}
