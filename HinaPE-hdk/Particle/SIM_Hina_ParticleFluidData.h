#ifndef HINAPE_SIM_HINA_PARTICLEFLUIDDATA_H
#define HINAPE_SIM_HINA_PARTICLEFLUIDDATA_H

#include "HinaHDKClassGenerator.h"

#include "Core/Particle/SPHSystemData.hpp"

#define PARTICLE_STATE_NEW_ADDED "new"
#define PARTICLE_STATE_DIRTY "dirty"
#define PARTICLE_STATE_CLEAN "clean"

NEW_HINA_DATA_CLASS(
		ParticleFluidData,
		CubbyFlow::SPHSystemData3Ptr InnerPtr;
				size_t scalar_idx_offset = std::numeric_limits<size_t>::max();
				size_t scalar_idx_state = std::numeric_limits<size_t>::max();
				GA_RWHandleI gdp_handle_CF_IDX;
				GA_RWHandleS gdp_handle_CF_STATE;
				GA_RWHandleV3 gdp_handle_position;
				GA_RWHandleV3 gdp_handle_velocity;
				GA_RWHandleV3 gdp_handle_force;
				GA_RWHandleD gdp_handle_mass;
				GA_RWHandleD gdp_handle_density;
				GA_RWHandleD gdp_handle_pressure;
				GA_RWHandleI gdp_handle_n_sum;
				GA_RWHandleIA gdp_handle_neighbors;

				NEW_GETSET_PARAMETER(CF_IDX_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(CF_STATE_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(VELOCITY_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(FORCE_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(MASS_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(DENSITY_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(PRESSURE_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(NEIGHBORS_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(NEIGHBORS_SUM_ATTRIBUTE_NAME, GETSET_DATA_FUNCS_S)
				NEW_GETSET_PARAMETER(FluidDomain, GETSET_DATA_FUNCS_V3)
				NEW_GETSET_PARAMETER(TargetDensity, GETSET_DATA_FUNCS_F)
				NEW_GETSET_PARAMETER(TargetSpacing, GETSET_DATA_FUNCS_F)
				NEW_GETSET_PARAMETER(KernelRadiusOverTargetSpacing, GETSET_DATA_FUNCS_F)
				NEW_GETSET_PARAMETER(RestitutionCoefficient, GETSET_DATA_FUNCS_F)
				NEW_GETSET_PARAMETER(ActivateFluidDomainCollider, GETSET_DATA_FUNCS_B)

				void configure_init(GU_Detail &gdp); // Call Inside Lock
				void runtime_init_handles(GU_Detail &gdp); // Call Inside Lock
				size_t pt_size() const;
				GA_Offset offset(size_t index);
				void set_offset(size_t index, GA_Offset pt_off);
				std::string state(size_t index);
				void set_state(size_t index, std::string state);

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

				int neighbor_sum(size_t index) const;
				CubbyFlow::Array1<size_t> &neighbors(size_t index);

				size_t gdp_index(GA_Offset pt_off); // Call Inside Lock
				void set_gdp_index(GA_Offset pt_off, size_t index); // Call Inside Lock

				std::string gdp_state(size_t index); // Call Inside Lock
				void set_gdp_state(size_t index, std::string state); // Call Inside Lock

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

				size_t gdp_neighbor_sum(size_t index);
				void set_gdp_neighbor_sum(size_t index, int n);

				UT_Int32Array gdp_neighbors(size_t index);
				void set_gdp_neighbors(size_t index, UT_Int32Array &array);
)

#endif //HINAPE_SIM_HINA_PARTICLEFLUIDDATA_H
