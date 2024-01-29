#ifndef HINAPE_SIM_HINA_CONFIGUREFORSMOKE_H
#define HINAPE_SIM_HINA_CONFIGUREFORSMOKE_H

#include "HinaHDKClassGenerator.h"
#include <Grid/SIM_Hina_GridFluidData.h>

NEW_HINA_DATA_CLASS(
		ConfigureForSmoke,
		bool InitConfigure(SIM_Object *obj, SIM_Hina_GridFluidData *data);
		bool UpdateConfigure(SIM_Object *obj, SIM_Hina_GridFluidData *data, SIM_Time current_time, SIM_Time timestep);
)

#endif //HINAPE_SIM_HINA_CONFIGUREFORSMOKE_H
