#ifndef HINAPE_GAS_HINA_VOLUMEPARTICLEEMITTTER_H
#define HINAPE_GAS_HINA_VOLUMEPARTICLEEMITTTER_H

#include "HinaHDKClassGenerator.h"

#include "Core/Emitter/VolumeParticleEmitter3.hpp"

NEW_HINA_MICRPSOLVER_CLASS(
		VolumeParticleEmittter,
		CubbyFlow::VolumeParticleEmitter3Ptr InnerPtr;
				CubbyFlow::Array1<CubbyFlow::Surface3Ptr> _AllSurfaces; // "_" meaas it is a temp cache

				void search_and_add_all_box();
				void search_and_add_all_sphere();
				void search_and_add_all_plane();
				void search_and_add_geometry();
)

#endif //HINAPE_GAS_HINA_VOLUMEPARTICLEEMITTTER_H
