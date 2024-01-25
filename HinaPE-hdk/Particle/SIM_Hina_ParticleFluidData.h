#ifndef HINAPE_CUBBYFLOW_SIM_HINA_PARTICLEFLUIDDATA_H
#define HINAPE_CUBBYFLOW_SIM_HINA_PARTICLEFLUIDDATA_H

#include "HinaHDKClassGenerator.h"

#include "Core/Particle/SPHSystemData.hpp"

NEW_HINA_DATA_CLASS(
		ParticleFluidData,
		CubbyFlow::SPHSystemData3Ptr InnerPtr;
				size_t scalar_idx_offset = std::numeric_limits<size_t>::max();
				GA_RWHandleI gdp_handle_CF_IDX;
				GA_RWHandleV3 gdp_handle_position;
				GA_RWHandleV3 gdp_handle_velocity;
				GA_RWHandleV3 gdp_handle_force;
				GA_RWHandleD gdp_handle_density;
				GA_RWHandleD gdp_handle_pressure;

				NEW_GETSET_PARAMETER(CF_IDX_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(VELOCITY_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(FORCE_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(DENSITY_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(PRESSURE_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)

				void runtime_init_handles(GU_Detail &gdp); // Call Inside Lock
				size_t pt_size() const;
				size_t index(GA_Offset offset);
				GA_Offset offset(size_t index);

				const CubbyFlow::Vector3D &operator[](size_t index) const;
				CubbyFlow::Vector3D &operator[](size_t index);

				const CubbyFlow::Vector3D &position(size_t index) const;
				CubbyFlow::Vector3D &position(size_t index);

				const CubbyFlow::Vector3D &velocity(size_t index) const;
				CubbyFlow::Vector3D &velocity(size_t index);

				const CubbyFlow::Vector3D &force(size_t index) const;
				CubbyFlow::Vector3D &force(size_t index);

				const double &density(size_t index) const;
				double &density(size_t index);

				const double &pressure(size_t index) const;
				double &pressure(size_t index);

				UT_Vector3D gdp_position(size_t index); // Call Inside Lock
				void set_gdp_position(size_t index, UT_Vector3D v3); // Call Inside Lock

				UT_Vector3D gdp_velocity(size_t index); // Call Inside Lock
				void set_gdp_velocity(size_t index, UT_Vector3D v3); // Call Inside Lock

				UT_Vector3D gdp_force(size_t index); // Call Inside Lock
				void set_gdp_force(size_t index, UT_Vector3D v3); // Call Inside Lock

				fpreal gdp_density(size_t index); // Call Inside Lock
				void set_gdp_density(size_t index, fpreal v); // Call Inside Lock

				fpreal gdp_pressure(size_t index); // Call Inside Lock
				void set_gdp_pressure(size_t index, fpreal v); // Call Inside Lock

)

#endif //HINAPE_CUBBYFLOW_SIM_HINA_PARTICLEFLUIDDATA_H
