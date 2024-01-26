#include "GAS_CF_ConfigurePCISPHSystemData.h"

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_GuideShared.h>
#include <SIM/SIM_ColliderLabel.h>
#include <SIM/SIM_ForceGravity.h>
#include <SIM/SIM_Time.h>
#include <SIM/SIM_Utils.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#include <Particle/PCISPHSystemData/SIM_CF_PCISPHSystemData.h>

bool GAS_CF_ConfigurePCISPHSystemData::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	// Remember to call the parent solver
	if (!GAS_CF_ConfigureSPHSystemData::solveGasSubclass(engine, obj,time,timestep))
		return false;

	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_ConfigurePCISPHSystemData::initializeSubclass()
{
	GAS_CF_ConfigureSPHSystemData::initializeSubclass();
}

void GAS_CF_ConfigurePCISPHSystemData::makeEqualSubclass(const SIM_Data *source)
{
	GAS_CF_ConfigureSPHSystemData::makeEqualSubclass(source);
}

const char *GAS_CF_ConfigurePCISPHSystemData::DATANAME = "CF_ConfigurePCISPHSystemData"; // TODO: maybe this should inherit the parent value...
const SIM_DopDescription *GAS_CF_ConfigurePCISPHSystemData::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_configure_pcisphsystemdata",
								   "CF Configure PCISPHSPHSystemData",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_ConfigurePCISPHSystemData::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	// We don't need to recheck the nullptr or something, because the parent solver already did this.



	// nothing to do, because PCISPH is so simple......

	return true;
}
