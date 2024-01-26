#include "SIM_CF_Plane.h"

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

GU_ConstDetailHandle SIM_CF_Plane::getGeometrySubclass() const
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

void SIM_CF_Plane::initializeSubclass()
{
	SIM_Geometry::initializeSubclass();

	// IMPORTANT!!!
	// NEVER CALL GET_SET FUNCTION HERE!!!

	/// Implement Initializations of Your Custom Fields
	this->my_detail_handle.clear();
}

void SIM_CF_Plane::makeEqualSubclass(const SIM_Data *source)
{
	SIM_Geometry::makeEqualSubclass(source);
	const SIM_CF_Plane *src = SIM_DATA_CASTCONST(source, SIM_CF_Plane);

	/// Implement Equal Operator of Your Custom Fields
	this->my_detail_handle = src->my_detail_handle;
}

const char *SIM_CF_Plane::DATANAME = "CF_Plane";
const SIM_DopDescription *SIM_CF_Plane::getDopDescription()
{
	static PRM_Name Center("Center", "Center");
	static std::array<PRM_Default, 3> CenterDefault{0, 0, 0};

	static PRM_Name Normal("Normal", "Normal");
	static std::array<PRM_Default, 3> NormalDefault{0, 1, 0};

	static PRM_Name IsNormalFlipped("IsNormalFlipped", "IsNormalFlipped");

	static std::array<PRM_Template, 5> PRMS{
			PRM_Template(PRM_FLT_J, 3, &Center, CenterDefault.data()),
			PRM_Template(PRM_FLT_J, 3, &Normal, NormalDefault.data()),
			PRM_Template(PRM_TOGGLE, 1, &IsNormalFlipped, PRMzeroDefaults),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "cf_plane",
								   "CF Plane",
								   DATANAME,
								   classname(),
								   PRMS.data());
	DESC.setDefaultUniqueDataName(true);
	return &DESC;
}

CubbyFlow::Plane3Ptr SIM_CF_Plane::RuntimeConstructCFPlane() const
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
