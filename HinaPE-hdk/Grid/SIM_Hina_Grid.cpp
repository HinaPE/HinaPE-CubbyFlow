#include "SIM_Hina_Grid.h"

NEW_HINA_DATA_IMPLEMENT(
		Grid,
		false,
)

void SIM_Hina_Grid::_init()
{
	this->InnerPtr = nullptr;
}
void SIM_Hina_Grid::_makeEqual(const SIM_Hina_Grid *src)
{
	this->InnerPtr = src->InnerPtr;
}
