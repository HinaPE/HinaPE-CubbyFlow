#ifndef HINAPE_CUBBYFLOW_SIM_CF_SPHERE_H
#define HINAPE_CUBBYFLOW_SIM_CF_SPHERE_H

#include <SIM/SIM_Geometry.h>

#include "Core/Geometry/Sphere.hpp"

class SIM_CF_Sphere : public SIM_Geometry
{
public:
	static const char *DATANAME;
	mutable GU_DetailHandle my_detail_handle;

	GETSET_DATA_FUNCS_V3("Center", Center)
	GETSET_DATA_FUNCS_F("Radius", Radius)
	GETSET_DATA_FUNCS_B("IsNormalFlipped", IsNormalFlipped)

	CubbyFlow::Sphere3Ptr RuntimeConstructCFSphere() const;

protected:
	explicit SIM_CF_Sphere(const SIM_DataFactory *factory) : BaseClass(factory) {};
	~SIM_CF_Sphere() override = default;
	GU_ConstDetailHandle getGeometrySubclass() const override;
	void initializeSubclass() override;
	void makeEqualSubclass(const SIM_Data *source) override;
	static const SIM_DopDescription *getDopDescription();


DECLARE_STANDARD_GETCASTTOTYPE();
DECLARE_DATAFACTORY(SIM_CF_Sphere,    // Our Classname
					SIM_Geometry,        // Base type
					"CF_Sphere",    // DOP Data Type
					getDopDescription() // PRM list.
);
};

#endif //HINAPE_CUBBYFLOW_SIM_CF_SPHERE_H
