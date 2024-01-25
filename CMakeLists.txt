cmake_minimum_required(VERSION 3.26)

project(HinaPE-CubbyFlow)

set(CMAKE_CXX_STANDARD 17)
set(TARGET SIM_CubbyFlow)

# Find Houdini
if (MSVC)
    set(Houdini_PATH "C:/Program Files/Side Effects Software/Houdini 20.0.590")
elseif (APPLE)
    set(Houdini_PATH "/Applications/Houdini/Houdini20.0.590/Frameworks/Houdini.framework/Versions/20.0/Resources")
endif ()
set(Houdini_DIR ${Houdini_PATH}/toolkit/cmake)
find_package(Houdini REQUIRED)

# Load Source
add_library(
        ${TARGET}
        SHARED
        hdk/Entrance.cpp
        hdk/CubbyHDKClassGenerator.h
        hdk/HOW_TO_USE_GENERATOR.cpp
        hdk/HOW_TO_USE_GENERATOR.h

        hdk/Collider/GAS_CF_ConfigureRigidBodyCollider.cpp
        hdk/Collider/GAS_CF_ConfigureRigidBodyCollider.h
        hdk/Collider/GAS_CF_UpdateCollider.cpp
        hdk/Collider/GAS_CF_UpdateCollider.h
        hdk/Collider/SIM_CF_RigidBodyCollider.cpp
        hdk/Collider/SIM_CF_RigidBodyCollider.h

        hdk/Emitter/GAS_CF_PointEmitter.cpp
        hdk/Emitter/GAS_CF_PointEmitter.h
        hdk/Emitter/GAS_CF_VolumeParticleEmitter.cpp
        hdk/Emitter/GAS_CF_VolumeParticleEmitter.h

        hdk/Geometry/SIM_CF_Box.cpp
        hdk/Geometry/SIM_CF_Box.h
        hdk/Geometry/SIM_CF_Plane.cpp
        hdk/Geometry/SIM_CF_Plane.h
        hdk/Geometry/SIM_CF_Sphere.cpp
        hdk/Geometry/SIM_CF_Sphere.h

        hdk/Particle/GAS_CF_CommitCache.cpp
        hdk/Particle/GAS_CF_CommitCache.h
        hdk/Particle/GAS_CF_UpdateToGeometrySheet.cpp
        hdk/Particle/GAS_CF_UpdateToGeometrySheet.h
        hdk/Particle/ParticleSystemData/GAS_CF_ConfigureParticleSystemData.cpp
        hdk/Particle/ParticleSystemData/GAS_CF_ConfigureParticleSystemData.h
        hdk/Particle/ParticleSystemData/SIM_CF_ParticleSystemData.cpp
        hdk/Particle/ParticleSystemData/SIM_CF_ParticleSystemData.h
        hdk/Particle/SPHSystemData/GAS_CF_ConfigureSPHSystemData.cpp
        hdk/Particle/SPHSystemData/GAS_CF_ConfigureSPHSystemData.h
        hdk/Particle/SPHSystemData/SIM_CF_SPHSystemData.cpp
        hdk/Particle/SPHSystemData/SIM_CF_SPHSystemData.h
        hdk/Particle/PCISPHSystemData/GAS_CF_ConfigurePCISPHSystemData.cpp
        hdk/Particle/PCISPHSystemData/GAS_CF_ConfigurePCISPHSystemData.h
        hdk/Particle/PCISPHSystemData/SIM_CF_PCISPHSystemData.cpp
        hdk/Particle/PCISPHSystemData/SIM_CF_PCISPHSystemData.h

        hdk/Solver/Advection/GAS_CF_Semi_Implicit_Euler.cpp
        hdk/Solver/Advection/GAS_CF_Semi_Implicit_Euler.h
        hdk/Solver/Advection/GAS_CF_SemiLagrangianAdvector.cpp
        hdk/Solver/Advection/GAS_CF_SemiLagrangianAdvector.h
        hdk/Solver/Collision/GAS_CF_CollisionSolver.cpp
        hdk/Solver/Collision/GAS_CF_CollisionSolver.h
        hdk/Solver/Density/GAS_CF_UpdateDensitySolver.cpp
        hdk/Solver/Density/GAS_CF_UpdateDensitySolver.h
        hdk/Solver/Force/GAS_CF_ActivateGravityForce.cpp
        hdk/Solver/Force/GAS_CF_ActivateGravityForce.h
        hdk/Solver/Force/GAS_CF_ClearForce.cpp
        hdk/Solver/Force/GAS_CF_ClearForce.h
        hdk/Solver/Neighbor/GAS_CF_BuildNeighborLists.cpp
        hdk/Solver/Neighbor/GAS_CF_BuildNeighborLists.h
        hdk/Solver/Neighbor/GAS_CF_ReadNeighborLists.cpp
        hdk/Solver/Neighbor/GAS_CF_ReadNeighborLists.h
        hdk/Solver/Pressure/GAS_CF_PCISPHPressureForceSolver.cpp
        hdk/Solver/Pressure/GAS_CF_PCISPHPressureForceSolver.h
        hdk/Solver/Pressure/GAS_CF_PressureForceSolver.cpp
        hdk/Solver/Pressure/GAS_CF_PressureForceSolver.h
        hdk/Solver/Viscosity/GAS_CF_PseudoViscosityForceSolver.cpp
        hdk/Solver/Viscosity/GAS_CF_PseudoViscosityForceSolver.h
        hdk/Solver/Viscosity/GAS_CF_ViscosityForceSolver.cpp
        hdk/Solver/Viscosity/GAS_CF_ViscosityForceSolver.h

        hdk/Utils/SIM_CL_NoLog.cpp
        hdk/Utils/SIM_CL_NoLog.h
)


# Config TBB only for Windows
if (MSVC)
    include(install/cmake/FindTBB.cmake)
    include(install/cmake/TaskingSystemOptions.cmake)
    include(install/cmake/CompileOptions.cmake)
endif ()

# Link Houdini Toolkit
target_link_libraries(
        ${TARGET}
        PUBLIC
        Houdini
        CubbyFlow
        ${DEFAULT_LINKER_OPTIONS}
        ${DEFAULT_LIBRARIES}
)
target_link_directories(
        ${TARGET}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/install/lib
)
target_include_directories(
        ${TARGET}
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/install/include
        hdk
)
target_compile_options(
        ${TARGET}
        PRIVATE
        -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION
)
houdini_configure_target(${TARGET})

# Developing Node
add_subdirectory(temp_hdk)

# Final Node
add_subdirectory(HinaPE-hdk)
