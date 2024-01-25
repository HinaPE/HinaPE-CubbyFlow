#include "GAS_Hina_SemiImplicitEuler.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICRPSOLVER_IMPLEMENT(
		SemiImplicitEuler,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_SemiImplicitEuler::_init() {}
void GAS_Hina_SemiImplicitEuler::_makeEqual(const GAS_Hina_SemiImplicitEuler *src) {}
bool GAS_Hina_SemiImplicitEuler::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg)
{
	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	double dt = timestep;
	double mass = data->InnerPtr->Mass();
	size_t pt_size = data->pt_size();
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		data->runtime_init_handles(gdp);

		CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, pt_size, [&](size_t pt_idx)
		{
			data->velocity(pt_idx) += dt * data->force(pt_idx) / mass;
			data->position(pt_idx) += dt * data->velocity(pt_idx);
			data->set_gdp_velocity(pt_idx, AS_UTVector3D(data->velocity(pt_idx)));
			data->set_gdp_position(pt_idx, AS_UTVector3D(data->position(pt_idx)));
		}, CubbyFlow::ExecutionPolicy::Serial);
	}

	return true;
}