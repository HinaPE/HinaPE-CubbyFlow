#include "SIM_CF_Box.h"

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

#include <GEO/GEO_PrimPoly.h>

GU_ConstDetailHandle SIM_CF_Box::getGeometrySubclass() const
{
	if (my_detail_handle.isNull())
	{
		UT_Vector3 Center = getCenter();
		UT_Vector3 Extent = getExtent();
		UT_Vector3 Rotation = getRotation();

		GU_Detail *gdp = new GU_Detail();
		my_detail_handle.allocateAndSet(gdp);
		GA_Offset center_point_offset = gdp->appendPoint();
		gdp->setPos3(center_point_offset, Center);

		UT_Vector3 vertices[8];
		for (int i = 0; i < 8; i++)
		{
			vertices[i] = UT_Vector3(
					Center.x() + Extent.x() * ((i & 1) ? 0.5 : -0.5),
					Center.y() + Extent.y() * ((i & 2) ? 0.5 : -0.5),
					Center.z() + Extent.z() * ((i & 4) ? 0.5 : -0.5)
			);
		}

//		UT_Matrix4 mat;
//		mat.identity();
//		mat.rotate(Rotation);
//		for (auto &vertex: vertices)
//		{
//			vertex = mat * vertex;
//		}

		GA_Offset ptoff[8];
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

void SIM_CF_Box::initializeSubclass()
{
	SIM_Geometry::initializeSubclass();

	/// Implement Initializations of Your Custom Fields
	this->my_detail_handle.clear();
}

void SIM_CF_Box::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Geometry::makeEqualSubclass(source);
	const SIM_CF_Box *src = SIM_DATA_CASTCONST(source, SIM_CF_Box);

	/// Implement Equal Operator of Your Custom Fields
	this->my_detail_handle = src->my_detail_handle;
}

const char *SIM_CF_Box::DATANAME = "CF_Box";
const SIM_DopDescription *SIM_CF_Box::getDopDescription()
{
	static PRM_Name Center("Center", "Center");
	static std::array<PRM_Default, 3> CenterDefault{0, 0, 0};

	static PRM_Name Extent("Extent", "Extent");
	static std::array<PRM_Default, 3> ExtentDefault{0.5, 0.5, 0.5};

	static PRM_Name Rotation("Rotation", "Rotation");
	static std::array<PRM_Default, 3> RotationDefault{0, 0, 0};

	static PRM_Name IsNormalFlipped("IsNormalFlipped", "IsNormalFlipped");

	static std::array<PRM_Template, 5> PRMS{
			PRM_Template(PRM_FLT_J, 3, &Center, CenterDefault.data()),
			PRM_Template(PRM_FLT_J, 3, &Extent, ExtentDefault.data()),
			PRM_Template(PRM_FLT_J, 3, &Rotation, RotationDefault.data()),
			PRM_Template(PRM_TOGGLE, 1, &IsNormalFlipped, PRMzeroDefaults),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_box",
								   "CF Box",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	return &DESC;
}

CubbyFlow::Box3Ptr SIM_CF_Box::RuntimeConstructCFBox() const
{
	UT_Vector3 Center = getCenter();
	UT_Vector3 Extent = getExtent();
	UT_Vector3 Rotation = getRotation();
	bool IsNormalFlipped = getIsNormalFlipped();

	UT_Vector3 LowerCorner = Center - Extent / 2.;
	UT_Vector3 UpperCorner = Center + Extent / 2.;

	return CubbyFlow::Box3::GetBuilder()
			.WithLowerCorner({LowerCorner.x(), LowerCorner.y(), LowerCorner.z()})
			.WithUpperCorner({UpperCorner.x(), UpperCorner.y(), UpperCorner.z()})
			.WithIsNormalFlipped(IsNormalFlipped)
			.MakeShared();
}
