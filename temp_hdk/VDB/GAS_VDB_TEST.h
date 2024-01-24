#ifndef HINAPE_CUBBYFLOW_GAS_VDB_TEST_H
#define HINAPE_CUBBYFLOW_GAS_VDB_TEST_H

#include <GAS/GAS_SubSolver.h>
#include <GAS/GAS_Utils.h>

#include <UT/UT_ThreadedAlgorithm.h>

class GAS_VDB_TEST : public GAS_SubSolver
{
protected:
	GAS_VDB_TEST(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_VDB_TEST() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	static const SIM_DopDescription *getDopDescription();

	bool shouldMultiThread(const SIM_RawField *field) const { return field->field()->numTiles() > 1; }
	THREADED_METHOD2(SIM_GasAdd, shouldMultiThread(dst),
					 addFields,
					 SIM_RawField *, dst,
					 const SIM_RawField *, src);
	void addFieldsPartial(SIM_RawField *dst, const SIM_RawField *src, const UT_JobInfo &info);

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_VDB_TEST,
					GAS_SubSolver,
					"MY_VDB_TEST",
					getDopDescription());
};

#endif //HINAPE_CUBBYFLOW_GAS_VDB_TEST_H
