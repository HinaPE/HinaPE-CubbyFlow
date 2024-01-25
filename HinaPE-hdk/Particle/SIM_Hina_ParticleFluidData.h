#ifndef HINAPE_CUBBYFLOW_SIM_HINA_PARTICLEFLUIDDATA_H
#define HINAPE_CUBBYFLOW_SIM_HINA_PARTICLEFLUIDDATA_H

#include "HinaHDKClassGenerator.h"

#include "Core/Particle/SPHSystemData.hpp"

NEW_HINA_DATA_CLASS(
		ParticleFluidData,
		CubbyFlow::SPHSystemData3Ptr InnerPtr;

				const UT_Vector3D& operator[](size_t index) const;
				UT_Vector3D& operator[](size_t index);
)

#endif //HINAPE_CUBBYFLOW_SIM_HINA_PARTICLEFLUIDDATA_H
