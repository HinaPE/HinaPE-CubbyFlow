#ifndef HINAPE_GAS_HINA_VOLUMEPARTICLEEMITTTER_H
#define HINAPE_GAS_HINA_VOLUMEPARTICLEEMITTTER_H

#include "HinaHDKClassGenerator.h"

#include "Core/Particle/SPHSystemData.hpp"

NEW_HINA_MICRPSOLVER_CLASS(
		VolumeParticleEmittter,
		CubbyFlow::SPHSystemData3Ptr InnerPtr;
				bool _init_runtime();
)

#endif //HINAPE_GAS_HINA_VOLUMEPARTICLEEMITTTER_H
