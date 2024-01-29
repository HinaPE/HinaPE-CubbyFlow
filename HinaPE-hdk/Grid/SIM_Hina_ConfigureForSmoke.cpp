#include "SIM_Hina_ConfigureForSmoke.h"
#include "Core/Grid/CellCenteredScalarGrid.hpp"

NEW_HINA_DATA_IMPLEMENT_NO_NODE(
		ConfigureForSmoke,
		false,
)

void SIM_Hina_ConfigureForSmoke::_init() {}
void SIM_Hina_ConfigureForSmoke::_makeEqual(const SIM_Hina_ConfigureForSmoke *src) {}
bool SIM_Hina_ConfigureForSmoke::InitConfigure(SIM_Object *obj, SIM_Hina_GridFluidData *data)
{
	CubbyFlow::Logging::Mute();

	CHECK_NULL(obj)
	CHECK_NULL(data)

	if (!data->Configured)
	{
		// Configure Density
		UT_Vector3 Center = data->getCenter();
		UT_Vector3I Size = data->getSize();
		fpreal Division = data->getDivision();

		CubbyFlow::Vector3UZ Resolution;
		Resolution.x = (size_t) ((double) Size.x() / Division);
		Resolution.y = (size_t) ((double) Size.y() / Division);
		Resolution.z = (size_t) ((double) Size.z() / Division);

		CubbyFlow::Vector3D GridSpacing;
		GridSpacing.x = Division;
		GridSpacing.y = Division;
		GridSpacing.z = Division;

		CubbyFlow::Vector3D Origin;
		Origin.x = Center.x() - (double) Size.x() / 2.;
		Origin.y = Center.y() - (double) Size.y() / 2.;
		Origin.z = Center.z() - (double) Size.z() / 2.;

		data->InnerPtr = std::make_shared<CubbyFlow::GridSystemData3>();
		data->InnerPtr->Resize(Resolution, GridSpacing, Origin);
		data->scalar_density_offset = data->InnerPtr->AddAdvectableScalarData(
				std::make_shared<CubbyFlow::CellCenteredScalarGrid3::Builder>(), 0.0);
		data->scalar_temperature_offset = data->InnerPtr->AddAdvectableScalarData(
				std::make_shared<CubbyFlow::CellCenteredScalarGrid3::Builder>(), 0.0);
	}

	return true;
}
bool SIM_Hina_ConfigureForSmoke::UpdateConfigure(SIM_Object *obj, SIM_Hina_GridFluidData *data, SIM_Time current_time, SIM_Time timestep)
{
	CubbyFlow::Logging::Mute();

	CHECK_NULL(obj)
	CHECK_NULL(data)

	return true;
}