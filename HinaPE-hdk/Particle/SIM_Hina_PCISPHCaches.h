#ifndef HINAPE_CUBBYFLOW_SIM_HINA_PCISPHCACHES_H
#define HINAPE_CUBBYFLOW_SIM_HINA_PCISPHCACHES_H

#include "HinaHDKClassGenerator.h"
#include "Core/Solver/Particle/PCISPH/PCISPHSolver3.hpp"

NEW_HINA_DATA_CLASS(
		PCISPHCaches,
		CubbyFlow::ParticleSystemData3::VectorData newPositions_Cache;
		CubbyFlow::ParticleSystemData3::VectorData newVelocities_Cache;
		CubbyFlow::ParticleSystemData3::VectorData tempPositions_Cache;
		CubbyFlow::ParticleSystemData3::VectorData tempVelocities_Cache;
		CubbyFlow::ParticleSystemData3::VectorData pressureForces_Cache;
		CubbyFlow::ParticleSystemData3::ScalarData densityErrors_Cache;
)

#endif //HINAPE_CUBBYFLOW_SIM_HINA_PCISPHCACHES_H
