#ifndef HINAPE_CUBBYFLOW_GAS_CF_UPDATETOGEOMETRYSHEET_H
#define HINAPE_CUBBYFLOW_GAS_CF_UPDATETOGEOMETRYSHEET_H

#include <GAS/GAS_SubSolver.h>
#include <PRM/PRM_Default.h>

/**
 * [IMPORTANT]
 * [DEPRECATED]
 * - 请不要过度依赖这个类，这种强制大范围更新所有数据的操作是很难维护的，预计将在不久的将来移除这个类。
 * - 作为替代，预计将会把所有节点分为会增加粒子的节点（比如Emitter）和不会增加粒子的节点（比如普通的solver），然后单独处理。
 */
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
					"CF_UpdateToGeometrySheet",
					getDopDescription());

private:
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg);
};

#endif //HINAPE_CUBBYFLOW_GAS_CF_UPDATETOGEOMETRYSHEET_H
