#ifndef HINAPE_CUBBYFLOW_CUBBYHDKCLASSGENERATOR_H
#define HINAPE_CUBBYFLOW_CUBBYHDKCLASSGENERATOR_H

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

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#define NEW_GETSET_PARAMETER(NAME, GETSET_TYPE) \
GETSET_TYPE(#NAME, NAME)

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

#define NEW_CUBBY_MICRPSOLVER_CLASS(NAME, ...) \
class GAS_CF_##NAME : public GAS_SubSolver \
{ \
public: \
static const char *DATANAME; \
__VA_ARGS__ \
protected: \
GAS_CF_##NAME(const SIM_DataFactory *factory) : BaseClass(factory) {} \
~GAS_CF_##NAME() override = default; \
bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override; \
void initializeSubclass() override; \
void makeEqualSubclass(const SIM_Data *source) override; \
static const SIM_DopDescription *getDopDescription(); \
DECLARE_STANDARD_GETCASTTOTYPE(); \
DECLARE_DATAFACTORY(GAS_CF_##NAME, GAS_SubSolver, "CF_"#NAME, getDopDescription());                    \
private: \
void _init(); \
void _makeEqual(const GAS_CF_##NAME *src); \
bool _solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const; \
};

#define NEW_CUBBY_MICRPSOLVER_IMPLEMENT(NAME, UNIQUE, ...) \
bool GAS_CF_##NAME::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) \
{ \
    UT_WorkBuffer error_msg; \
    if (!_solve(engine, obj, time, timestep, error_msg) || UTisstring(error_msg.buffer())) \
    { \
        SIM_Data::addError(obj, SIM_MESSAGE, error_msg.buffer(), UT_ERROR_ABORT); \
        return false; \
    } \
    return true; \
} \
void GAS_CF_##NAME::initializeSubclass() \
{ \
    SIM_Data::initializeSubclass(); \
    _init(); \
} \
void GAS_CF_##NAME::makeEqualSubclass(const SIM_Data *source) \
{ \
    SIM_Data::makeEqualSubclass(source); \
    const GAS_CF_##NAME *src = SIM_DATA_CASTCONST(source, GAS_CF_##NAME); \
    _makeEqual(src); \
} \
const char *GAS_CF_##NAME::DATANAME = "CF_"#NAME; \
const SIM_DopDescription *GAS_CF_##NAME::getDopDescription() \
{ \
static std::vector<PRM_Template> PRMS; \
__VA_ARGS__ \
PRMS.emplace_back(); \
static SIM_DopDescription DESC(true, \
                               "cf_"#NAME, \
                               "CF "#NAME, \
                               DATANAME, \
                               classname(), \
                               PRMS.data()); \
DESC.setDefaultUniqueDataName(UNIQUE); \
setGasDescription(DESC); \
return &DESC; \
}

#define NEW_CUBBY_DATA_CLASS(NAME, ...) \
class SIM_CF_##NAME : public SIM_Data, public SIM_OptionsUser \
{ \
public: \
static const char *DATANAME; \
__VA_ARGS__ \
protected: \
SIM_CF_##NAME(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {} \
~SIM_CF_##NAME() override = default; \
void initializeSubclass() override; \
void makeEqualSubclass(const SIM_Data *source) override; \
static const SIM_DopDescription *getDopDescription(); \
DECLARE_STANDARD_GETCASTTOTYPE(); \
DECLARE_DATAFACTORY(SIM_CF_##NAME, SIM_Data, "CF_"#NAME, getDopDescription());                    \
private: \
void _init(); \
void _makeEqual(const SIM_CF_##NAME *src); \
};

#define NEW_CUBBY_DATA_IMPLEMENT(NAME, UNIQUE, ...) \
void SIM_CF_##NAME::initializeSubclass() \
{ \
    SIM_Data::initializeSubclass(); \
    _init(); \
} \
void SIM_CF_##NAME::makeEqualSubclass(const SIM_Data *source) \
{ \
    SIM_Data::makeEqualSubclass(source); \
    const SIM_CF_##NAME *src = SIM_DATA_CASTCONST(source, SIM_CF_##NAME); \
    _makeEqual(src); \
} \
const char *SIM_CF_##NAME::DATANAME = "CF_"#NAME; \
const SIM_DopDescription *SIM_CF_##NAME::getDopDescription() \
{ \
static std::vector<PRM_Template> PRMS; \
__VA_ARGS__ \
PRMS.emplace_back(); \
static SIM_DopDescription DESC(true, \
                               "cf_"#NAME, \
                               "CF "#NAME, \
                               DATANAME, \
                               classname(), \
                               PRMS.data()); \
DESC.setDefaultUniqueDataName(UNIQUE); \
return &DESC; \
}

#endif //HINAPE_CUBBYFLOW_CUBBYHDKCLASSGENERATOR_H
