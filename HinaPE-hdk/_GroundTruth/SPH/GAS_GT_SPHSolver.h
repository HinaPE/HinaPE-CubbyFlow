#ifndef HINAPE_CUBBYFLOW_GAS_GT_SPHSOLVER_H
#define HINAPE_CUBBYFLOW_GAS_GT_SPHSOLVER_H

#include "HinaHDKClassGenerator.h"

#include "Core/Solver/Particle/SPH/SPHSolver3.hpp"

NEW_HINA_MICRPSOLVER_CLASS(
		GT_SPHSolver,
		CubbyFlow::SPHSolver3Ptr InnerPtr;
				CubbyFlow::Frame frame;
)

#endif //HINAPE_CUBBYFLOW_GAS_GT_SPHSOLVER_H
