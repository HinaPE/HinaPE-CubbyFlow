#ifndef HINAPE_GAS_HINA_CONFIGUREFORSPH_H
#define HINAPE_GAS_HINA_CONFIGUREFORSPH_H

#include "HinaHDKClassGenerator.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

NEW_HINA_DATA_CLASS(
		ConfigureForSPH,
		bool InitConfigure(SIM_Object *obj, SIM_Hina_ParticleFluidData *data);
		bool UpdateConfigure(SIM_Object *obj, SIM_Hina_ParticleFluidData *data, SIM_Time current_time, SIM_Time timestep);
)

#endif //HINAPE_GAS_HINA_CONFIGUREFORSPH_H
