// This code is based on Jet framework.
// Copyright (c) 2018 Doyub Kim
// CubbyFlow is voxel-based fluid simulation engine for computer games.
// Copyright (c) 2020 CubbyFlow Team
// Core Part: Chris Ohk, Junwoo Hwang, Jihong Sin, Seungwoo Yoo
// AI Part: Dongheon Cho, Minseo Kim
// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Core/CUDA/CUDAParticleSystemSolver3.hpp>
#include <Core/CUDA/CUDAUtils.hpp>

using namespace CubbyFlow;

namespace
{
__global__ void AdvanceTimeStepKernel(float dt, float4 gravity,
                                      const float4* positions,
                                      const float4* velocities, size_t n,
                                      float4* newPositions,
                                      float4* newVelocities)
{
    size_t i = blockIdx.x * blockDim.x + threadIdx.x;

    if (i < n)
    {
        // Time integration
        float4 v1 = velocities[i] + dt * gravity;
        float4 p1 = positions[i] + dt * v1;

        // Collision handling
        if (p1.y < 0.0f)
        {
            p1.y = 0.0f;

            if (v1.y < 0.0f)
            {
                v1.y *= -1.0f;
            }
        }

        newPositions[i] = p1;
        newVelocities[i] = v1;
    }
}

}  // namespace

void CUDAParticleSystemSolver3::OnAdvanceTimeStep(double timeStepInSeconds)
{
    UpdateCollider(timeStepInSeconds);
    UpdateEmitter(timeStepInSeconds);

    auto particles = ParticleSystemData();
    const size_t n = particles->NumberOfParticles();
    auto curPos = particles->Positions();
    auto curVel = particles->Velocities();
    const auto dt = static_cast<float>(timeStepInSeconds);
    const auto g = ToFloat4(Gravity(), 0.0f);

    unsigned int numBlocks, numThreads;
    CUDAComputeGridSize(static_cast<unsigned>(n), 256, numBlocks, numThreads);
    AdvanceTimeStepKernel<<<numBlocks, numThreads>>>(
        dt, g, curPos.data(), curVel.data(), n, curPos.data(), curVel.data());
}