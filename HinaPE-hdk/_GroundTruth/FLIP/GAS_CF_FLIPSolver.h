#ifndef HINAPE_CUBBYFLOW_GAS_CF_FLIPSOVER_H
#define HINAPE_CUBBYFLOW_GAS_CF_FLIPSOVER_H

#include <GAS/GAS_SubSolver.h>

#include "Core/Animation/Frame.hpp"

class GAS_CF_FLIPSolver : public GAS_SubSolver
{
public:
	static const char *DATANAME;
	CubbyFlow::Frame frame;

protected:
	GAS_CF_FLIPSolver(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_CF_FLIPSolver() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_CF_FLIPSolver,
					GAS_SubSolver,
					"CF_FLIPSolver",
					getDopDescription());

private:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg);
};

#endif //HINAPE_CUBBYFLOW_GAS_CF_FLIPSOVER_H
