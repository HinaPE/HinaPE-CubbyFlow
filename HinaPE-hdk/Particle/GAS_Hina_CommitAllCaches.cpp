#include "GAS_Hina_CommitAllCaches.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>
#include <Particle/SIM_Hina_SPHCaches.h>
#include <Particle/SIM_Hina_PCISPHCaches.h>

NEW_HINA_MICROSOLVER_IMPLEMENT(
		CommitAllCaches,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_CommitAllCaches::_init() {}
void GAS_Hina_CommitAllCaches::_makeEqual(const GAS_Hina_CommitAllCaches *src) {}
bool GAS_Hina_CommitAllCaches::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	CubbyFlow::ParticleSystemData3::VectorData *newPositions = nullptr;
	CubbyFlow::ParticleSystemData3::VectorData *newVelocities = nullptr;
	if (data->getFluidDataType() == 0) // "SPH"
	{
		SIM_Hina_SPHCaches *cache = SIM_DATA_GET(*data, SIM_Hina_SPHCaches::DATANAME, SIM_Hina_SPHCaches);
		newPositions = &cache->newPositions_Cache;
		newVelocities = &cache->newVelocities_Cache;
	} else if (data->getFluidDataType() == 1) // "PCISPH"
	{
		SIM_Hina_PCISPHCaches *cache = SIM_DATA_GET(*data, SIM_Hina_PCISPHCaches::DATANAME, SIM_Hina_PCISPHCaches);
		newPositions = &cache->newPositions_Cache;
		newVelocities = &cache->newVelocities_Cache;
	}
	if (!newPositions || !newVelocities)
		return false;

	size_t pt_size = data->pt_size();
	CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, pt_size, [&](size_t pt_idx)
	{
		data->velocity(pt_idx) = (*newVelocities)[pt_idx];
		data->position(pt_idx) = (*newPositions)[pt_idx];
	});
	data->sync_velocity(geo); // sync gdp
	data->sync_position(geo); // sync gdp

	return true;
}
