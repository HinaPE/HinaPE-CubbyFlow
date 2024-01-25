#include "SIM_Hina_ParticleFluidData.h"

NEW_HINA_DATA_IMPLEMENT(
		ParticleFluidData,
		false
)

void SIM_Hina_ParticleFluidData::_init()
{
	this->InnerPtr = nullptr;
}
void SIM_Hina_ParticleFluidData::_makeEqual(const SIM_Hina_ParticleFluidData *src)
{
	this->InnerPtr = src->InnerPtr;
}

const UT_Vector3D &SIM_Hina_ParticleFluidData::operator[](size_t index) const
{
//	CHECK_CONFIGURED_WITH_RETURN(this, UT_Vector3D(0.))
	UT_Vector3D res(0.);
	return res;
}

UT_Vector3D &SIM_Hina_ParticleFluidData::operator[](size_t index)
{
	UT_Vector3D res(0.);
	return res;
}