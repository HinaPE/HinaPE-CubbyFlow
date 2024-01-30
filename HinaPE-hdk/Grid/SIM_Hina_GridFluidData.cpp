#include "SIM_Hina_GridFluidData.h"

#include <SIM/SIM_ScalarField.h>
#include <SIM/SIM_VectorField.h>

NEW_HINA_DATA_IMPLEMENT(
		GridFluidData,
		false,
		ACTIVATE_GAS_DENSITY \
        ACTIVATE_GAS_TEMPERATURE \
        ACTIVATE_GAS_VELOCITY \
        NEW_FLOAT_VECTOR_PARAMETER(Center, 3, 0, 0, 0) \
        NEW_INT_VECTOR_PARAMETER(Size, 3, 1, 1, 1) \
        NEW_FLOAT_PARAMETER(Division, .1) \
)

//#define NEW_HINA_DATA_GETSET_VECTOR_FIELD_IMPL

#define NEW_HINA_DATA_GETSET_SCALAR_FIELD_IMPL(NAME, DATA_INDEX) \
const double &SIM_Hina_GridFluidData::NAME(size_t x, size_t y, size_t z) const \
{ \
    CubbyFlow::ScalarGrid3Ptr sg = this->InnerPtr->AdvectableScalarDataAt(DATA_INDEX); \
    return (*sg)(x, y, z); \
} \
double &SIM_Hina_GridFluidData::NAME(size_t x, size_t y, size_t z) \
{ \
    CubbyFlow::ScalarGrid3Ptr sg = this->InnerPtr->AdvectableScalarDataAt(DATA_INDEX); \
    return (*sg)(x, y, z); \
} \
const double &SIM_Hina_GridFluidData::NAME##_at(const CubbyFlow::Vector3D& pos) const \
{ \
	CubbyFlow::ScalarGrid3Ptr sg = this->InnerPtr->AdvectableScalarDataAt(DATA_INDEX); \
	return sg->Sample(pos); \
} \
void SIM_Hina_GridFluidData::sync_##NAME(SIM_ScalarField *sf) \
{ \
    CubbyFlow::ScalarGrid3Ptr sg = this->InnerPtr->AdvectableScalarDataAt(1); \
    sg->ForEachDataPointIndex( \
    [&](size_t i, size_t j, size_t k) \
    { \
    double v = (*sg)(i, j, k); \
    sf->getField()->setCellValue(i, j, k, v); \
    }); \
}

NEW_HINA_DATA_GETSET_SCALAR_FIELD_IMPL(density, scalar_density_offset)
NEW_HINA_DATA_GETSET_SCALAR_FIELD_IMPL(temprature, scalar_temperature_offset)

void SIM_Hina_GridFluidData::_init()
{
	this->InnerPtr = nullptr;
	this->scalar_density_offset = std::numeric_limits<size_t>::max();
	this->scalar_temperature_offset = std::numeric_limits<size_t>::max();
}
void SIM_Hina_GridFluidData::_makeEqual(const SIM_Hina_GridFluidData *src)
{
	this->InnerPtr = src->InnerPtr;
	this->scalar_density_offset = src->scalar_density_offset;
	this->scalar_temperature_offset = src->scalar_temperature_offset;
}
