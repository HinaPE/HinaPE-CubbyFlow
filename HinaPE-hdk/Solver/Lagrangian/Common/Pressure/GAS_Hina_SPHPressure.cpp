#include "GAS_Hina_SPHPressure.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

#include "Core/Particle/SPHKernels.hpp"
#include "Core/Utils/PhysicsHelpers.hpp"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		SPHPressure,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_SPHPressure::_init() {}
void GAS_Hina_SPHPressure::_makeEqual(const GAS_Hina_SPHPressure *src) {}
bool GAS_Hina_SPHPressure::_solve(SIM_Engine &, SIM_Object *obj, SIM_Time, SIM_Time)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();
		data->runtime_init_handles(gdp);

		double m_speedOfSound = data->getSpeedOfSound();
		double m_eosExponent = data->getEosExponent();
		double m_negativePressureScale = data->getNegativePressureScale();

		using namespace CubbyFlow;
		auto &particles = data->InnerPtr;
		const size_t numberOfParticles = particles->NumberOfParticles();
		ArrayView1<double> d = particles->Densities();
		ArrayView1<double> p = particles->Pressures();


		// See Murnaghan-Tait equation of state from
		// https://en.wikipedia.org/wiki/Tait_equation
		const double targetDensity = particles->TargetDensity();
		const double eosScale = targetDensity * Square(m_speedOfSound);

		ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i)
		{
			p[i] = ComputePressureFromEos(d[i], targetDensity, eosScale,
										  m_eosExponent,
										  m_negativePressureScale);
		});
	}

	{
		// AccumulatePressureForce
		using namespace CubbyFlow;
		auto &particles = data->InnerPtr;
		const ArrayView1<Vector3D> positions = particles->Positions();
		const ArrayView1<double> densities = particles->Densities();
		const ArrayView1<double> pressures = particles->Pressures();
		ArrayView1<Vector3D> pressureForces = particles->Forces();

		const size_t numberOfParticles = particles->NumberOfParticles();

		const double massSquared = Square(particles->Mass());
		const SPHSpikyKernel3 kernel{particles->KernelRadius()};

		ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i)
		{
			const auto &neighbors = particles->NeighborLists()[i];
			for (size_t j: neighbors)
			{
				const double dist = positions[i].DistanceTo(positions[j]);
				if (dist > 0.0)
				{
					Vector3D dir = (positions[j] - positions[i]) / dist;
					pressureForces[i] -= massSquared *
										 (pressures[i] / (densities[i] * densities[i]) +
										  pressures[j] / (densities[j] * densities[j])) *
										 kernel.Gradient(dist, dir);
				}
			}
		});
	}
	data->sync_pressure(geo);
	data->sync_force(geo);

	return true;
}
