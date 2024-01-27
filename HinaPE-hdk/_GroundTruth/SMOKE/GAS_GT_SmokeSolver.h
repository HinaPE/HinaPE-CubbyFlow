#ifndef HINAPE_GAS_GT_SMOKESOLVER_H
#define HINAPE_GAS_GT_SMOKESOLVER_H

#include "HinaHDKClassGenerator.h"
#include "Core/Solver/Grid/GridSmokeSolver3.hpp"

NEW_HINA_MICRPSOLVER_CLASS(
		GT_SmokeSolver,
		CubbyFlow::GridSmokeSolver3Ptr InnerPtr;
				CubbyFlow::Frame frame;
)

#endif //HINAPE_GAS_GT_SMOKESOLVER_H
