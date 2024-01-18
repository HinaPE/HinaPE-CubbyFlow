#include "gtest/gtest.h"

#include <Core/Geometry/Box.hpp>
#include <Core/Geometry/ColliderSet.hpp>
#include <Core/Geometry/RigidBodyCollider.hpp>

using namespace CubbyFlow;

TEST(ColliderSet2, Constructors)
{
    auto box1 = Box2::GetBuilder()
                    .WithLowerCorner({ 0, 1 })
                    .WithUpperCorner({ 1, 2 })
                    .MakeShared();

    auto box2 = Box2::GetBuilder()
                    .WithLowerCorner({ 2, 3 })
                    .WithUpperCorner({ 3, 4 })
                    .MakeShared();

    auto col1 = RigidBodyCollider2::GetBuilder().WithSurface(box1).MakeShared();

    auto col2 = RigidBodyCollider2::GetBuilder().WithSurface(box2).MakeShared();

    ColliderSet2 colSet1;
    EXPECT_EQ(0u, colSet1.NumberOfColliders());

    ColliderSet2 colSet2(Array1<Collider2Ptr>{ col1, col2 });
    EXPECT_EQ(2u, colSet2.NumberOfColliders());
    EXPECT_EQ(col1, colSet2.GetCollider(0));
    EXPECT_EQ(col2, colSet2.GetCollider(1));
}

TEST(ColliderSet2, Builder)
{
    auto box1 = Box2::GetBuilder()
                    .WithLowerCorner({ 0, 1 })
                    .WithUpperCorner({ 1, 2 })
                    .MakeShared();

    auto box2 = Box2::GetBuilder()
                    .WithLowerCorner({ 2, 3 })
                    .WithUpperCorner({ 3, 4 })
                    .MakeShared();

    auto col1 = RigidBodyCollider2::GetBuilder().WithSurface(box1).MakeShared();

    auto col2 = RigidBodyCollider2::GetBuilder().WithSurface(box2).MakeShared();

    auto colSet1 = ColliderSet2::GetBuilder().MakeShared();
    EXPECT_EQ(0u, colSet1->NumberOfColliders());

    auto colSet2 = ColliderSet2::GetBuilder()
                       .WithColliders(Array1<Collider2Ptr>{ col1, col2 })
                       .MakeShared();
    EXPECT_EQ(2u, colSet2->NumberOfColliders());
    EXPECT_EQ(col1, colSet2->GetCollider(0));
    EXPECT_EQ(col2, colSet2->GetCollider(1));

    auto colSet3 = ColliderSet2::GetBuilder().Build();
    EXPECT_EQ(0u, colSet3.NumberOfColliders());
}