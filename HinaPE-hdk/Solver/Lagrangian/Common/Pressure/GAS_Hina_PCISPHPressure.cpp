#include "GAS_Hina_PCISPHPressure.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>
#include <Particle/SIM_Hina_SPHCaches.h>
#include <Particle/SIM_Hina_PCISPHCaches.h>
#include <Collider/SIM_Hina_RigidBodyCollider.h>

#include "Core/Particle/SPHKernels.hpp"
#include "Core/Utils/PhysicsHelpers.hpp"
#include "Core/PointGenerator/BccLatticePointGenerator.hpp"

NEW_HINA_MICROSOLVER_IMPLEMENT(
		PCISPHPressure,
		false,
		ACTIVATE_GAS_GEOMETRY
)

void GAS_Hina_PCISPHPressure::_init() {}
void GAS_Hina_PCISPHPressure::_makeEqual(const GAS_Hina_PCISPHPressure *src) {}
bool GAS_Hina_PCISPHPressure::_solve(SIM_Engine &, SIM_Object *obj, SIM_Time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
	CHECK_NULL(data)
	CHECK_CONFIGURED(data)
	SIM_GeometryCopy *geo = getOrCreateGeometry(obj, GAS_NAME_GEOMETRY);
	CHECK_NULL(geo)

	if (data->getFluidDataType() != 1) // "PCISPH"
		return false;
	SIM_Hina_PCISPHCaches *cache = SIM_DATA_GET(*data, SIM_Hina_PCISPHCaches::DATANAME, SIM_Hina_PCISPHCaches);

	SIM_ObjectArray affectors;
	obj->getAffectors(affectors, "SIM_RelationshipCollide");
	exint num_affectors = affectors.entries();

	{
		using namespace CubbyFlow;
		SPHSystemData3Ptr particles = data->InnerPtr;
		const size_t numberOfParticles = particles->NumberOfParticles();
		double timeIntervalInSeconds = timestep;

		double delta;
		{
			// Compute Delta
			const double kernelRadius = particles->KernelRadius();

			Array1<Vector3D> points;
			const BccLatticePointGenerator pointsGenerator;
			const Vector3D origin;
			BoundingBox3D sampleBound{origin, origin};
			sampleBound.Expand(1.5 * kernelRadius);

			pointsGenerator.Generate(sampleBound, particles->TargetSpacing(), &points);

			const SPHSpikyKernel3 kernel{kernelRadius};

			double denom = 0;
			Vector3D denom1;
			double denom2 = 0;

			for (const auto &point: points)
			{
				const double distanceSquared = point.LengthSquared();

				if (distanceSquared < kernelRadius * kernelRadius)
				{
					const double distance = std::sqrt(distanceSquared);
					Vector3D direction =
							(distance > 0.0) ? point / distance : Vector3D{};

					// grad(Wij)
					Vector3D gradWij = kernel.Gradient(distance, direction);
					denom1 += gradWij;
					denom2 += gradWij.Dot(gradWij);
				}
			}

			denom += -denom1.Dot(denom1) - denom2;

			double Beta = 2.0 * Square(particles->Mass() * timeIntervalInSeconds /
									   particles->TargetDensity());

			delta = (std::fabs(denom) > 0.0)
					? -1 / (Beta * denom)
					: 0;
		}
		const double targetDensity = particles->TargetDensity();
		const double mass = particles->Mass();

		ArrayView1<double> p = particles->Pressures();
		ArrayView1<double> d = particles->Densities();
		ArrayView1<Vector3D> x = particles->Positions();
		ArrayView1<Vector3D> v = particles->Velocities();
		ArrayView1<Vector3D> f = particles->Forces();

		// Predicted density ds
		Array1<double> ds(numberOfParticles, 0.0);

		SPHStdKernel3 kernel{particles->KernelRadius()};

		auto &m_pressureForces = cache->pressureForces_Cache;
		auto &m_densityErrors = cache->densityErrors_Cache;
		auto &m_tempVelocities = cache->tempVelocities_Cache;
		auto &m_tempPositions = cache->tempPositions_Cache;
		m_pressureForces.Resize(numberOfParticles);
		m_densityErrors.Resize(numberOfParticles);
		m_tempVelocities.Resize(numberOfParticles);
		m_tempPositions.Resize(numberOfParticles);

		// Initialize buffers
		ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i)
		{
			p[i] = 0.0;
			m_pressureForces[i] = Vector3D{};
			m_densityErrors[i] = 0.0;
			ds[i] = d[i];
		});

		unsigned int maxNumIter = 0;
		double maxDensityError = 0.0;
		double densityErrorRatio = 0.0;

		int m_maxNumberOfIterations = 5;
		double NegativePressureScale = data->getNegativePressureScale();
		double MaxDensityErrorRatio = data->getMaxDensityErrorRatio();

		for (unsigned int k = 0; k < m_maxNumberOfIterations; ++k)
		{
			// Predict velocity and position
			ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i)
			{
				m_tempVelocities[i] = v[i] + timeIntervalInSeconds / mass *
											 (f[i] + m_pressureForces[i]);
				m_tempPositions[i] =
						x[i] + timeIntervalInSeconds * m_tempVelocities[i];
			});

			// Resolve collisions
			for (exint i = 0; i < num_affectors; ++i)
			{
				SIM_Object *affector = affectors(i);
//		if (!affector->getName().equal(obj->getName())) # Also Collider With Self Fluid Domain Boundary
				{
					SIM_Hina_RigidBodyCollider *collider_data = SIM_DATA_GET(*affector, SIM_Hina_RigidBodyCollider::DATANAME, SIM_Hina_RigidBodyCollider);
					if (!collider_data)
						continue;

					if (!collider_data->Configured)
					{
						error_msg.appendSprintf("Affector %s Collider is not Configured, From %s\n", affector->getName().toStdString().c_str(), DATANAME);
						return false;
					}

					{
						using namespace CubbyFlow;
						auto &m_particleSystemData = data->InnerPtr;
						auto &m_collider = collider_data->InnerPtr;
						auto m_restitutionCoefficient = data->getRestitutionCoefficient();
						auto &newPositions = m_tempPositions;
						auto &newVelocities = m_tempVelocities;

						const double radius = m_particleSystemData->Radius();

						ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i)
						{
							m_collider->ResolveCollision(radius, m_restitutionCoefficient,
														 &newPositions[i], &newVelocities[i]);
						}, ExecutionPolicy::Serial); // TODO: make parallel
					}
				}
			}

			// Compute pressure from density error
			ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i)
			{
				double weightSum = 0.0;
				const auto &neighbors = particles->NeighborLists()[i];

				for (size_t j: neighbors)
				{
					const double dist =
							m_tempPositions[j].DistanceTo(m_tempPositions[i]);
					weightSum += kernel(dist);
				}
				weightSum += kernel(0);

				const double density = mass * weightSum;
				double densityError = (density - targetDensity);
				double pressure = delta * densityError;

				if (pressure < 0.0)
				{
					pressure *= NegativePressureScale;
					densityError *= NegativePressureScale;
				}

				p[i] += pressure;
				ds[i] = density;
				m_densityErrors[i] = densityError;
			});

			// Compute pressure gradient force
			m_pressureForces.Fill(Vector3D{});
			const ConstArrayView1<Vector3D> &positions = x;
			const ConstArrayView1<double> &densities = ds;
			const ConstArrayView1<double> &pressures = p;
			ArrayView1<Vector3D> pressureForces = m_pressureForces;
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

			// Compute max density error
			maxDensityError = 0.0;
			for (size_t i = 0; i < numberOfParticles; ++i)
			{
				maxDensityError = AbsMax(maxDensityError, m_densityErrors[i]);
			}

			densityErrorRatio = maxDensityError / targetDensity;
			maxNumIter = k + 1;

			if (std::fabs(densityErrorRatio) < MaxDensityErrorRatio)
			{
				break;
			}
		}

		ParallelFor(ZERO_SIZE, numberOfParticles,
					[&](size_t i) { f[i] += m_pressureForces[i]; });
	}

	return true;
}
