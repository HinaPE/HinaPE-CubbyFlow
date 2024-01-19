#ifndef HINAPE_CUBBYFLOW_GAS_CF_VOLUMEPARTICLEEMITTER_H
#define HINAPE_CUBBYFLOW_GAS_CF_VOLUMEPARTICLEEMITTER_H

#include <GAS/GAS_SubSolver.h>
#include <PRM/PRM_Default.h>

#include "Core/Emitter/VolumeParticleEmitter3.hpp"

class GAS_CF_VolumeParticleEmitter : public GAS_SubSolver
{
public:
	static const char *DATANAME;
	CubbyFlow::VolumeParticleEmitter3Ptr InnerPtr;

	GETSET_DATA_FUNCS_B("IsOneShot", IsOneShot)
	GETSET_DATA_FUNCS_V3("MaxRegion", MaxRegion)
	GETSET_DATA_FUNCS_F("Spacing", Spacing)
	GETSET_DATA_FUNCS_F("MaxNumberOfParticles", MaxNumberOfParticles)
	GETSET_DATA_FUNCS_F("RandomSeed", RandomSeed)

protected:
	GAS_CF_VolumeParticleEmitter(const SIM_DataFactory *factory) : BaseClass(factory) {}
	~GAS_CF_VolumeParticleEmitter() override = default;
	bool solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep) override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

	SIM_Guide *createGuideObjectSubclass() const override;
	void buildGuideGeometrySubclass(const SIM_RootData &root, const SIM_Options &options, const GU_DetailHandle &gdh, UT_DMatrix4 *xform, const SIM_Time &t) const override;

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(GAS_CF_VolumeParticleEmitter,
					GAS_SubSolver,
					"CF VolumeParticleEmitter SubSolver",
					getDopDescription());

private:
	bool InitRuntime(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg);
	bool Solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const;
};

#endif //HINAPE_CUBBYFLOW_GAS_CF_VOLUMEPARTICLEEMITTER_H
