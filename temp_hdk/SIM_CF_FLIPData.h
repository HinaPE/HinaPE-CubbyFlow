#ifndef HINAPE_CUBBYFLOW_SIM_CF_GRIDSYSTEMDATA_H
#define HINAPE_CUBBYFLOW_SIM_CF_GRIDSYSTEMDATA_H

#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataUtils.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Utils.h>
#include <UT/UT_WorkBuffer.h>

#include <Core/Grid/GridSystemData.hpp>

class SIM_CF_FLIPData : public SIM_Data, public SIM_OptionsUser
{
public:
	static const char *DATANAME;
	static constexpr int OPTIONS_SIZE = 13; // [MAKE SURE THIS VALUE IS ACCURATE]
	bool Configured = false;
	CubbyFlow::GridSystemData3Ptr InnerPtr;

protected:
	SIM_CF_FLIPData(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {}
	~SIM_CF_FLIPData() override = default;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *GetDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_CF_FLIPData, SIM_Data, "CF_FLIPData", GetDescription());
};

#endif //HINAPE_CUBBYFLOW_SIM_CF_GRIDSYSTEMDATA_H
