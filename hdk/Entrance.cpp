#include <UT/UT_DSOVersion.h> // Very Important!!! Include this!!!

#include "Particle/SIM_CF_ParticleSystemData.h"
#include "Particle/SIM_CF_SPHSystemData.h"
#include "Particle/GAS_CF_ConfigureParticleSystemData.h"
#include "Particle/GAS_CF_ConfigureSPHSystemData.h"
#include "Particle/GAS_CF_UpdateToGeometrySheet.h"

#include "Emitter/GAS_CF_PointEmitter.h"
#include "Emitter/GAS_CF_VolumeParticleEmitter.h"

#include "Geometry/SIM_CF_Sphere.h"

void initializeSIM(void *)
{
	IMPLEMENT_DATAFACTORY(SIM_CF_ParticleSystemData);
	IMPLEMENT_DATAFACTORY(SIM_CF_SPHSystemData);
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureParticleSystemData);
	IMPLEMENT_DATAFACTORY(GAS_CF_ConfigureSPHSystemData);
	IMPLEMENT_DATAFACTORY(GAS_CF_UpdateToGeometrySheet);

	IMPLEMENT_DATAFACTORY(GAS_CF_PointEmitter);
	IMPLEMENT_DATAFACTORY(GAS_CF_VolumeParticleEmitter);

	IMPLEMENT_DATAFACTORY(SIM_CF_Sphere);
}
