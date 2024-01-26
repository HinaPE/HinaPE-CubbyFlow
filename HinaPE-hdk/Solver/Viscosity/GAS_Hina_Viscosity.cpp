#include "GAS_Hina_Viscosity.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

#include "Core/Particle/SPHKernels.hpp"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		Viscosity,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_Viscosity::_init() {}
void GAS_Hina_Viscosity::_makeEqual(const GAS_Hina_Viscosity *src) {}
bool GAS_Hina_Viscosity::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	CubbyFlow::ArrayView1<CubbyFlow::Vector3D> x = data->InnerPtr->Positions();
	CubbyFlow::ArrayView1<CubbyFlow::Vector3D> v = data->InnerPtr->Velocities();
	CubbyFlow::ArrayView1<double> d = data->InnerPtr->Densities();
	CubbyFlow::ArrayView1<CubbyFlow::Vector3D> f = data->InnerPtr->Forces();

	const double viscosity = data->getViscosityCoefficient();
	const double mass_squared = data->InnerPtr->Mass() * data->InnerPtr->Mass();
	const CubbyFlow::SPHSpikyKernel3 kernel{data->InnerPtr->KernelRadius()};

	size_t p_size = data->InnerPtr->NumberOfParticles();
	CubbyFlow::ParallelFor(CubbyFlow::ZERO_SIZE, p_size, [&](size_t i)
	{
		const auto &neighbors = data->InnerPtr->NeighborLists()[i];
		for (size_t j: neighbors)
		{
			const double dist = x[i].DistanceTo(x[j]);

			f[i] += viscosity * mass_squared * (v[j] - v[i]) /
					d[j] * kernel.SecondDerivative(dist);
		}
	});
	data->sync_force(geo);

	return true;
}
