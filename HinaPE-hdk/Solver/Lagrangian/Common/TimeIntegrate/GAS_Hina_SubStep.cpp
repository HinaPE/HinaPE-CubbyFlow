#include "GAS_Hina_SubStep.h"
#include <Particle/SIM_Hina_ParticleFluidData.h>
#include <Particle/SIM_Hina_ConfigureForSPH.h>

SIM_Solver::SIM_Result GAS_Hina_SubStep::solveObjectsSubclass(SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep)
{
	CubbyFlow::Logging::Mute();
	SIM_Object *obj = nullptr;
	SIM_Hina_ParticleFluidData *data = nullptr;
	SIM_Hina_ConfigureForSPH *configure = nullptr;

	double sub_time = timestep;

	// Find Fluid Data (ONLY [ONE] SUPPORTED)
	auto obj_size = objects.entries();
	for (int obj_idx = 0; obj_idx < obj_size; ++obj_idx)
	{
		obj = objects(obj_idx);
		data = SIM_DATA_GET(*obj, SIM_Hina_ParticleFluidData::DATANAME, SIM_Hina_ParticleFluidData);
		if (!data) continue;
		break;
	}

	if (!obj || !data)
		return SIM_Result::SIM_SOLVER_FAIL;

	configure = SIM_DATA_GET(*data, SIM_Hina_ConfigureForSPH::DATANAME, SIM_Hina_ConfigureForSPH);
	// If not Found, then Attach Configure
	if (!configure)
		configure = SIM_DATA_CREATE(*data, SIM_Hina_ConfigureForSPH::DATANAME, SIM_Hina_ConfigureForSPH,
									SIM_DATA_RETURN_EXISTING | SIM_DATA_ADOPT_EXISTING_ON_DELETE);

	if (!data->Configured && configure)
		configure->InitConfigure(obj, data);
	sub_time = data->calculate_dynamic_dt();
	int sub_step = std::ceil(timestep / sub_time);

	// Limit Max substeps
	int max_substeps = getMAX_SUBSTEP();
	if (sub_step > max_substeps)
	{
		sub_step = max_substeps;
		sub_time = timestep / (double) sub_step;
	}

	std::cout << "Time Step: " << timestep << "\tSub Step: " << sub_step << "\tSub Time: " << sub_time << '\n';

	for (int i = 0; i < sub_step; ++i)
	{
		data->substep_num = i;
		configure->UpdateConfigure(obj, data, engine.getSimulationTime() + i * sub_time, sub_time); // TODO: Not Checked
		SIM_Solver::SIM_Result res = GAS_SubStep::solveObjectsSubclass(engine, objects, newobjects, feedbacktoobjects, sub_time);
		if (res != SIM_Solver::SIM_SOLVER_SUCCESS)
			return res;
	}
	return SIM_Solver::SIM_SOLVER_SUCCESS;
}

const SIM_DopDescription *GAS_Hina_SubStep::getDopDescription()
{
	static PRM_Name MAX_SUBSTEP("MAX_SUBSTEP", "MAX_SUBSTEP");
	static PRM_Default DefaultMAX_SUBSTEP{20};

	static std::array<PRM_Template, 2> PRMS{
			PRM_Template(PRM_INT, 1, &MAX_SUBSTEP, &DefaultMAX_SUBSTEP),
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
