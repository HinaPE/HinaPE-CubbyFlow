#include "SIM_Hina_ParticleFluidData.h"

NEW_HINA_DATA_IMPLEMENT(
		ParticleFluidData,
		false,
		NEW_STRING_PARAMETER(CF_IDX_ATTRIBUTE_NAME, "CF_IDX") \
		NEW_STRING_PARAMETER(VELOCITY_ATTRIBUTE_NAME, "vel") \
        NEW_STRING_PARAMETER(FORCE_ATTRIBUTE_NAME, "force") \
        NEW_STRING_PARAMETER(DENSITY_ATTRIBUTE_NAME, "dens") \
        NEW_STRING_PARAMETER(PRESSURE_ATTRIBUTE_NAME, "pres") \
)

static CubbyFlow::Vector3D ZERO_V3 = CubbyFlow::Vector3D();
static UT_Vector3D ZERO_V3_HDK = UT_Vector3D(0.);
static double ZERO = 0;

void SIM_Hina_ParticleFluidData::_init()
{
	this->InnerPtr = nullptr;
	this->scalar_idx_offset = std::numeric_limits<size_t>::max();
	this->gdp_handle_position.clear();
	this->gdp_handle_velocity.clear();
	this->gdp_handle_force.clear();
	this->gdp_handle_density.clear();
	this->gdp_handle_pressure.clear();
}
void SIM_Hina_ParticleFluidData::_makeEqual(const SIM_Hina_ParticleFluidData *src)
{
	this->InnerPtr = src->InnerPtr;
	this->gdp_handle_position = src->gdp_handle_position;
	this->gdp_handle_velocity = src->gdp_handle_velocity;
	this->gdp_handle_force = src->gdp_handle_force;
	this->gdp_handle_density = src->gdp_handle_density;
	this->gdp_handle_pressure = src->gdp_handle_pressure;
}

void SIM_Hina_ParticleFluidData::runtime_init_handles(GU_Detail &gdp)
{
	gdp_handle_CF_IDX = gdp.findPointAttribute(getCF_IDX_ATTRIBUTE_NAME());
	gdp_handle_position = gdp.getP();
	gdp_handle_velocity = gdp.findPointAttribute(getVELOCITY_ATTRIBUTE_NAME());
	gdp_handle_force = gdp.findPointAttribute(getFORCE_ATTRIBUTE_NAME());
	gdp_handle_density = gdp.findPointAttribute(getDENSITY_ATTRIBUTE_NAME());
	gdp_handle_pressure = gdp.findPointAttribute(getPRESSURE_ATTRIBUTE_NAME());
}
size_t SIM_Hina_ParticleFluidData::index(GA_Offset offset)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_GDP_HANDLE_VALID_WITH_RETURN(gdp_handle_CF_IDX, ZERO)

	size_t index = gdp_handle_CF_IDX.get(offset);
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)

	return index;
}
GA_Offset SIM_Hina_ParticleFluidData::offset(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)

	return (GA_Offset) InnerPtr->ScalarDataAt(scalar_idx_offset)[index];
}
size_t SIM_Hina_ParticleFluidData::pt_size() const
{
	return InnerPtr->NumberOfParticles();
}
const CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::operator[](size_t index) const
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3)

	return InnerPtr->Positions()[index];
}

CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::operator[](size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3)

	return InnerPtr->Positions()[index];
}

const CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::position(size_t index) const
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3)

	return InnerPtr->Positions()[index];
}
CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::position(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3)

	return InnerPtr->Positions()[index];
}
const CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::velocity(size_t index) const
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3)

	return InnerPtr->Velocities()[index];
}
CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::velocity(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3)

	return InnerPtr->Velocities()[index];
}
const CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::force(size_t index) const
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3)

	return InnerPtr->Forces()[index];
}
CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::force(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3)

	return InnerPtr->Forces()[index];
}
const double &SIM_Hina_ParticleFluidData::density(size_t index) const
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)

	return InnerPtr->Densities()[index];
}
double &SIM_Hina_ParticleFluidData::density(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)

	return InnerPtr->Densities()[index];
}
const double &SIM_Hina_ParticleFluidData::pressure(size_t index) const
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)

	return InnerPtr->Pressures()[index];
}
double &SIM_Hina_ParticleFluidData::pressure(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)

	return InnerPtr->Pressures()[index];
}
UT_Vector3D SIM_Hina_ParticleFluidData::gdp_position(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3_HDK)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3_HDK)
	CHECK_GDP_HANDLE_VALID_WITH_RETURN(gdp_handle_position, ZERO_V3_HDK)

	return gdp_handle_position.get(offset(index));
}
void SIM_Hina_ParticleFluidData::set_gdp_position(size_t index, UT_Vector3D v3)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)
	CHECK_GDP_HANDLE_VALID_NO_RETURN(gdp_handle_position)

	gdp_handle_position.set(offset(index), v3);
}
UT_Vector3D SIM_Hina_ParticleFluidData::gdp_velocity(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3_HDK)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3_HDK)
	CHECK_GDP_HANDLE_VALID_WITH_RETURN(gdp_handle_velocity, ZERO_V3_HDK)

	return gdp_handle_velocity.get(offset(index));
}
void SIM_Hina_ParticleFluidData::set_gdp_velocity(size_t index, UT_Vector3D v3)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)
	CHECK_GDP_HANDLE_VALID_NO_RETURN(gdp_handle_velocity)

	gdp_handle_velocity.set(offset(index), v3);
}
UT_Vector3D SIM_Hina_ParticleFluidData::gdp_force(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3_HDK)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3_HDK)
	CHECK_GDP_HANDLE_VALID_WITH_RETURN(gdp_handle_force, ZERO_V3_HDK)

	return gdp_handle_force.get(offset(index));
}
void SIM_Hina_ParticleFluidData::set_gdp_force(size_t index, UT_Vector3D v3)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)
	CHECK_GDP_HANDLE_VALID_NO_RETURN(gdp_handle_force)

	gdp_handle_force.set(offset(index), v3);
}
fpreal SIM_Hina_ParticleFluidData::gdp_density(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)
	CHECK_GDP_HANDLE_VALID_WITH_RETURN(gdp_handle_density, ZERO)

	return gdp_handle_density.get(offset(index));
}
void SIM_Hina_ParticleFluidData::set_gdp_density(size_t index, fpreal v)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)
	CHECK_GDP_HANDLE_VALID_NO_RETURN(gdp_handle_density)

	gdp_handle_density.set(offset(index), v);
}
fpreal SIM_Hina_ParticleFluidData::gdp_pressure(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)
	CHECK_GDP_HANDLE_VALID_WITH_RETURN(gdp_handle_pressure, ZERO)

	return gdp_handle_pressure.get(offset(index));
}
void SIM_Hina_ParticleFluidData::set_gdp_pressure(size_t index, fpreal v)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)
	CHECK_GDP_HANDLE_VALID_NO_RETURN(gdp_handle_pressure)

	gdp_handle_pressure.set(offset(index), v);
}
