#ifndef HINAPE_HDK_CLASS_GENERATOR_H
#define HINAPE_HDK_CLASS_GENERATOR_H

#include <GAS/GAS_SubSolver.h>

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataUtils.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_GuideShared.h>
#include <SIM/SIM_ColliderLabel.h>
#include <SIM/SIM_ForceGravity.h>
#include <SIM/SIM_Time.h>
#include <SIM/SIM_Utils.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>

#include <GEO/GEO_PrimPoly.h>
#include <GEO/GEO_PrimSphere.h>
#include <GU/GU_PrimSphere.h>

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#define NEW_GETSET_PARAMETER(NAME, GETSET_TYPE) \
GETSET_TYPE(#NAME, NAME)

#define ACTIVATE_GAS_GEOMETRY \
static PRM_Name    theGeometryName(GAS_NAME_GEOMETRY, "Geometry"); \
static PRM_Default    theGeometryNameDefault(0, "Geometry"); \
PRMS.emplace_back(PRM_STRING, 1, &theGeometryName, &theGeometryNameDefault);

#define ACTIVATE_GAS_VELOCITY \
static PRM_Name    theVelocityName(GAS_NAME_VELOCITY, "Velocity"); \
static PRM_Default    theVelocityNameDefault(0, "Velocity"); \
PRMS.emplace_back(PRM_STRING, 1, &theVelocityName, &theVelocityNameDefault);

#define ACTIVATE_GAS_DENSITY \
static PRM_Name    theDensityName(GAS_NAME_DENSITY, "Density"); \
static PRM_Default    theDensityNameDefault(0, "Density"); \
PRMS.emplace_back(PRM_STRING, 1, &theDensityName, &theDensityNameDefault);

#define ACTIVATE_GAS_PRESSURE \
static PRM_Name    thePressureName(GAS_NAME_PRESSURE, "Pressure"); \
static PRM_Default    thePressureNameDefault(0, "Pressure"); \
PRMS.emplace_back(PRM_STRING, 1, &thePressureName, &thePressureNameDefault);

#define NEW_INT_PARAMETER(NAME, DEFAULT_VALUE) \
static PRM_Name NAME(#NAME, #NAME); \
static PRM_Default Default##NAME(DEFAULT_VALUE);  \
PRMS.emplace_back(PRM_INT, 1, &NAME, &Default##NAME);

#define NEW_FLOAT_PARAMETER(NAME, DEFAULT_VALUE) \
static PRM_Name NAME(#NAME, #NAME); \
static PRM_Default Default##NAME(DEFAULT_VALUE);  \
PRMS.emplace_back(PRM_FLT, 1, &NAME, &Default##NAME);

#define NEW_STRING_PARAMETER(NAME, DEFAULT_VALUE) \
static PRM_Name NAME(#NAME, #NAME); \
static PRM_Default Default##NAME(0, DEFAULT_VALUE);  \
PRMS.emplace_back(PRM_STRING, 1, &NAME, &Default##NAME);

#define NEW_BOOL_PARAMETER(NAME, DEFAULT_VALUE) \
static PRM_Name NAME(#NAME, #NAME); \
static PRM_Default Default##NAME(DEFAULT_VALUE);  \
PRMS.emplace_back(PRM_TOGGLE, 1, &NAME, &Default##NAME);

#define NEW_INT_VECTOR_PARAMETER(NAME, SIZE, ...) \
static PRM_Name NAME(#NAME, #NAME); \
static std::array<PRM_Default, SIZE> Default##NAME{__VA_ARGS__};  \
PRMS.emplace_back(PRM_INT, SIZE, &NAME, Default##NAME.data());

#define NEW_FLOAT_VECTOR_PARAMETER(NAME, SIZE, ...) \
static PRM_Name NAME(#NAME, #NAME); \
static std::array<PRM_Default, SIZE> Default##NAME{__VA_ARGS__};  \
PRMS.emplace_back(PRM_FLT, SIZE, &NAME, Default##NAME.data());

#define CHECK_NULL(ptr) \
if (!ptr) \
{ \
error_msg.appendSprintf("%s::NULL POINTER Exception, From %s\n", ptr->getDataType().c_str(), DATANAME); \
return false; \
}

#define CHECK_CONFIGURED(dataptr) \
if (!dataptr->Configured) \
{ \
error_msg.appendSprintf("%s::DATA NOT CONFIGURED Exception, From %s\n", dataptr->getDataType().c_str(), DATANAME); \
return false; \
}

#define CHECK_CONFIGURED_NO_RETURN(data_ptr) \
if (!data_ptr->Configured) \
{ \
error_msg.appendSprintf("%s::DATA NOT CONFIGURED Exception, From %s\n", data_ptr->getDataType().c_str(), DATANAME); \
}

#define CHECK_CONFIGURED_WITH_RETURN(data_ptr, return_value) \
if (!data_ptr->Configured) \
{ \
error_msg.appendSprintf("%s::DATA NOT CONFIGURED Exception, From %s\n", data_ptr->getDataType().c_str(), DATANAME); \
return return_value; \
}

#define CHECK_CUBBY_ARRAY_BOUND_NO_RETURN(InnerPtr, index) \
if (index >= InnerPtr->NumberOfParticles()) \
{ \
error_msg.appendSprintf("Index > Array Bound, From %s\n", DATANAME); \
}

#define CHECK_CUBBY_ARRAY_BOUND_WITH_RETURN(InnerPtr, index, return_value) \
if (index >= InnerPtr->NumberOfParticles()) \
{ \
error_msg.appendSprintf("Index > Array Bound, From %s\n", DATANAME); \
return return_value; \
}

#define CHECK_GDP_HANDLE_VALID_NO_RETURN(handle) \
if (handle.isValid())                                           \
{ \
error_msg.appendSprintf("HANDLE INVALID, PLEASE ENSURE YOU HAVE INIT THIS ATTRIBUTE::%s, From %s\n", handle->getName().c_str(), DATANAME); \
}

#define CHECK_GDP_HANDLE_VALID_WITH_RETURN(handle, return_value) \
if (handle.isValid())                                           \
{ \
error_msg.appendSprintf("HANDLE INVALID, PLEASE ENSURE YOU HAVE INIT THIS ATTRIBUTE::%s, From %s\n", handle->getName().c_str(), DATANAME); \
return return_value; \
}

#define NEW_HINA_MICRPSOLVER_CLASS(NAME, ...) \
class GAS_Hina_##NAME : public GAS_SubSolver \
{ \
public: \
static const char *DATANAME; \
mutable UT_WorkBuffer error_msg; \
__VA_ARGS__ \
protected: \
GAS_Hina_##NAME(const SIM_DataFactory *factory) : BaseClass(factory) {} \
~GAS_Hina_##NAME() override = default; \
bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override; \
void initializeSubclass() override; \
void makeEqualSubclass(const SIM_Data *source) override; \
static const SIM_DopDescription *getDopDescription(); \
DECLARE_STANDARD_GETCASTTOTYPE(); \
DECLARE_DATAFACTORY(GAS_Hina_##NAME, GAS_SubSolver, "Hina_"#NAME, getDopDescription());                    \
private: \
void _init(); \
void _makeEqual(const GAS_Hina_##NAME *src); \
bool _solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg); \
};

#define NEW_HINA_MICRPSOLVER_IMPLEMENT(NAME, UNIQUE, ...) \
bool GAS_Hina_##NAME::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) \
{ \
    CHECK_NULL(obj) \
    if (!_solve(engine, obj, time, timestep, this->error_msg) || UTisstring(this->error_msg.buffer())) \
    { \
        SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT); \
        return false; \
    } \
    return true; \
} \
void GAS_Hina_##NAME::initializeSubclass() \
{ \
    SIM_Data::initializeSubclass(); \
    this->error_msg.clear(); \
    _init(); \
} \
void GAS_Hina_##NAME::makeEqualSubclass(const SIM_Data *source) \
{ \
    SIM_Data::makeEqualSubclass(source); \
    const GAS_Hina_##NAME *src = SIM_DATA_CASTCONST(source, GAS_Hina_##NAME); \
    this->error_msg = src->error_msg; \
    _makeEqual(src); \
} \
const char *GAS_Hina_##NAME::DATANAME = "Hina_"#NAME; \
const SIM_DopDescription *GAS_Hina_##NAME::getDopDescription() \
{ \
static std::vector<PRM_Template> PRMS; \
PRMS.clear(); \
__VA_ARGS__ \
PRMS.emplace_back(); \
static SIM_DopDescription DESC(true, \
                               "Hina_"#NAME, \
                               "Hina "#NAME, \
                               DATANAME, \
                               classname(), \
                               PRMS.data()); \
DESC.setDefaultUniqueDataName(UNIQUE); \
setGasDescription(DESC); \
return &DESC; \
}

#define NEW_HINA_DATA_CLASS(NAME, ...) \
class SIM_Hina_##NAME : public SIM_Data, public SIM_OptionsUser \
{ \
public: \
static const char *DATANAME; \
bool Configured = false; \
mutable UT_WorkBuffer error_msg; \
__VA_ARGS__ \
protected: \
SIM_Hina_##NAME(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {} \
~SIM_Hina_##NAME() override = default; \
void initializeSubclass() override; \
void makeEqualSubclass(const SIM_Data *source) override; \
static const SIM_DopDescription *getDopDescription(); \
DECLARE_STANDARD_GETCASTTOTYPE(); \
DECLARE_DATAFACTORY(SIM_Hina_##NAME, SIM_Data, "Hina_"#NAME, getDopDescription());                    \
private: \
void _init(); \
void _makeEqual(const SIM_Hina_##NAME *src); \
};

#define NEW_HINA_DATA_IMPLEMENT(NAME, UNIQUE, ...) \
void SIM_Hina_##NAME::initializeSubclass() \
{ \
    SIM_Data::initializeSubclass(); \
    this->Configured = false; \
    this->error_msg.clear(); \
    _init(); \
} \
void SIM_Hina_##NAME::makeEqualSubclass(const SIM_Data *source) \
{ \
    SIM_Data::makeEqualSubclass(source); \
    const SIM_Hina_##NAME *src = SIM_DATA_CASTCONST(source, SIM_Hina_##NAME); \
    this->Configured = src->Configured; \
    this->error_msg = src->error_msg; \
    _makeEqual(src); \
} \
const char *SIM_Hina_##NAME::DATANAME = "Hina_"#NAME; \
const SIM_DopDescription *SIM_Hina_##NAME::getDopDescription() \
{ \
static std::vector<PRM_Template> PRMS;             \
PRMS.clear(); \
__VA_ARGS__ \
PRMS.emplace_back(); \
static SIM_DopDescription DESC(true, \
                               "Hina_"#NAME, \
                               "Hina "#NAME, \
                               DATANAME, \
                               classname(), \
                               PRMS.data()); \
DESC.setDefaultUniqueDataName(UNIQUE); \
return &DESC; \
}

#define NEW_HINA_GEOMETRY_CLASS(NAME, InnerTypePtr, ...) \
class SIM_Hina_##NAME : public SIM_Geometry \
{ \
public: \
static const char *DATANAME; \
bool Configured = false; \
mutable GU_DetailHandle my_detail_handle; \
mutable UT_WorkBuffer error_msg; \
__VA_ARGS__ \
InnerTypePtr RuntimeConstruct() const; \
protected: \
SIM_Hina_##NAME(const SIM_DataFactory *factory) : BaseClass(factory) {} \
~SIM_Hina_##NAME() override = default; \
GU_ConstDetailHandle getGeometrySubclass() const override; \
void initializeSubclass() override; \
void makeEqualSubclass(const SIM_Data *source) override; \
static const SIM_DopDescription *getDopDescription(); \
DECLARE_STANDARD_GETCASTTOTYPE(); \
DECLARE_DATAFACTORY(SIM_Hina_##NAME, SIM_Geometry, "Hina_"#NAME, getDopDescription());                    \
private: \
void _init(); \
void _makeEqual(const SIM_Hina_##NAME *src); \
};

#define NEW_HINA_GEOMETRY_IMPLEMENT(NAME, InnerTypePtr, UNIQUE, ...) \
void SIM_Hina_##NAME::initializeSubclass() \
{ \
    SIM_Geometry::initializeSubclass(); \
    this->Configured = false; \
    this->my_detail_handle.clear(); \
    this->error_msg.clear(); \
    _init(); \
} \
void SIM_Hina_##NAME::makeEqualSubclass(const SIM_Data *source) \
{ \
    SIM_Geometry::makeEqualSubclass(source); \
    const SIM_Hina_##NAME *src = SIM_DATA_CASTCONST(source, SIM_Hina_##NAME); \
    this->Configured = src->Configured; \
    this->my_detail_handle = src->my_detail_handle; \
    this->error_msg = src->error_msg; \
    _makeEqual(src); \
}                                                                   \
const char *SIM_Hina_##NAME::DATANAME = "Hina_"#NAME; \
const SIM_DopDescription *SIM_Hina_##NAME::getDopDescription() \
{ \
static std::vector<PRM_Template> PRMS; \
PRMS.clear(); \
__VA_ARGS__ \
PRMS.emplace_back(); \
static SIM_DopDescription DESC(true, \
                               "Hina_"#NAME, \
                               "Hina "#NAME, \
                               DATANAME, \
                               classname(), \
                               PRMS.data()); \
DESC.setDefaultUniqueDataName(UNIQUE); \
return &DESC; \
}

#define AS_UTVector3D(Vec3) UT_Vector3D(Vec3.x, Vec3.y, Vec3.z)

#endif //HINAPE_HDK_CLASS_GENERATOR_H
