#ifndef HINAPE_CUBBYFLOW_SIM_CF_PARTICLESYSTEMDATA_H
#define HINAPE_CUBBYFLOW_SIM_CF_PARTICLESYSTEMDATA_H

#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataUtils.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>
#include <UT/UT_WorkBuffer.h>

#include "Core/Particle/ParticleSystemData.hpp"

class SIM_CF_ParticleSystemData : public SIM_Data, public SIM_OptionsUser
{
public:
	static const char *DATANAME;
	static const char *CL_PT_IDX_ATTRIBUTE_NAME;
	static const char *FORCE_ATTRIBUTE_NAME;
	bool Configured = false;
	CubbyFlow::ParticleSystemData3Ptr InnerPtr;
	size_t scalar_idx_offset = -1;
	size_t scalar_idx_state = -1;

	GA_Offset GetParticleOffset(size_t index, UT_WorkBuffer &error_msg) const;
	void SetParticleOffset(size_t index, GA_Offset offset, UT_WorkBuffer &error_msg) const;

	enum ParticleState
	{
		PARTICLE_ADDED = 0, // default value
		PARTICLE_DELETED = 1,
		PARTICLE_CLEAN = 2,
		PARTICLE_DIRTY = 3,
		PARTICLE_INVALID = 99,
	};
	ParticleState GetParticleState(size_t index, UT_WorkBuffer &error_msg) const;
	void SetParticleState(size_t index, ParticleState state, UT_WorkBuffer &error_msg) const;

	GETSET_DATA_FUNCS_V3("ParticlesDomain", ParticlesDomain)
	GETSET_DATA_FUNCS_F("ParticleRadius", ParticleRadius)
	GETSET_DATA_FUNCS_F("TargetSpacing", TargetSpacing)

	GET_GUIDE_FUNC_B(SIM_NAME_SHOWGUIDE, ShowGuideGeometry, true);
	GET_GUIDE_FUNC_V3("DomainColor", DomainColor, (.0156356, 0, .5))

protected:
	SIM_CF_ParticleSystemData(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {}
	~SIM_CF_ParticleSystemData() override = default;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *GetDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;

DECLARE_STANDARD_GETCASTTOTYPE();
	DECLARE_DATAFACTORY(SIM_CF_ParticleSystemData, SIM_Data, "CF_ParticleSystemData", GetDescription());
};

#endif //HINAPE_CUBBYFLOW_SIM_CF_PARTICLESYSTEMDATA_H
