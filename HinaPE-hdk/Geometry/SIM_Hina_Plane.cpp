#include "SIM_Hina_Plane.h"

NEW_HINA_GEOMETRY_IMPLEMENT(
		Plane,
		CubbyFlow::Plane3Ptr,
		false,
		NEW_FLOAT_VECTOR_PARAMETER(Center, 3, 0, 0, 0) \
        NEW_FLOAT_VECTOR_PARAMETER(Extent, 3, 0, 1, 0) \
        NEW_BOOL_PARAMETER(IsNormalFlipped, false) \
)

void SIM_Hina_Plane::_init() {}
void SIM_Hina_Plane::_makeEqual(const SIM_Hina_Plane *src) {}
CubbyFlow::Plane3Ptr SIM_Hina_Plane::RuntimeConstruct() const
{
	UT_Vector3 Center = getCenter();
	UT_Vector3 Normal = getNormal();
	bool IsNormalFlipped = getIsNormalFlipped();

	return CubbyFlow::Plane3::GetBuilder()
			.WithPoint({Center.x(), Center.y(), Center.z()})
			.WithNormal({Normal.x(), Normal.y(), Normal.z()})
			.WithIsNormalFlipped(IsNormalFlipped)
			.MakeShared();
}
GU_ConstDetailHandle SIM_Hina_Plane::getGeometrySubclass() const
{
	if (my_detail_handle.isNull())
	{
		GU_Detail *gdp = new GU_Detail();
		my_detail_handle.allocateAndSet(gdp);

		UT_Vector3 Center = getCenter();
		UT_Vector3 Normal = getNormal();
		UT_Vector2 Extent = UT_Vector2(5, 5);

		std::array<UT_Vector3, 4> vertices{
				UT_Vector3(Center.x() - 0.5f * Extent.x(), Center.y(), Center.z() - 0.5f * Extent.y()),
				UT_Vector3(Center.x() - 0.5f * Extent.x(), Center.y(), Center.z() + 0.5f * Extent.y()),
				UT_Vector3(Center.x() + 0.5f * Extent.x(), Center.y(), Center.z() + 0.5f * Extent.y()),
				UT_Vector3(Center.x() + 0.5f * Extent.x(), Center.y(), Center.z() - 0.5f * Extent.y()),
		};
		UT_Matrix3 Rotation_Matrix;
		Rotation_Matrix.identity();

		// check the same
		UT_Vector3 InitNormal = {0, 1, 0};
		InitNormal.normalize();
		Normal.normalize();
		if (InitNormal != Normal)
		{
			UT_Vector3 Rotation_Axis = InitNormal;
			Rotation_Axis.cross(Normal);
			Rotation_Axis.normalize();
			fpreal angle = acos(InitNormal.dot(Normal));
			Rotation_Matrix.rotate(Rotation_Axis, angle);
		}

		for (auto &v: vertices)
			v = colVecMult(Rotation_Matrix, v);

		std::array<GA_Offset, 8> ptoff{};
		for (int i = 0; i < 8; i++)
		{
			ptoff[i] = gdp->appendPointOffset();
			gdp->setPos3(ptoff[i], vertices[i]);
		}

		static const int faces[1][4] = {
				{0, 1, 2, 3},
		};

		GEO_PrimPoly *prim = (GEO_PrimPoly *) gdp->appendPrimitive(GA_PRIMPOLY);
		for (int j = 0; j < 4; j++)
			prim->appendVertex(ptoff[faces[0][j]]);
		prim->close();
	}
	return my_detail_handle;
}
