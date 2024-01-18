#ifndef HINAPE_CUBBYFLOW_GAS_CF_UPDATETOGEOMETRYSHEET_H
#define HINAPE_CUBBYFLOW_GAS_CF_UPDATETOGEOMETRYSHEET_H

#include <GAS/GAS_SubSolver.h>
#include <PRM/PRM_Default.h>

#include "SIM_CF_ParticleSystemData.h"
#include "SIM_CF_SPHSystemData.h"

class GAS_CF_UpdateToGeometrySheet : public GAS_SubSolver
{
public:
	static const char *DATANAME;

protected:
	GAS_CF_UpdateToGeometrySheet(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_CF_UpdateToGeometrySheet() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_CF_UpdateToGeometrySheet,
					GAS_SubSolver,
					"CF UpdateToGeometrySheet SubSolver",
					getDopDescription());

private:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg);
};

#endif //HINAPE_CUBBYFLOW_GAS_CF_UPDATETOGEOMETRYSHEET_H
