#include "GAS_CF_PointEmitter.h"

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

#include "Particle/SIM_CF_ParticleSystemData.h"
#include "Particle/SIM_CF_SPHSystemData.h"

bool GAS_CF_PointEmitter::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}
void GAS_CF_PointEmitter::initializeSubclass()
{
	SIM_Data::initializeSubclass();

	/// Implement Initializations of Your Custom Fields
	UT_Vector3 Origin = getOrigin();
	UT_Vector3 Direction = getDirection();
	fpreal Speed = getSpeed();
	fpreal SpreadAngleInDegrees = getSpreadAngleInDegrees();
	fpreal MaxNumberOfNewParticlesPerSecond = getMaxNumberOfNewParticlesPerSecond();
	fpreal MaxNumberOfParticles = getMaxNumberOfParticles();
	fpreal RandomSeed = getRandomSeed();

	this->InnerPtr = CubbyFlow::PointParticleEmitter3::GetBuilder()
			.WithOrigin({Origin.x(), Origin.y(), Origin.z()})
			.WithDirection({Direction.x(), Direction.y(), Direction.z()})
			.WithSpeed(Speed)
			.WithSpreadAngleInDegrees(SpreadAngleInDegrees)
			.WithMaxNumberOfNewParticlesPerSecond(MaxNumberOfNewParticlesPerSecond)
			.WithMaxNumberOfParticles(MaxNumberOfParticles)
			.WithRandomSeed(RandomSeed)
			.MakeShared();
}

void GAS_CF_PointEmitter::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
	const GAS_CF_PointEmitter *src = SIM_DATA_CASTCONST(source, GAS_CF_PointEmitter);

	/// Implement Equal Operator of Your Custom Fields
	this->InnerPtr = src->InnerPtr;
}

const char *GAS_CF_PointEmitter::DATANAME = "CF_PointEmitter";
const SIM_DopDescription *GAS_CF_PointEmitter::getDopDescription()
{
	static PRM_Name Origin("Origin", "Origin");
	static std::array<PRM_Default, 3> OriginDefault{0, 0, 0};

	static PRM_Name Direction("Direction", "Direction");
	static std::array<PRM_Default, 3> DirectionDefault{0, 1, 0};

	static PRM_Name Speed("Speed", "Speed");
	static PRM_Default SpeedDefault(1);

	static PRM_Name SpreadAngleInDegrees("SpreadAngleInDegrees", "SpreadAngleInDegrees");
	static PRM_Default SpreadAngleInDegreesDefault(90);

	static PRM_Name MaxNumberOfNewParticlesPerSecond("MaxNumberOfNewParticlesPerSecond", "MaxNumberOfNewParticlesPerSecond");
	static PRM_Default MaxNumberOfNewParticlesPerSecondDefault(1000);

	static PRM_Name MaxNumberOfParticles("MaxNumberOfParticles", "MaxNumberOfParticles");
	static PRM_Default MaxNumberOfParticlesDefault(10e8);

	static PRM_Name RandomSeed("RandomSeed", "RandomSeed");
	static PRM_Default RandomSeedDefault(0);

	static std::array<PRM_Template, 8> PRMS{
			PRM_Template(PRM_FLT_J, 3, &Origin, OriginDefault.data()),
			PRM_Template(PRM_FLT_J, 3, &Direction, DirectionDefault.data()),
			PRM_Template(PRM_FLT, 1, &Speed, &SpeedDefault),
			PRM_Template(PRM_FLT, 1, &SpreadAngleInDegrees, &SpreadAngleInDegreesDefault),
			PRM_Template(PRM_FLT, 1, &MaxNumberOfNewParticlesPerSecond, &MaxNumberOfNewParticlesPerSecondDefault),
			PRM_Template(PRM_FLT, 1, &MaxNumberOfParticles, &MaxNumberOfParticlesDefault),
			PRM_Template(PRM_FLT, 1, &RandomSeed, &RandomSeedDefault),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_point_emitter",
								   "CF Point Emitter",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_PointEmitter::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_GeometryCopy *geo = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy);
	if (!geo)
	{
		error_msg.appendSprintf("Geometry Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_ParticleSystemData *psdata = SIM_DATA_GET(*obj, SIM_CF_ParticleSystemData::DATANAME, SIM_CF_ParticleSystemData);
	SIM_CF_SPHSystemData *sphdata = SIM_DATA_GET(*obj, SIM_CF_SPHSystemData::DATANAME, SIM_CF_SPHSystemData);
	if (!psdata && !sphdata)
	{
		error_msg.appendSprintf("No Valid Target Data, From %s\n", DATANAME);
		return false;
	}

	if (psdata)
	{
		if (!psdata->Configured)
		{
			error_msg.appendSprintf("ParticleSystemData Not Configured Yet, From %s\n", DATANAME);
			return false;
		}

		if (!psdata->InnerPtr)
		{
			error_msg.appendSprintf("ParticleSystemData InnerPtr is nullptr, From %s\n", DATANAME);
			return false;
		}

		this->InnerPtr->SetTarget(psdata->InnerPtr);
		this->InnerPtr->Update(time, timestep);
	}

	if (sphdata)
	{
		if (!sphdata->Configured)
		{
			error_msg.appendSprintf("SPHSystemData Not Configured Yet, From %s\n", DATANAME);
			return false;
		}

		if (!sphdata->InnerPtr)
		{
			error_msg.appendSprintf("SPHSystemData InnerPtr is nullptr, From %s\n", DATANAME);
			return false;
		}

		this->InnerPtr->SetTarget(sphdata->InnerPtr);
		this->InnerPtr->Update(time, timestep);
	}

	return true;
}
