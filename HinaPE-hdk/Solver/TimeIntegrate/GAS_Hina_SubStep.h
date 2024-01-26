#ifndef HINAPE_GAS_HINA_SUBSTEP_H
#define HINAPE_GAS_HINA_SUBSTEP_H

#include "HinaHDKClassGenerator.h"
#include <GAS/GAS_SubStep.h>

class GAS_Hina_SubStep : public GAS_SubStep
{
public:
	SIM_Result solveObjectsSubclass(SIM_Engine &engine, SIM_ObjectArray &objects, SIM_ObjectArray &newobjects, SIM_ObjectArray &feedbacktoobjects, const SIM_Time &timestep) override;

protected:
	explicit GAS_Hina_SubStep(const SIM_DataFactory *factory) : GAS_SubStep(factory) {}
	~GAS_Hina_SubStep() override = default;
	static const SIM_DopDescription	*getDopDescription();
DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_Hina_SubStep,
					GAS_SubStep,
					"Hina SubStep",
					getDopDescription());
};

#endif //HINAPE_GAS_HINA_SUBSTEP_H
