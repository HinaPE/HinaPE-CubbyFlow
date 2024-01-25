#include "SIM_Hina_Box.h"

NEW_HINA_GEOMETRY_IMPLEMENT(
		Box,
		CubbyFlow::Box3Ptr,
		false,
		NEW_FLOAT_VECTOR_PARAMETER(Center, 3, 0, 0, 0) \
        NEW_FLOAT_VECTOR_PARAMETER(Extent, 3, .5, .5, .5) \
        NEW_BOOL_PARAMETER(IsNormalFlipped, false) \
)

void SIM_Hina_Box::_init() {}
void SIM_Hina_Box::_makeEqual(const SIM_Hina_Box *src) {}
CubbyFlow::Box3Ptr SIM_Hina_Box::RuntimeConstruct() const
{
	UT_Vector3 Center = getCenter();
	UT_Vector3 Extent = getExtent();
	bool IsNormalFlipped = getIsNormalFlipped();

	UT_Vector3 LowerCorner = Center - Extent / 2.;
	UT_Vector3 UpperCorner = Center + Extent / 2.;

	return CubbyFlow::Box3::GetBuilder()
			.WithLowerCorner({LowerCorner.x(), LowerCorner.y(), LowerCorner.z()})
			.WithUpperCorner({UpperCorner.x(), UpperCorner.y(), UpperCorner.z()})
			.WithIsNormalFlipped(IsNormalFlipped)
			.MakeShared();
}
GU_ConstDetailHandle SIM_Hina_Box::getGeometrySubclass() const
{
	if (my_detail_handle.isNull())
	{
		GU_Detail *gdp = new GU_Detail();
		my_detail_handle.allocateAndSet(gdp);

		UT_Vector3 Center = getCenter();
		UT_Vector3 Extent = getExtent();

		std::array<UT_Vector3, 8> vertices{};
		for (int i = 0; i < 8; i++)
		{
			vertices[i] = UT_Vector3(
					Center.x() + Extent.x() * ((i & 1) ? 0.5 : -0.5),
					Center.y() + Extent.y() * ((i & 2) ? 0.5 : -0.5),
					Center.z() + Extent.z() * ((i & 4) ? 0.5 : -0.5)
			);
		}

		std::array<GA_Offset, 8> ptoff{};
		for (int i = 0; i < 8; i++)
		{
			ptoff[i] = gdp->appendPointOffset();
			gdp->setPos3(ptoff[i], vertices[i]);
		}

		static const int faces[6][4] = {
				{0, 4, 6, 2}, // Front
				{1, 3, 7, 5}, // Back
				{0, 1, 5, 4}, // Bottom
				{2, 6, 7, 3}, // Top
				{0, 2, 3, 1}, // Left
				{4, 5, 7, 6}  // Right
		};

		for (int i = 0; i < 6; i++)
		{
			GEO_PrimPoly *prim = (GEO_PrimPoly *) gdp->appendPrimitive(GA_PRIMPOLY);
			for (int j = 0; j < 4; j++)
				prim->appendVertex(ptoff[faces[i][j]]);
			prim->close();
		}
	}

	return my_detail_handle;
}
