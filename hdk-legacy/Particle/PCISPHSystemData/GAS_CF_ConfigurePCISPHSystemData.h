#ifndef HINAPE_CUBBYFLOW_GAS_CF_CONFIGUREPCISPHSYSTEMDATA_H
#define HINAPE_CUBBYFLOW_GAS_CF_CONFIGUREPCISPHSYSTEMDATA_H

#include <Particle/SPHSystemData/GAS_CF_ConfigureSPHSystemData.h>

class GAS_CF_ConfigurePCISPHSystemData : public GAS_CF_ConfigureSPHSystemData
{
public:
	static const char *DATANAME;

protected:
	GAS_CF_ConfigurePCISPHSystemData(const SIM_DataFactory *factory) : GAS_CF_ConfigureSPHSystemData(factory) {}
	~GAS_CF_ConfigurePCISPHSystemData() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_CF_ConfigurePCISPHSystemData,
					GAS_CF_ConfigureSPHSystemData,
					"CF_ConfigurePCISPHSystemData",
					getDopDescription());

private:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg);
};

#endif //HINAPE_CUBBYFLOW_GAS_CF_CONFIGUREPCISPHSYSTEMDATA_H
