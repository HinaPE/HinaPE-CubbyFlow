#include "GAS_CF_ConfigureParticleSystemData.h"

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

#include <Particle/SIM_CF_ParticleSystemData.h>

bool GAS_CF_ConfigureParticleSystemData::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_ConfigureParticleSystemData::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_ConfigureParticleSystemData::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_ConfigureParticleSystemData::DATANAME = "CF_ConfigureParticleSystemData";
const SIM_DopDescription *GAS_CF_ConfigureParticleSystemData::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_configure_particlesystemdata",
								   "CF Configure ParticleSystemData",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_ConfigureParticleSystemData::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_ParticleSystemData *data = SIM_DATA_GET(*obj, SIM_CF_ParticleSystemData::DATANAME, SIM_CF_ParticleSystemData);
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

	// Configure Geometry Sheet
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

		// add vel attribute
		GA_RWAttributeRef velref = gdp.addFloatTuple(GA_ATTRIB_POINT, gdp.getStdAttributeName(GEO_ATTRIBUTE_VELOCITY), 3, GA_Defaults(0));
		velref.setTypeInfo(GA_TYPE_VECTOR);
		GA_RWAttributeRef forceref = gdp.addFloatTuple(GA_ATTRIB_POINT, SIM_CF_ParticleSystemData::FORCE_ATTRIBUTE_NAME, 3, GA_Defaults(0));
		forceref.setTypeInfo(GA_TYPE_VECTOR);
		GA_RWAttributeRef massref = gdp.addFloatTuple(GA_ATTRIB_POINT, gdp.getStdAttributeName(GEO_ATTRIBUTE_MASS), 1, GA_Defaults(1));
		massref.setTypeInfo(GA_TYPE_VOID);
		GA_RWAttributeRef CL_PT_IDXref = gdp.addIntTuple(GA_ATTRIB_POINT, SIM_CF_ParticleSystemData::CL_PT_IDX_ATTRIBUTE_NAME, 1, GA_Defaults(-1));
		CL_PT_IDXref.setTypeInfo(GA_TYPE_VOID);
	}

	data->Configured = true;

	return true;
}
