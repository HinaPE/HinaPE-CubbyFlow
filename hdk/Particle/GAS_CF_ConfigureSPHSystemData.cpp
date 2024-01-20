#include "GAS_CF_ConfigureSPHSystemData.h"

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

#include <Particle/SIM_CF_SPHSystemData.h>

bool GAS_CF_ConfigureSPHSystemData::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_ConfigureSPHSystemData::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_ConfigureSPHSystemData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_ConfigureSPHSystemData::DATANAME = "CF_ConfigureSPHSystemData";
const SIM_DopDescription *GAS_CF_ConfigureSPHSystemData::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_configure_sphsystemdata",
								   "CF Configure SPHSystemData",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_ConfigureSPHSystemData::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_SPHSystemData *data = SIM_DATA_GET(*obj, SIM_CF_SPHSystemData::DATANAME, SIM_CF_SPHSystemData);
	if (!data)
	{
		error_msg.appendSprintf("CF_ParticleSystemData Is Null, From %s\n", DATANAME);
		return false;
	}

	if (data->Configured)
		return true;

	SIM_GeometryCopy *geo_exist = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy);
	if (geo_exist)
	{
		error_msg.appendSprintf("Geometry Already Exist, From %s\n", DATANAME);
		return false;
	}

	SIM_GeometryCopy *geo = SIM_DATA_CREATE(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy,
											SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE);
	if (!geo)
	{
		error_msg.appendSprintf("Geometry Is Null, From %s\n", DATANAME);
		return false;
	}

	// Config Geometry Sheet
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

		GA_RWAttributeRef velref = gdp.addFloatTuple(GA_ATTRIB_POINT, gdp.getStdAttributeName(GEO_ATTRIBUTE_VELOCITY), 3, GA_Defaults(0));
		velref.setTypeInfo(GA_TYPE_VECTOR);
		GA_RWAttributeRef forceref = gdp.addFloatTuple(GA_ATTRIB_POINT, SIM_CF_SPHSystemData::FORCE_ATTRIBUTE_NAME, 3, GA_Defaults(0));
		forceref.setTypeInfo(GA_TYPE_VECTOR);
		GA_RWAttributeRef massref = gdp.addFloatTuple(GA_ATTRIB_POINT, gdp.getStdAttributeName(GEO_ATTRIBUTE_MASS), 1, GA_Defaults(1));
		massref.setTypeInfo(GA_TYPE_VOID);
		GA_RWAttributeRef densityref = gdp.addFloatTuple(GA_ATTRIB_POINT, SIM_CF_SPHSystemData::DENSITY_ATTRIBUTE_NAME, 1, GA_Defaults(0));
		densityref.setTypeInfo(GA_TYPE_VOID);
		GA_RWAttributeRef pressureref = gdp.addFloatTuple(GA_ATTRIB_POINT, SIM_CF_SPHSystemData::PRESSURE_ATTRIBUTE_NAME, 1, GA_Defaults(0));
		pressureref.setTypeInfo(GA_TYPE_VOID);
	}

	data->Configured = true;

	return true;
}
