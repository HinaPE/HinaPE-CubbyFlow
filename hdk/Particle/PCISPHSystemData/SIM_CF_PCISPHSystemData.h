#ifndef HINAPE_CUBBYFLOW_SIM_CF_PCISPHSYSTEMDATA_H
#define HINAPE_CUBBYFLOW_SIM_CF_PCISPHSYSTEMDATA_H

#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>
#include <UT/UT_WorkBuffer.h>

#include <Particle/SPHSystemData/SIM_CF_SPHSystemData.h>

/**
 * PCISPH Solver also Use SPHSystemData3Ptr
 */
class SIM_CF_PCISPHSystemData : public SIM_CF_SPHSystemData
{
public:
	static const char *DATANAME;
	CubbyFlow::ParticleSystemData3::VectorData tempPositions_Cache;
	CubbyFlow::ParticleSystemData3::VectorData tempVelocities_Cache;
	CubbyFlow::ParticleSystemData3::VectorData pressureForces_Cache;
	CubbyFlow::ParticleSystemData3::VectorData densityErrors_Cache;

	// From Parent
	GETSET_DATA_FUNCS_V3("FluidDomain", FluidDomain)
//	GETSET_DATA_FUNCS_F("ParticleRadius", ParticleRadius)
	GETSET_DATA_FUNCS_F("TargetDensity", TargetDensity)
	GETSET_DATA_FUNCS_F("TargetSpacing", TargetSpacing)
	GETSET_DATA_FUNCS_F("KernelRadiusOverTargetSpacing", KernelRadiusOverTargetSpacing)
//	GETSET_DATA_FUNCS_F("KernelRadius", KernelRadius)
	GETSET_DATA_FUNCS_V3("Gravity", Gravity)
	GETSET_DATA_FUNCS_F("RestitutionCoefficient", RestitutionCoefficient)
	GETSET_DATA_FUNCS_F("EosExponent", EosExponent)
	GETSET_DATA_FUNCS_F("NegativePressureScale", NegativePressureScale)
	GETSET_DATA_FUNCS_F("ViscosityCoefficient", ViscosityCoefficient)
	GETSET_DATA_FUNCS_F("PseudoViscosityCoefficient", PseudoViscosityCoefficient)
	GETSET_DATA_FUNCS_F("SpeedOfSound", SpeedOfSound)
	GETSET_DATA_FUNCS_F("TimeStepLimitScale", TimeStepLimitScale)

	GET_GUIDE_FUNC_B(SIM_NAME_SHOWGUIDE, ShowGuideGeometry, true);
	GET_GUIDE_FUNC_V3("DomainColor", DomainColor, (.0156356, 0, .5))

	// This Class
	GETSET_DATA_FUNCS_F("MaxDensityErrorRatio", MaxDensityErrorRatio)
	GETSET_DATA_FUNCS_F("MaxNumberOfIterations", MaxNumberOfIterations)

protected:
	SIM_CF_PCISPHSystemData(const SIM_DataFactory *factory) : SIM_CF_SPHSystemData(factory) {}
	~SIM_CF_PCISPHSystemData() override = default;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *GetDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_CF_PCISPHSystemData, SIM_CF_SPHSystemData, "CF_PCISPHSystemData", GetDescription());
};

#endif //HINAPE_CUBBYFLOW_SIM_CF_PCISPHSYSTEMDATA_H
