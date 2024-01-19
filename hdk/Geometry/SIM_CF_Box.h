#ifndef HINAPE_CUBBYFLOW_SIM_CF_BOX_H
#define HINAPE_CUBBYFLOW_SIM_CF_BOX_H

#include <SIM/SIM_Geometry.h>

#include "Core/Geometry/Box.hpp"

class SIM_CF_Box : public SIM_Geometry
{
public:
	static const char *DATANAME;
	mutable GU_DetailHandle my_detail_handle;

	GETSET_DATA_FUNCS_V3("Center", Center)
	GETSET_DATA_FUNCS_V3("Extent", Extent)
	GETSET_DATA_FUNCS_V3("Rotation", Rotation)
	GETSET_DATA_FUNCS_B("IsNormalFlipped", IsNormalFlipped)

	CubbyFlow::Box3Ptr RuntimeConstructCFBox() const;

protected:
	explicit SIM_CF_Box(const SIM_DataFactory *factory) : BaseClass(factory) {};
	~SIM_CF_Box() override = default;
	GU_ConstDetailHandle getGeometrySubclass() const override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();


DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_CF_Box,
					SIM_Geometry,
					"CF_Box",
					getDopDescription());
};

#endif //HINAPE_CUBBYFLOW_SIM_CF_BOX_H
