#include "gtest/gtest.h"

#include <Core/Solver/Grid/GridFractionalBoundaryConditionSolver2.hpp>

using namespace CubbyFlow;

TEST(GridFractionalBoundaryConditionSolver2, ClosedDomain)
{
    GridFractionalBoundaryConditionSolver2 bndSolver;
    Vector2UZ gridSize(10, 10);
    Vector2D gridSpacing(1.0, 1.0);
    Vector2D gridOrigin(-5.0, -5.0);

    bndSolver.UpdateCollider(nullptr, gridSize, gridSpacing, gridOrigin);

    FaceCenteredGrid2 velocity(gridSize, gridSpacing, gridOrigin);
    velocity.Fill(Vector2D(1.0, 1.0));

    bndSolver.ConstrainVelocity(&velocity);

    velocity.ForEachUIndex([&](const Vector2UZ& idx) {
        if (idx.x == 0 || idx.x == gridSize.x)
        {
            EXPECT_DOUBLE_EQ(0.0, velocity.U(idx));
        }
        else
        {
            EXPECT_DOUBLE_EQ(1.0, velocity.U(idx));
        }
    });

    velocity.ForEachVIndex([&](const Vector2UZ& idx) {
        if (idx.y == 0 || idx.y == gridSize.y)
        {
            EXPECT_DOUBLE_EQ(0.0, velocity.V(idx));
        }
        else
        {
            EXPECT_DOUBLE_EQ(1.0, velocity.V(idx));
        }
    });
}

TEST(GridFractionalBoundaryConditionSolver2, OpenDomain)
{
    GridFractionalBoundaryConditionSolver2 bndSolver;
    Vector2UZ gridSize(10, 10);
    Vector2D gridSpacing(1.0, 1.0);
    Vector2D gridOrigin(-5.0, -5.0);

    // Partially open domain
    bndSolver.SetClosedDomainBoundaryFlag(DIRECTION_LEFT | DIRECTION_UP);
    bndSolver.UpdateCollider(nullptr, gridSize, gridSpacing, gridOrigin);

    FaceCenteredGrid2 velocity(gridSize, gridSpacing, gridOrigin);
    velocity.Fill(Vector2D(1.0, 1.0));

    bndSolver.ConstrainVelocity(&velocity);

    velocity.ForEachUIndex([&](const Vector2UZ& idx) {
        if (idx.x == 0)
        {
            EXPECT_DOUBLE_EQ(0.0, velocity.U(idx));
        }
        else
        {
            EXPECT_DOUBLE_EQ(1.0, velocity.U(idx));
        }
    });

    velocity.ForEachVIndex([&](const Vector2UZ& idx) {
        if (idx.y == gridSize.y)
        {
            EXPECT_DOUBLE_EQ(0.0, velocity.V(idx));
        }
        else
        {
            EXPECT_DOUBLE_EQ(1.0, velocity.V(idx));
        }
    });
}