#ifndef HINAPE_GAS_GT_PICSOLVER_H
#define HINAPE_GAS_GT_PICSOLVER_H

#include "HinaHDKClassGenerator.h"
#include "Core/Solver/Hybrid/PIC/PICSolver3.hpp"

NEW_HINA_MICRPSOLVER_CLASS(
		GT_PICSolver,
		CubbyFlow::PICSolver3Ptr InnerPtr;
		CubbyFlow::Frame frame;
)

#endif //HINAPE_GAS_GT_PICSOLVER_H
