#include "GAS_CF_PCISPHPressureForceSolver.h"

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

#include <Particle/PCISPHSystemData/SIM_CF_PCISPHSystemData.h>
#include <Collider/SIM_CF_RigidBodyCollider.h>

#include "Core/Particle/SPHKernels.hpp"
#include "Core/Utils/PhysicsHelpers.hpp"
#include "Core/PointGenerator/BccLatticePointGenerator.hpp"

bool GAS_CF_PCISPHPressureForceSolver::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	UT_WorkBuffer error_msg;
	if (!Solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer()))
	{
		SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT);
		return false;
	}

	return true;
}

void GAS_CF_PCISPHPressureForceSolver::initializeSubclass()
{
	SIM_Data::initializeSubclass();
}

void GAS_CF_PCISPHPressureForceSolver::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Data::makeEqualSubclass(source);
}

const char *GAS_CF_PCISPHPressureForceSolver::DATANAME = "CF_PCISPHPressureForceSolver";
const SIM_DopDescription *GAS_CF_PCISPHPressureForceSolver::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_PCISPH_pressure_force_solver",
								   "CF PCISPH Pressure Force Solver",
								   DATANAME,
								   classname(),
								   PRMS.data());
//	DESC.setDefaultUniqueDataName(true);
	setGasDescription(DESC);
	return &DESC;
}

bool GAS_CF_PCISPHPressureForceSolver::Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	SIM_CF_PCISPHSystemData *pcisphdata = SIM_DATA_GET(*obj, SIM_CF_PCISPHSystemData::DATANAME, SIM_CF_PCISPHSystemData);
	if (!pcisphdata)
	{
		error_msg.appendSprintf("No Valid Target Data, From %s\n", DATANAME);
		return false;
	}

	SIM_GeometryCopy *geo = SIM_DATA_GET(*obj, SIM_GEOMETRY_DATANAME, SIM_GeometryCopy);
	if (!geo)
	{
		error_msg.appendSprintf("Geometry Is Null, From %s\n", DATANAME);
		return false;
	}

	if (!pcisphdata->Configured)
	{
		error_msg.appendSprintf("SIM_CF_PCISPHSystemData Not Configured Yet, From %s\n", DATANAME);
		return false;
	}

	if (!pcisphdata->InnerPtr)
	{
		error_msg.appendSprintf("SIM_CF_PCISPHSystemData InnerPtr is nullptr, From %s\n", DATANAME);
		return false;
	}

	SIM_ObjectArray affectors;
	obj->getAffectors(affectors, "SIM_RelationshipCollide");
	exint num_affectors = affectors.entries();

	{
		using namespace CubbyFlow;
		SPHSystemData3Ptr particles = pcisphdata->InnerPtr;
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

		auto &m_pressureForces = pcisphdata->pressureForces_Cache;
		auto &m_densityErrors = pcisphdata->densityErrors_Cache;
		auto &m_tempVelocities = pcisphdata->tempVelocities_Cache;
		auto &m_tempPositions = pcisphdata->tempPositions_Cache;
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
		double NegativePressureScale = pcisphdata->getNegativePressureScale();
		double MaxDensityErrorRatio = pcisphdata->getMaxDensityErrorRatio();

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
					SIM_CF_RigidBodyCollider *collider_data = SIM_DATA_GET(*affector, SIM_CF_RigidBodyCollider::DATANAME, SIM_CF_RigidBodyCollider);
					if (!collider_data)
						continue;

					if (!collider_data->Configured)
					{
						error_msg.appendSprintf("Affector %s Collider is not Configured, From %s\n", affector->getName().toStdString().c_str(), DATANAME);
						return false;
					}

					{
						using namespace CubbyFlow;
						auto &m_particleSystemData = pcisphdata->InnerPtr;
						auto &m_collider = collider_data->InnerPtr;
						auto m_restitutionCoefficient = pcisphdata->getRestitutionCoefficient();
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

	const CubbyFlow::ArrayView1<double> pressures = pcisphdata->InnerPtr->Pressures();
	const CubbyFlow::ArrayView1<CubbyFlow::Vector3D> f = pcisphdata->InnerPtr->Forces();
	// Sync Pressure Force in Geometry Sheet
	{
		SIM_GeometryAutoWriteLock lock(geo);
		GU_Detail &gdp = lock.getGdp();

		GA_RWHandleV3 gdp_handle_force = gdp.findPointAttribute(SIM_CF_SPHSystemData::FORCE_ATTRIBUTE_NAME);
		GA_ROHandleI gdp_handle_CL_PT_IDX = gdp.findPointAttribute(SIM_CF_SPHSystemData::CL_PT_IDX_ATTRIBUTE_NAME);
		GA_RWHandleF gdp_handle_pressure = gdp.findPointAttribute(SIM_CF_SPHSystemData::PRESSURE_ATTRIBUTE_NAME);

		GA_Offset pt_off;
		GA_FOR_ALL_PTOFF(&gdp, pt_off)
			{
				UT_Vector3 exist_force = gdp_handle_force.get(pt_off);
				int cl_index = gdp_handle_CL_PT_IDX.get(pt_off);
				UT_Vector3 pressure_force = UT_Vector3D{f[cl_index].x, f[cl_index].y, f[cl_index].z};
				gdp_handle_force.set(pt_off, pressure_force + exist_force);
				auto pressure = pressures[cl_index];
				gdp_handle_pressure.set(pt_off, pressure);
			}
	}

	return true;
}
