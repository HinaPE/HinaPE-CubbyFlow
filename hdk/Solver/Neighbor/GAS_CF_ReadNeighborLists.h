#ifndef HINAPE_CUBBYFLOW_GAS_CF_READNEIGHBORLISTS_H
#define HINAPE_CUBBYFLOW_GAS_CF_READNEIGHBORLISTS_H

#include <GAS/GAS_SubSolver.h>

// Instead of Computing the Neighbor List, just Read it!

class GAS_CF_ReadNeighborLists : public GAS_SubSolver
{
public:
	static const char *DATANAME;

protected:
	GAS_CF_ReadNeighborLists(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_CF_ReadNeighborLists() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_CF_ReadNeighborLists,
					GAS_SubSolver,
					"CF_ReadNeighborLists",
					getDopDescription());

private:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const;
};
#endif //HINAPE_CUBBYFLOW_GAS_CF_READNEIGHBORLISTS_H
