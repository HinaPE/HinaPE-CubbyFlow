#ifndef HINAPE_SPHERE_H
#define HINAPE_SPHERE_H

#include "HinaHDKClassGenerator.h"
#include "Core/Geometry/Sphere.hpp"

NEW_HINA_GEOMETRY_CLASS(
		Sphere,
		CubbyFlow::Sphere3Ptr,
		NEW_GETSET_PARAMETER(Center, GETSET_DATA_FUNCS_V3)
		NEW_GETSET_PARAMETER(Radius, GETSET_DATA_FUNCS_F)
		NEW_GETSET_PARAMETER(IsNormalFlipped, GETSET_DATA_FUNCS_B)
)

#endif //HINAPE_SPHERE_H
