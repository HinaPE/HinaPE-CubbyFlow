#ifndef HINAPE_CUBBYFLOW_SIM_CF_PLANE_H
#define HINAPE_CUBBYFLOW_SIM_CF_PLANE_H

#include <SIM/SIM_Geometry.h>

#include "Core/Geometry/Plane.hpp"

/**
 * [Notice]
 * - CF Plane Now has some BUGs!
 */

class SIM_CF_Plane : public SIM_Geometry
{
public:
	static const char *DATANAME;
	mutable GU_DetailHandle my_detail_handle;

	GETSET_DATA_FUNCS_V3("Center", Center)
	GETSET_DATA_FUNCS_V3("Normal", Normal)
	GETSET_DATA_FUNCS_B("IsNormalFlipped", IsNormalFlipped)

	CubbyFlow::Plane3Ptr RuntimeConstructCFPlane() const;

protected:
	explicit SIM_CF_Plane(const SIM_DataFactory *factory) : BaseClass(factory) {};
	~SIM_CF_Plane() override = default;
	GU_ConstDetailHandle getGeometrySubclass() const override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();

DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_CF_Plane,
					SIM_Geometry,
					"CF_Plane",
					getDopDescription());
};

#endif //HINAPE_CUBBYFLOW_SIM_CF_PLANE_H