#include "GAS_Hina_SubStep.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>

SIM_Solver::SIM_Result GAS_Hina_SubStep::solveObjectsSubclass(SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep)
{
	CubbyFlow::Logging::Mute();

	double sub_time = 1e-2;

	int obj_size = objects.entries();
	for (int obj_idx = 0; obj_idx < obj_size; ++obj_idx)
	{
		auto *obj = objects(obj_idx);
		SIM_Hina_ParticleFluidData *data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
		if (data)
		{
			sub_time = data->dynamic_dt;
			break;
		}
	}
	int sub_step = std::ceil(timestep / sub_time);

	for (int i = 0; i < sub_step; ++i)
	{
		SIM_Solver::SIM_Result res = GAS_SubStep::solveObjectsSubclass(engine, objects, newobjects, feedbacktoobjects, sub_time);
		if (res != SIM_Solver::SIM_SOLVER_SUCCESS)
			return res;
	}
	return SIM_Solver::SIM_SOLVER_SUCCESS;
}

const SIM_DopDescription *GAS_Hina_SubStep::getDopDescription()
{
	static std::array<PRM_Template, 1> PRMS{
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "hina_substep",
								   "Hina SubStep",
								   "Hina_SubStep",
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
//	setGasDescription(DESC);
	return &DESC;
}
