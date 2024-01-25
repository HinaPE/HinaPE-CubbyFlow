#include "GAS_Hina_VolumeParticleEmittter.h"

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		VolumeParticleEmittter,
		false,
		ACTIVATE_GAS_GEOMETRY \
)

void GAS_Hina_VolumeParticleEmittter::_init()
{
	this->InnerPtr = nullptr;
}
void GAS_Hina_VolumeParticleEmittter::_makeEqual(const GAS_Hina_VolumeParticleEmittter *src)
{
	this->InnerPtr = src->InnerPtr;
}
bool GAS_Hina_VolumeParticleEmittter::_init_runtime()
{
//	if ()
	return true;
}
bool GAS_Hina_VolumeParticleEmittter::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	CHECK_NULL(obj)
	_init_runtime();
	SIM_GeometryCopy *geo = getGeometryCopy(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

	}

	return true;
}
