#include "GAS_Hina_ClearForce.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		ClearForce,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_ClearForce::_init() {}
void GAS_Hina_ClearForce::_makeEqual(const GAS_Hina_ClearForce *src) {}
bool GAS_Hina_ClearForce::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	size_t pt_size = data->pt_size();
	CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, pt_size, [&](size_t pt_idx)
	{
		data->force(pt_idx) = CubbyFlow::Vector3D::MakeZero();
	});
	data->sync_force(geo); // sync gdp

	return true;
}
