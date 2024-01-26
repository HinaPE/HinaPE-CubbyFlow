#include "SIM_Hina_SPHCaches.h"

NEW_HINA_DATA_IMPLEMENT_NO_NODE(
		SPHCaches,
		false,
)

void SIM_Hina_SPHCaches::_init()
{
	newPositions_Cache.Clear();
	newVelocities_Cache.Clear();
}
void SIM_Hina_SPHCaches::_makeEqual(const SIM_Hina_SPHCaches *src)
{
	newPositions_Cache.Clear();
	newVelocities_Cache.Clear();
}
