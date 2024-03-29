#include "benchmark/benchmark.h"

#include <Core/Geometry/TriangleMeshToSDF.hpp>
#include <Core/Grid/VertexCenteredScalarGrid.hpp>

#include <fstream>
#include <random>

using CubbyFlow::Vector3D;

class TriangleMeshToSDF : public ::benchmark::Fixture
{
 protected:
    std::mt19937 rng{ 0 };
    std::uniform_real_distribution<> dist{ 0.0, 1.0 };
    CubbyFlow::TriangleMesh3 triMesh;
    CubbyFlow::VertexCenteredScalarGrid3 grid;

    void SetUp(const ::benchmark::State&)
    {
        std::ifstream file(RESOURCES_DIR "/bunny.obj");

        if (file)
        {
            [[maybe_unused]] bool isLoaded = triMesh.ReadObj(&file);
            file.close();
        }

        CubbyFlow::BoundingBox3D box = triMesh.GetBoundingBox();
        const Vector3D scale{ box.Width(), box.Height(), box.Depth() };
        box.lowerCorner -= 0.2 * scale;
        box.upperCorner += 0.2 * scale;

        grid.Resize(
            { 100, 100, 100 },
            { box.Width() / 100, box.Height() / 100, box.Depth() / 100 },
            { box.lowerCorner.x, box.lowerCorner.y, box.lowerCorner.z });
    }
};

BENCHMARK_DEFINE_F(TriangleMeshToSDF, Call)(benchmark::State& state)
{
    while (state.KeepRunning())
    {
        CubbyFlow::TriangleMeshToSDF(triMesh, &grid);
    }
}

BENCHMARK_REGISTER_F(TriangleMeshToSDF, Call)->Unit(benchmark::kMillisecond);