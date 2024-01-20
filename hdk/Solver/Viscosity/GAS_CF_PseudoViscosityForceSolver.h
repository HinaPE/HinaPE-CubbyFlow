#ifndef HINAPE_CUBBYFLOW_GAS_CF_PSEUDOVISCOSITYFORCESOLVER_H
#define HINAPE_CUBBYFLOW_GAS_CF_PSEUDOVISCOSITYFORCESOLVER_H

#include <GAS/GAS_SubSolver.h>
#include <PRM/PRM_Default.h>

class GAS_CF_PseudoViscosityForceSolver : public GAS_SubSolver
{
public:
	static const char *DATANAME;

protected:
	GAS_CF_PseudoViscosityForceSolver(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_CF_PseudoViscosityForceSolver() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_CF_PseudoViscosityForceSolver,
					GAS_SubSolver,
					"CF_PseudoViscosityForceSolver",
					getDopDescription());

private:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const;
};

#endif //HINAPE_CUBBYFLOW_GAS_CF_PSEUDOVISCOSITYFORCESOLVER_H
