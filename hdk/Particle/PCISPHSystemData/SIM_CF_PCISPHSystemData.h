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

	GETSET_DATA_FUNCS_F("MaxDensityErrorRatio", MaxDensityErrorRatio)
	GETSET_DATA_FUNCS_F("MaxNumberOfIterations", MaxNumberOfIterations)

protected:
	SIM_CF_PCISPHSystemData(const SIM_DataFactory *factory) : SIM_CF_SPHSystemData(factory) {}
	~SIM_CF_PCISPHSystemData() override = default;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *GetDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_CF_PCISPHSystemData, SIM_CF_SPHSystemData, "CF_PCISPHSystemData", GetDescription());
};

#endif //HINAPE_CUBBYFLOW_SIM_CF_PCISPHSYSTEMDATA_H
