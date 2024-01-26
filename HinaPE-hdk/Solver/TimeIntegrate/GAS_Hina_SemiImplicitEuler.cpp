#include "GAS_Hina_SemiImplicitEuler.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		SemiImplicitEuler,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_SemiImplicitEuler::_init() {}
void GAS_Hina_SemiImplicitEuler::_makeEqual(const GAS_Hina_SemiImplicitEuler *src) {}
bool GAS_Hina_SemiImplicitEuler::_solve(SIM_Engine &, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	double dt = timestep;
	double mass = data->InnerPtr->Mass();
	size_t pt_size = data->pt_size();
	CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, pt_size, [&](size_t pt_idx)
	{
		data->velocity(pt_idx) += dt * data->force(pt_idx) / mass;
		data->position(pt_idx) += dt * data->velocity(pt_idx);
	});
	data->sync_position(geo); // sync gdp
	data->sync_velocity(geo); // sync gdp

	std::cout << "Solved" << "time:" << time << "timestep" << timestep << std::endl;

	return true;
}
