#ifndef HINAPE_CUBBYFLOW_SIM_CL_NOLOG_H
#define HINAPE_CUBBYFLOW_SIM_CL_NOLOG_H

#include <SIM/SIM_Data.h>
#include <SIM/SIM_DataUtils.h>
#include <SIM/SIM_OptionsUser.h>
#include <SIM/SIM_DopDescription.h>

class SIM_CL_NoLog : public SIM_Data, public SIM_OptionsUser
{
public:
	static const char *DATANAME;

protected:
	SIM_CL_NoLog(const SIM_DataFactory *factory) : SIM_Data(factory), SIM_OptionsUser(this) {}
	~SIM_CL_NoLog() override = default;
	void initializeSubclass() override;
	static const SIM_DopDescription *GetDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_CL_NoLog, SIM_Data, "CL_NoLog", GetDescription());
};

#endif //HINAPE_CUBBYFLOW_SIM_CL_NOLOG_H
