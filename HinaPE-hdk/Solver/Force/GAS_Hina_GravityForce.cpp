#include "GAS_Hina_GravityForce.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		GravityForce,
		false,
		ACTIVATE_GAS_GEOMETRY \
        NEW_FLOAT_VECTOR_PARAMETER(Gravity, 3, 0, -9.8, 0) \
)

void GAS_Hina_GravityForce::_init() {}
void GAS_Hina_GravityForce::_makeEqual(const GAS_Hina_GravityForce *src) {}
bool GAS_Hina_GravityForce::_solve(SIM_Engine &, SIM_Object *obj, SIM_Time, SIM_Time)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	CubbyFlow::Vector3D Gravity = AS_CFVector3D(getGravityD());
	double mass = data->InnerPtr->Mass();
	size_t pt_size = data->pt_size();
	CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, pt_size, [&](size_t pt_idx)
	{
		data->force(pt_idx) += mass * Gravity;
	});
	data->sync_force(geo); // sync gdp

	return true;
}
