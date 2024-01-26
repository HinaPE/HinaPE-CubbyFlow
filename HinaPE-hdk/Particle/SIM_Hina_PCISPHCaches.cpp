#include "SIM_Hina_PCISPHCaches.h"

NEW_HINA_DATA_IMPLEMENT_NO_NODE(
		PCISPHCaches,
		false,
)

void SIM_Hina_PCISPHCaches::_init()
{
	newPositions_Cache.Clear();
	newVelocities_Cache.Clear();
	tempPositions_Cache.Clear();
	tempVelocities_Cache.Clear();
	pressureForces_Cache.Clear();
	densityErrors_Cache.Clear();
}
void SIM_Hina_PCISPHCaches::_makeEqual(const SIM_Hina_PCISPHCaches *src)
{
	this->newPositions_Cache.Clear(); // We don't need this cache
	this->newVelocities_Cache.Clear(); // We don't need this cache
	tempPositions_Cache.Clear(); // We don't need this cache
	tempVelocities_Cache.Clear(); // We don't need this cache
	pressureForces_Cache.Clear(); // We don't need this cache
	densityErrors_Cache.Clear(); // We don't need this cache
}
