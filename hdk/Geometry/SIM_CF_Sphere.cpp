#include "SIM_CF_Sphere.h"

#include <SIM/SIM_Engine.h>
#include <SIM/SIM_DopDescription.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ObjectArray.h>
#include <SIM/SIM_Geometry.h>
#include <SIM/SIM_GeometryCopy.h>
#include <SIM/SIM_GuideShared.h>
#include <SIM/SIM_ColliderLabel.h>
#include <SIM/SIM_ForceGravity.h>
#include <SIM/SIM_Time.h>
#include <SIM/SIM_Utils.h>

#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>
#include <PRM/PRM_Shared.h>
#include <PRM/PRM_Default.h>
#include <PRM/PRM_Utils.h>
#include <PRM/PRM_SpareData.h>

#include <UT/UT_WorkBuffer.h>
#include <UT/UT_NetMessage.h>

#include <GEO/GEO_PrimSphere.h>
#include <GU/GU_PrimSphere.h>

GU_ConstDetailHandle SIM_CF_Sphere::getGeometrySubclass() const
{
	if (my_detail_handle.isNull())
	{
		GU_Detail *gdp = new GU_Detail();
		my_detail_handle.allocateAndSet(gdp);

		UT_Vector3 Center = getCenter();
		fpreal Radius = getRadius();

		GA_Offset center_point_offset = gdp->appendPoint();
		gdp->setPos3(center_point_offset, Center);

		GU_PrimSphereParms params;
		params.gdp = gdp;
		params.ptoff = center_point_offset;
		params.xform.scale(Radius, Radius, Radius);
		params.xform.translate(Center);
		GEO_PrimSphere *sphere_prim = (GEO_PrimSphere *) GU_PrimSphere::build(params);
	}

	return my_detail_handle;
}

void SIM_CF_Sphere::initializeSubclass()
{
	SIM_Geometry::initializeSubclass();

	// IMPORTANT!!!
	// NEVER CALL GET_SET FUNCTION HERE!!!

	/// Implement Initializations of Your Custom Fields
	this->my_detail_handle.clear();
}

void SIM_CF_Sphere::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Geometry::makeEqualSubclass(source);
	const SIM_CF_Sphere *src = SIM_DATA_CASTCONST(source, SIM_CF_Sphere);

	/// Implement Equal Operator of Your Custom Fields
	this->my_detail_handle = src->my_detail_handle;
}

const char *SIM_CF_Sphere::DATANAME = "CF_Sphere";
const SIM_DopDescription *SIM_CF_Sphere::getDopDescription()
{
	static PRM_Name Center("Center", "Center");
	static std::array<PRM_Default, 3> CenterDefault{0, 0, 0};

	static PRM_Name Radius("Radius", "Radius");
	static PRM_Default RadiusDefault(0.5);

	static PRM_Name IsNormalFlipped("IsNormalFlipped", "IsNormalFlipped");

	static std::array<PRM_Template, 4> PRMS{
			PRM_Template(PRM_FLT_J, 3, &Center, CenterDefault.data()),
			PRM_Template(PRM_FLT, 1, &Radius, &RadiusDefault),
			PRM_Template(PRM_TOGGLE, 1, &IsNormalFlipped, PRMzeroDefaults),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_sphere",
								   "CF Sphere",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	return &DESC;
}

CubbyFlow::Sphere3Ptr SIM_CF_Sphere::RuntimeConstructCFSphere() const
{
	UT_Vector3 Center = getCenter();
	fpreal Radius = getRadius();
	bool IsNormalFlipped = getIsNormalFlipped();

	return CubbyFlow::Sphere3::GetBuilder()
			.WithCenter({Center.x(), Center.y(), Center.z()})
			.WithRadius(Radius)
			.WithIsNormalFlipped(IsNormalFlipped)
			.MakeShared();
}
