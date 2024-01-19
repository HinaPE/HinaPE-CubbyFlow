#ifndef HINAPE_CUBBYFLOW_GAS_CF_BUILDNEIGHBORLISTS_H
#define HINAPE_CUBBYFLOW_GAS_CF_BUILDNEIGHBORLISTS_H

#include <GAS/GAS_SubSolver.h>
#include <PRM/PRM_Default.h>

class GAS_CF_BuildNeighborLists : public GAS_SubSolver
{
public:
	static const char *DATANAME;

protected:
	GAS_CF_BuildNeighborLists(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_CF_BuildNeighborLists() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_CF_BuildNeighborLists,
					GAS_SubSolver,
					"CF_BuildNeighborLists",
					getDopDescription());

private:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const;
};

#endif //HINAPE_CUBBYFLOW_GAS_CF_BUILDNEIGHBORLISTS_H
