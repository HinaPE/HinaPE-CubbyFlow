#ifndef HINAPE_CUBBYFLOW_SIM_CF_PARTICLESYSTEMDATA_H
#define HINAPE_CUBBYFLOW_SIM_CF_PARTICLESYSTEMDATA_H

#include <SIM/SIM_SingleSolver.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>
#include <UT/UT_WorkBuffer.h>

#include "Core/Particle/ParticleSystemData.hpp"

class SIM_CF_ParticleSystemData : public SIM_Data, public SIM_OptionsUser, public CubbyFlow::ParticleSystemData3
{
public:
	static const char *DATANAME;
	static const char *FORCE_ATTRIBUTE_NAME;
	bool Configured = false;

protected:
	SIM_CF_ParticleSystemData(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {}
	~SIM_CF_ParticleSystemData() override = default;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *GetDescription();

	DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_CF_ParticleSystemData, SIM_Data, "CubbyFlow ParticleSystemData", GetDescription());
};

#endif //HINAPE_CUBBYFLOW_SIM_CF_PARTICLESYSTEMDATA_H
