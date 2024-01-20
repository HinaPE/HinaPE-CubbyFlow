cmake_minimum_required(VERSION 3.26)

project(HinaPE-CubbyFlow)

set(CMAKE_CXX_STANDARD 17)
set(TARGET SIM_CubbyFlow)

# Find Houdini
if (MSVC)
    set(Houdini_PATH "C:/Program Files/Side Effects Software/Houdini 20.0.547")
elseif (APPLE)
    set(Houdini_PATH "/Applications/Houdini/Houdini20.0.547/Frameworks/Houdini.framework/Versions/20.0/Resources")
endif ()
set(Houdini_DIR ${Houdini_PATH}/toolkit/cmake)
find_package(Houdini REQUIRED)

# Load Source
add_library(
        ${TARGET}
        SHARED
        hdk/Entrance.cpp

        hdk/Particle/SIM_CF_ParticleSystemData.cpp
        hdk/Particle/SIM_CF_ParticleSystemData.h
        hdk/Particle/SIM_CF_SPHSystemData.cpp
        hdk/Particle/SIM_CF_SPHSystemData.h
        hdk/Particle/GAS_CF_ConfigureParticleSystemData.cpp
        hdk/Particle/GAS_CF_ConfigureParticleSystemData.h
        hdk/Particle/GAS_CF_ConfigureSPHSystemData.cpp
        hdk/Particle/GAS_CF_ConfigureSPHSystemData.h
        hdk/Particle/GAS_CF_UpdateToGeometrySheet.cpp
        hdk/Particle/GAS_CF_UpdateToGeometrySheet.h

        hdk/Collider/SIM_CF_RigidBodyCollider.cpp
        hdk/Collider/SIM_CF_RigidBodyCollider.h

        hdk/Emitter/GAS_CF_PointEmitter.cpp
        hdk/Emitter/GAS_CF_PointEmitter.h
        hdk/Emitter/GAS_CF_VolumeParticleEmitter.cpp
        hdk/Emitter/GAS_CF_VolumeParticleEmitter.h

        hdk/Solver/Advection/GAS_CF_SemiLagrangianAdvector.cpp
        hdk/Solver/Advection/GAS_CF_SemiLagrangianAdvector.h
        hdk/Solver/Neighbor/GAS_CF_BuildNeighborLists.cpp
        hdk/Solver/Neighbor/GAS_CF_BuildNeighborLists.h
        hdk/Solver/Density/GAS_CF_UpdateDensitySolver.cpp
        hdk/Solver/Density/GAS_CF_UpdateDensitySolver.h
        hdk/Solver/Force/GAS_CF_ClearForce.cpp
        hdk/Solver/Force/GAS_CF_ClearForce.h

        hdk/Geometry/SIM_CF_Sphere.cpp
        hdk/Geometry/SIM_CF_Sphere.h
        hdk/Geometry/SIM_CF_Box.cpp
        hdk/Geometry/SIM_CF_Box.h
        hdk/Geometry/SIM_CF_Plane.cpp
        hdk/Geometry/SIM_CF_Plane.h
)


# Config CubbyFlow
include(install/cmake/FindTBB.cmake)
include(install/cmake/TaskingSystemOptions.cmake)
include(install/cmake/CompileOptions.cmake)

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

# Test
add_subdirectory(test)