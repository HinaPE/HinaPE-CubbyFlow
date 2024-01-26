#ifndef HINAPE_CUBBYFLOW_GAS_GT_PCISPHSOLVER_H
#define HINAPE_CUBBYFLOW_GAS_GT_PCISPHSOLVER_H

#include "HinaHDKClassGenerator.h"

#include "Core/Solver/Particle/PCISPH/PCISPHSolver3.hpp"

NEW_HINA_MICRPSOLVER_CLASS(
		GT_PCISPHSolver,
		CubbyFlow::PCISPHSolver3Ptr InnerPtr;
				CubbyFlow::Frame frame;
)

#endif //HINAPE_CUBBYFLOW_GAS_GT_PCISPHSOLVER_H
