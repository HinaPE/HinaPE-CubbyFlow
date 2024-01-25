#include "SIM_Hina_Sphere.h"

NEW_HINA_GEOMETRY_IMPLEMENT(
		Sphere,
		CubbyFlow::Sphere3Ptr,
		false,
		NEW_FLOAT_VECTOR_PARAMETER(Center, 3, 0, 0, 0) \
        NEW_FLOAT_PARAMETER(Radius, .5) \
        NEW_BOOL_PARAMETER(IsNormalFlipped, false) \
)

void SIM_Hina_Sphere::_init() {}
void SIM_Hina_Sphere::_makeEqual(const SIM_Hina_Sphere *src) {}
CubbyFlow::Sphere3Ptr SIM_Hina_Sphere::RuntimeConstruct() const
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
GU_ConstDetailHandle SIM_Hina_Sphere::getGeometrySubclass() const
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
