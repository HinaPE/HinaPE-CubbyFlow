#ifndef HINAPE_SIM_HINA_GRIDFLUIDDATA_H
#define HINAPE_SIM_HINA_GRIDFLUIDDATA_H

#include "HinaHDKClassGenerator.h"
#include "Core/Grid/GridSystemData.hpp"

#define NEW_HINA_DATA_GETSET_VECTOR_FIELD(NAME) \
const CubbyFlow::Vector3D &NAME(size_t x, size_t y, size_t z) const; \
CubbyFlow::Vector3D &NAME(size_t x, size_t y, size_t z); \
const CubbyFlow::Vector3D &NAME##_at(size_t x, size_t y, size_t z) const; \
void sync_##NAME(SIM_VectorField *vf);

#define NEW_HINA_DATA_GETSET_SCALAR_FIELD(NAME) \
const double &NAME(size_t x, size_t y, size_t z) const; \
double &NAME(size_t x, size_t y, size_t z); \
const double &NAME##_at(const CubbyFlow::Vector3D& pos) const; \
void sync_##NAME(SIM_ScalarField *sf);

NEW_HINA_DATA_CLASS(
		GridFluidData,
		CubbyFlow::GridSystemData3Ptr InnerPtr;
				size_t scalar_density_offset = std::numeric_limits<size_t>::max();
				size_t scalar_temperature_offset = std::numeric_limits<size_t>::max();

				NEW_GETSET_PARAMETER(Center, GETSET_DATA_FUNCS_V3)
				NEW_GETSET_PARAMETER(Size, GETSET_DATA_FUNCS_V3)
				NEW_GETSET_PARAMETER(Division, GETSET_DATA_FUNCS_F)

				NEW_HINA_DATA_GETSET_SCALAR_FIELD(density)
				NEW_HINA_DATA_GETSET_SCALAR_FIELD(temprature)
)

#endif //HINAPE_SIM_HINA_GRIDFLUIDDATA_H
