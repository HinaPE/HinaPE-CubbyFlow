#ifndef HINAPE_CUBBYFLOW_GAS_CF_POINTEMITTER_H
#define HINAPE_CUBBYFLOW_GAS_CF_POINTEMITTER_H

#include <GAS/GAS_SubSolver.h>
#include <PRM/PRM_Default.h>

#include "Core/Emitter/PointParticleEmitter3.hpp"

class GAS_CF_PointEmitter : public GAS_SubSolver
{
public:
	static const char *DATANAME;
	CubbyFlow::PointParticleEmitter3Ptr InnerPtr;

	GETSET_DATA_FUNCS_V3("Origin", Origin)
	GETSET_DATA_FUNCS_V3("Direction", Direction)
	GETSET_DATA_FUNCS_F("Speed", Speed)
	GETSET_DATA_FUNCS_F("SpreadAngleInDegrees", SpreadAngleInDegrees)
	GETSET_DATA_FUNCS_I("MaxNumberOfNewParticlesPerSecond", MaxNumberOfNewParticlesPerSecond)
	GETSET_DATA_FUNCS_I("MaxNumberOfParticles", MaxNumberOfParticles)
	GETSET_DATA_FUNCS_I("RandomSeed", RandomSeed)

protected:
	GAS_CF_PointEmitter(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_CF_PointEmitter() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_CF_PointEmitter,
					GAS_SubSolver,
					"CF ParticleEmitter SubSolver",
					getDopDescription());

private:
	bool InitRuntime(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg);
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const;
};

#endif //HINAPE_CUBBYFLOW_GAS_CF_POINTEMITTER_H
