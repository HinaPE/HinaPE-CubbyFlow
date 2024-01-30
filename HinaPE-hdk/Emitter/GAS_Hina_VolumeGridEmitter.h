#ifndef HINAPE_GAS_HINA_VOLUMEGRIDEMITTER_H
#define HINAPE_GAS_HINA_VOLUMEGRIDEMITTER_H

#include "HinaHDKClassGenerator.h"
#include "Core/Emitter/VolumeGridEmitter3.hpp"

NEW_HINA_MICRPSOLVER_CLASS(
		VolumeGridEmitter,
		CubbyFlow::VolumeGridEmitter3Ptr InnerPtr;
				CubbyFlow::Array1<CubbyFlow::Surface3Ptr> _AllSurfaces; // "_" meaas it is a temp cache

				NEW_GETSET_PARAMETER(IsOneShot, GETSET_DATA_FUNCS_B)

				void _search_and_add_all_box();
				void _search_and_add_all_sphere();
				void _search_and_add_all_plane();
				void _search_and_add_geometry();
)

#endif //HINAPE_GAS_HINA_VOLUMEGRIDEMITTER_H
