#include "SIM_Hina_ParticleFluidData.h"
#include <Particle/SIM_Hina_ConfigureForSPH.h>

static double TIME_STEP_LIMIT_BY_SPEED_FACTOR = 0.4;
static double TIME_STEP_LIMIT_BY_FORCE_FACTOR = 0.25;
namespace ParticleFluidData
{
static CubbyFlow::Vector3D ZERO_V3 = CubbyFlow::Vector3D();
static UT_Vector3D ZERO_V3_HDK = UT_Vector3D(0.);
static double ZERO = 0;
static int ZERO_I = 0;
static CubbyFlow::Array1<size_t> ZERO_Arr{};
static UT_Int32Array Zero_Arr_HDK{};
}
using namespace ParticleFluidData;

#define NEW_HINA_DATA_GETSET_V3_IMPL(NAME, SRC, HANDLE) \
const CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::NAME(size_t index) const \
{ \
CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3) \
CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3) \
return SRC[index]; \
} \
CubbyFlow::Vector3D &SIM_Hina_ParticleFluidData::NAME(size_t index) \
{ \
    CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3) \
    CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3) \
    return SRC[index]; \
} \
UT_Vector3D SIM_Hina_ParticleFluidData::gdp_##NAME(size_t index) \
{ \
    CHECK_CONFIGURED_WITH_RETURN(this, ZERO_V3_HDK) \
    CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_V3_HDK) \
    CHECK_GDP_HANDLE_VALID_WITH_RETURN(HANDLE, ZERO_V3_HDK) \
    return HANDLE.get(offset(index)); \
} \
void SIM_Hina_ParticleFluidData::set_gdp_##NAME(size_t index, UT_Vector3D v3) \
{ \
    CHECK_CONFIGURED_NO_RETURN(this) \
    CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index) \
    CHECK_GDP_HANDLE_VALID_NO_RETURN(HANDLE) \
    HANDLE.set(offset(index), v3); \
} \
void SIM_Hina_ParticleFluidData::sync_##NAME(SIM_GeometryCopy *geo) \
{ \
SIM_GeometryAutoWriteLock lock(geo); \
GU_Detail &gdp = lock.getGdp(); \
runtime_init_handles(gdp); \
GA_Offset pt_off; \
GA_FOR_ALL_PTOFF(&gdp, pt_off) \
{ \
size_t pt_idx = gdp_index(pt_off); \
HANDLE.set(pt_off, AS_UTVector3D(NAME(pt_idx))); \
} \
}

#define NEW_HINA_DATA_GETSET_D_IMPL(NAME, SRC, HANDLE) \
const double &SIM_Hina_ParticleFluidData::NAME(size_t index) const \
{ \
    CHECK_CONFIGURED_WITH_RETURN(this, ZERO) \
    CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO) \
    return SRC[index]; \
} \
double &SIM_Hina_ParticleFluidData::NAME(size_t index) \
{ \
    CHECK_CONFIGURED_WITH_RETURN(this, ZERO) \
    CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO) \
    return SRC[index]; \
} \
fpreal SIM_Hina_ParticleFluidData::gdp_##NAME(size_t index) \
{ \
    CHECK_CONFIGURED_WITH_RETURN(this, ZERO) \
    CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO) \
    CHECK_GDP_HANDLE_VALID_WITH_RETURN(HANDLE, ZERO) \
    return HANDLE.get(offset(index)); \
} \
void SIM_Hina_ParticleFluidData::set_gdp_##NAME(size_t index, fpreal v) \
{ \
    CHECK_CONFIGURED_NO_RETURN(this) \
    CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index) \
    CHECK_GDP_HANDLE_VALID_NO_RETURN(HANDLE) \
    HANDLE.set(offset(index), v); \
} \
void SIM_Hina_ParticleFluidData::sync_##NAME(SIM_GeometryCopy *geo) \
{ \
SIM_GeometryAutoWriteLock lock(geo); \
GU_Detail &gdp = lock.getGdp(); \
runtime_init_handles(gdp); \
GA_Offset pt_off; \
GA_FOR_ALL_PTOFF(&gdp, pt_off) \
{ \
size_t pt_idx = gdp_index(pt_off); \
HANDLE.set(pt_off, NAME(pt_idx)); \
} \
}

NEW_HINA_DATA_IMPLEMENT(
		ParticleFluidData,
		false,
		NEW_STRING_PARAMETER(CF_IDX_ATTRIBUTE_NAME, "CF_IDX") \
        NEW_STRING_PARAMETER(CF_STATE_ATTRIBUTE_NAME, "CF_ST") \
        NEW_STRING_PARAMETER(VELOCITY_ATTRIBUTE_NAME, "vel") \
        NEW_STRING_PARAMETER(FORCE_ATTRIBUTE_NAME, "force") \
        NEW_STRING_PARAMETER(MASS_ATTRIBUTE_NAME, "mass") \
        NEW_STRING_PARAMETER(DENSITY_ATTRIBUTE_NAME, "dens") \
        NEW_STRING_PARAMETER(PRESSURE_ATTRIBUTE_NAME, "pres") \
        NEW_STRING_PARAMETER(NEIGHBORS_ATTRIBUTE_NAME, "neighbors") \
        NEW_STRING_PARAMETER(NEIGHBORS_SUM_ATTRIBUTE_NAME, "n_sums") \
        NEW_FLOAT_VECTOR_PARAMETER(FluidDomain, 3, 2, 2, 2) \
        NEW_FLOAT_PARAMETER(TargetDensity, 1000.) \
        NEW_FLOAT_PARAMETER(TargetSpacing, .02) \
        NEW_FLOAT_PARAMETER(KernelRadiusOverTargetSpacing, 1.8) \
        NEW_FLOAT_PARAMETER(RestitutionCoefficient, .1) \
        NEW_FLOAT_PARAMETER(EosExponent, 7.) \
        NEW_FLOAT_PARAMETER(NegativePressureScale, 0.) \
        NEW_FLOAT_PARAMETER(ViscosityCoefficient, 0.01) \
        NEW_FLOAT_PARAMETER(PseudoViscosityCoefficient, 10) \
        NEW_FLOAT_PARAMETER(SpeedOfSound, 100) \
        NEW_FLOAT_PARAMETER(TimeStepLimitScale, 1.) \
        NEW_FLOAT_PARAMETER(MaxDensityErrorRatio, 0.01) \
        NEW_FLOAT_PARAMETER(MaxNumberOfIterations, 5) \
        NEW_BOOL_PARAMETER(ActivateFluidDomainCollider, true) \
        static std::array<PRM_Name, 3> DataTypeList = {\
                    PRM_Name("0", "SPH"), \
                    PRM_Name("1", "PCISPH"), \
                    PRM_Name(nullptr), \
		}; \
        static PRM_Name DataTypeName("FluidDataType", "Fluid Data Type"); \
        static PRM_Default DataTypeDefault(0, "SPH"); \
        static PRM_ChoiceList CL(PRM_CHOICELIST_SINGLE, DataTypeList.data()); \
        PRMS.emplace_back(PRM_ORD, 1, &DataTypeName, &DataTypeDefault, &CL); \
)
NEW_HINA_DATA_GETSET_V3_IMPL(position, InnerPtr->Positions(), gdp_handle_position)
NEW_HINA_DATA_GETSET_V3_IMPL(velocity, InnerPtr->Velocities(), gdp_handle_velocity)
NEW_HINA_DATA_GETSET_V3_IMPL(force, InnerPtr->Forces(), gdp_handle_force)
NEW_HINA_DATA_GETSET_D_IMPL(density, InnerPtr->Densities(), gdp_handle_density)
NEW_HINA_DATA_GETSET_D_IMPL(pressure, InnerPtr->Pressures(), gdp_handle_pressure)

void SIM_Hina_ParticleFluidData::_init()
{
	this->InnerPtr = nullptr;
	this->scalar_idx_offset = std::numeric_limits<size_t>::max();
	this->scalar_idx_state = std::numeric_limits<size_t>::max();
	this->gdp_handle_CF_IDX.clear();
	this->gdp_handle_CF_STATE.clear();
	this->gdp_handle_position.clear();
	this->gdp_handle_velocity.clear();
	this->gdp_handle_force.clear();
	this->gdp_handle_mass.clear();
	this->gdp_handle_density.clear();
	this->gdp_handle_pressure.clear();
	this->gdp_handle_n_sum.clear();
	this->gdp_handle_neighbors.clear();
}
void SIM_Hina_ParticleFluidData::_makeEqual(const SIM_Hina_ParticleFluidData *src)
{
	this->InnerPtr = src->InnerPtr;
	this->scalar_idx_offset = src->scalar_idx_offset;
	this->scalar_idx_state = src->scalar_idx_state;
	this->gdp_handle_CF_IDX = src->gdp_handle_CF_IDX;
	this->gdp_handle_CF_STATE = src->gdp_handle_CF_STATE;
	this->gdp_handle_position = src->gdp_handle_position;
	this->gdp_handle_velocity = src->gdp_handle_velocity;
	this->gdp_handle_force = src->gdp_handle_force;
	this->gdp_handle_mass = src->gdp_handle_mass;
	this->gdp_handle_density = src->gdp_handle_density;
	this->gdp_handle_pressure = src->gdp_handle_pressure;
	this->gdp_handle_n_sum = src->gdp_handle_n_sum;
	this->gdp_handle_neighbors = src->gdp_handle_neighbors;
}

void SIM_Hina_ParticleFluidData::runtime_init_handles(GU_Detail &gdp)
{
	CHECK_CONFIGURED_NO_RETURN(this)

	gdp_handle_CF_IDX = gdp.findPointAttribute(getCF_IDX_ATTRIBUTE_NAME());
	gdp_handle_CF_STATE = gdp.findPointAttribute(getCF_STATE_ATTRIBUTE_NAME());
	gdp_handle_position = gdp.getP();
	gdp_handle_velocity = gdp.findPointAttribute(getVELOCITY_ATTRIBUTE_NAME());
	gdp_handle_force = gdp.findPointAttribute(getFORCE_ATTRIBUTE_NAME());
	gdp_handle_mass = gdp.findPointAttribute(getMASS_ATTRIBUTE_NAME());
	gdp_handle_density = gdp.findPointAttribute(getDENSITY_ATTRIBUTE_NAME());
	gdp_handle_pressure = gdp.findPointAttribute(getPRESSURE_ATTRIBUTE_NAME());
	gdp_handle_n_sum = gdp.findPointAttribute(getNEIGHBORS_SUM_ATTRIBUTE_NAME());
	gdp_handle_neighbors = gdp.findPointAttribute(getNEIGHBORS_ATTRIBUTE_NAME());
}
double SIM_Hina_ParticleFluidData::calculate_dynamic_dt()
{
	CHECK_CONFIGURED_NO_RETURN(this)

	using namespace CubbyFlow;
	double m_speedOfSound = getSpeedOfSound();
	double m_timeStepLimitScale = getTimeStepLimitScale();
	SPHSystemData3Ptr particles = InnerPtr;
	const size_t numberOfParticles = particles->NumberOfParticles();
	ArrayView1<Vector3D> f = particles->Forces();

	const double kernelRadius = particles->KernelRadius();
	const double mass = particles->Mass();

	double maxForceMagnitude = 0.0;

	for (size_t i = 0; i<numberOfParticles; ++i)
	{
		maxForceMagnitude = std::max(maxForceMagnitude, f[i].Length());
	}

	if (maxForceMagnitude < 0.1)
		maxForceMagnitude = 0.1;

	const double timeStepLimitBySpeed =
			TIME_STEP_LIMIT_BY_SPEED_FACTOR * kernelRadius / m_speedOfSound;
	const double timeStepLimitByForce =
			TIME_STEP_LIMIT_BY_FORCE_FACTOR *
			std::sqrt(kernelRadius * mass / maxForceMagnitude);

	const double desiredTimeStep =
			m_timeStepLimitScale *
			std::min(timeStepLimitBySpeed, timeStepLimitByForce);

	return desiredTimeStep;
}
size_t SIM_Hina_ParticleFluidData::pt_size() const
{
	return InnerPtr->NumberOfParticles();
}

int SIM_Hina_ParticleFluidData::neighbor_sum(size_t index) const
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_I)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_I)

	return (int) InnerPtr->NeighborLists()[index].Size().x;
}
CubbyFlow::Array1<size_t> &SIM_Hina_ParticleFluidData::neighbors(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_Arr)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_Arr)

	return const_cast<CubbyFlow::Array1<size_t> &>(InnerPtr->NeighborLists()[index]);
}
size_t SIM_Hina_ParticleFluidData::gdp_neighbor_sum(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO_I)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO_I)
	CHECK_GDP_HANDLE_VALID_WITH_RETURN(gdp_handle_n_sum, ZERO_I)

	gdp_handle_n_sum.get(offset(index));
}
void SIM_Hina_ParticleFluidData::set_gdp_neighbor_sum(size_t index, int n)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)
	CHECK_GDP_HANDLE_VALID_NO_RETURN(gdp_handle_n_sum)

	gdp_handle_n_sum.set(offset(index), n);
}
UT_Int32Array SIM_Hina_ParticleFluidData::gdp_neighbors(size_t index)
{
	// TODO: Check
//	CHECK_CONFIGURED_WITH_RETURN(this, Zero_Arr_HDK)
//	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, Zero_Arr_HDK)
//	CHECK_GDP_HANDLE_VALID_WITH_RETURN(gdp_handle_neighbors, Zero_Arr_HDK)

	UT_Int32Array res;
	gdp_handle_neighbors.get(offset(index), res);
	return res;
}
void SIM_Hina_ParticleFluidData::set_gdp_neighbors(size_t index, UT_Int32Array &array)
{
	// TODO: Check

	gdp_handle_neighbors.set(offset(index), array);
}
void SIM_Hina_ParticleFluidData::sync_neighbors(SIM_GeometryCopy *geo)
{
	SIM_GeometryAutoWriteLock lock(geo);
	GU_Detail &gdp = lock.getGdp();
	runtime_init_handles(gdp);
	GA_Offset pt_off;
	GA_FOR_ALL_PTOFF(&gdp, pt_off)
		{
			size_t pt_idx = gdp_index(pt_off);
			set_gdp_neighbor_sum(pt_idx, neighbor_sum(pt_idx));
			UT_Int32Array ns;
			for (auto n: neighbors(pt_idx))
				ns.append(n);
			set_gdp_neighbors(pt_idx, ns);
		}
}

GA_Offset SIM_Hina_ParticleFluidData::offset(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)

	return (GA_Offset) InnerPtr->ScalarDataAt(scalar_idx_offset)[index];
}
void SIM_Hina_ParticleFluidData::set_offset(size_t index, GA_Offset pt_off)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)

	InnerPtr->ScalarDataAt(scalar_idx_offset)[index] = pt_off;
}
size_t SIM_Hina_ParticleFluidData::gdp_index(GA_Offset offset)
{
	CHECK_CONFIGURED_WITH_RETURN(this, ZERO)
	CHECK_GDP_HANDLE_VALID_WITH_RETURN(gdp_handle_CF_IDX, ZERO)

	size_t index = gdp_handle_CF_IDX.get(offset);
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, ZERO)

	return index;
}
void SIM_Hina_ParticleFluidData::set_gdp_index(GA_Offset pt_off, size_t index)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)
	CHECK_GDP_HANDLE_VALID_NO_RETURN(gdp_handle_CF_IDX)

	gdp_handle_CF_IDX.set(pt_off, index);
}
std::string SIM_Hina_ParticleFluidData::state(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, "")
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, "")

	int state = (int) InnerPtr->ScalarDataAt(scalar_idx_state)[index];
	switch (state)
	{
		case 0:
			return PARTICLE_STATE_NEW_ADDED;
		case 1:
			return PARTICLE_STATE_DIRTY;
		case 2:
			return PARTICLE_STATE_CLEAN;
		default:
			return "ERROR";
	}
}
void SIM_Hina_ParticleFluidData::set_state(size_t index, std::string state)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)

	if (state == PARTICLE_STATE_NEW_ADDED)
		InnerPtr->ScalarDataAt(scalar_idx_state)[index] = 0;
	else if (state == PARTICLE_STATE_DIRTY)
		InnerPtr->ScalarDataAt(scalar_idx_state)[index] = 1;
	else if (state == PARTICLE_STATE_CLEAN)
		InnerPtr->ScalarDataAt(scalar_idx_state)[index] = 2;
	else
		InnerPtr->ScalarDataAt(scalar_idx_state)[index] = 99999;
}
std::string SIM_Hina_ParticleFluidData::gdp_state(size_t index)
{
	CHECK_CONFIGURED_WITH_RETURN(this, "")
	CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, "")
	CHECK_GDP_HANDLE_VALID_NO_RETURN(gdp_handle_CF_STATE)

	auto state = gdp_handle_CF_STATE.get(offset(index));
	return state.toStdString();
}
void SIM_Hina_ParticleFluidData::set_gdp_state(size_t index, std::string state)
{
	CHECK_CONFIGURED_NO_RETURN(this)
	CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index)
	CHECK_GDP_HANDLE_VALID_NO_RETURN(gdp_handle_CF_STATE)

	gdp_handle_CF_STATE.set(offset(index), state);
}
SIM_Guide *SIM_Hina_ParticleFluidData::createGuideObjectSubclass() const
{
	return new SIM_GuideShared(this, true);
}
void SIM_Hina_ParticleFluidData::buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const
{
	if (gdh.isNull())
		return;

	if (!getShowGuideGeometry(options))
		return;

	UT_Vector3 color = getDomainColor(options);

	GU_DetailHandleAutoWriteLock gdl(gdh);
	GU_Detail *gdp = gdl.getGdp();
	gdp->clearAndDestroy();

	UT_Vector3 Center = UT_Vector3(0.);
	UT_Vector3 Extent = getFluidDomain();

	std::array<UT_Vector3, 8> vertices{};
	for (int i = 0; i < 8; i++)
	{
		vertices[i] = UT_Vector3(
				Center.x() + Extent.x() * ((i & 1) ? 0.5 : -0.5),
				Center.y() + Extent.y() * ((i & 2) ? 0.5 : -0.5),
				Center.z() + Extent.z() * ((i & 4) ? 0.5 : -0.5)
		);
	}

	std::array<GA_Offset, 8> pt_off{};
	for (int i = 0; i < 8; i++)
	{
		pt_off[i] = gdp->appendPointOffset();
		gdp->setPos3(pt_off[i], vertices[i]);

		GA_RWHandleV3 gdp_handle_cd(gdp->addFloatTuple(GA_ATTRIB_POINT, "Cd", 3));
		gdp_handle_cd.set(pt_off[i], color);
	}

	static const int edges[12][2] = {
			{0, 1},
			{0, 4},
			{1, 3},
			{1, 5},
			{2, 0},
			{2, 3},
			{2, 6},
			{3, 7},
			{4, 5},
			{4, 6},
			{5, 7},
			{6, 7},
	};

	for (int i = 0; i < 12; i++)
	{
		GEO_PrimPoly *line = GEO_PrimPoly::build(gdp, 2, GU_POLY_OPEN);
		for (int j = 0; j < 2; j++)
			line->setVertexPoint(j, pt_off[edges[i][j]]);
	}
}
