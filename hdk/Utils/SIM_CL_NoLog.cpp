#include "SIM_CL_NoLog.h"

#include <PRM/PRM_Template.h>

#include "Core/Utils/Logging.hpp"

void SIM_CL_NoLog::initializeSubclass()
{
	SIM_Data::initializeSubclass();

	CubbyFlow::Logging::Mute();
}

const char *SIM_CL_NoLog::DATANAME = "CL_NoLog";
const SIM_DopDescription *SIM_CL_NoLog::GetDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_no_log",
								   "CF No Log",
								   DATANAME,
								   classname(),
								   PRMS.data());
	return &DESC;
}
