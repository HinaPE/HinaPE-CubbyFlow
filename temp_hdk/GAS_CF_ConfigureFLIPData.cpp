#include "GAS_CF_ConfigureFLIPData.h"

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

#include <SIM_CF_FLIPData.h>

bool GAS_CF_ConfigureFLIPData::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_ConfigureFLIPData::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_ConfigureFLIPData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_ConfigureFLIPData::DATANAME = "CF_ConfigureFLIPData";
const SIM_DopDescription *GAS_CF_ConfigureFLIPData::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_configure_FLIPdata",
								   "CF Configure FLIP Data",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_ConfigureFLIPData::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_FLIPData *data = SIM_DATA_GET(*obj, SIM_CF_FLIPData::DATANAME, SIM_CF_FLIPData);
	if (!data)
	{
		error_msg.appendSprintf("SIM_CF_FLIPData Is Null, From %s\n", DATANAME);
		return false;
	}

	if (data->Configured)
		return true;

	return true;
}
