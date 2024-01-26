#ifndef HINAPE_CUBBYFLOW_SIM_HINA_SPHCACHES_H
#define HINAPE_CUBBYFLOW_SIM_HINA_SPHCACHES_H

#include "HinaHDKClassGenerator.h"
#include "Core/Solver/Particle/SPH/SPHSolver3.hpp"

NEW_HINA_DATA_CLASS(
		SPHCaches,
		CubbyFlow::ParticleSystemData3::VectorData newPositions_Cache;
		CubbyFlow::ParticleSystemData3::VectorData newVelocities_Cache;
)

#endif //HINAPE_CUBBYFLOW_SIM_HINA_SPHCACHES_H
