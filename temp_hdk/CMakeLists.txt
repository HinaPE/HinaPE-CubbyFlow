set(TEMP_TARGET SIM_CubbyFlowTemp)

add_library(${TEMP_TARGET} SHARED
        Entrance.cpp

        FLIP/GAS_CF_FLIPSolver.cpp
        FLIP/GAS_CF_FLIPSolver.h

        FLIP/GAS_CF_ConfigureFLIPData.cpp
        FLIP/GAS_CF_ConfigureFLIPData.h

        FLIP/SIM_CF_FLIPData.cpp
        FLIP/SIM_CF_FLIPData.h

        VDB/GAS_VDB_TEST.cpp
        VDB/GAS_VDB_TEST.h
)

# Link Houdini Toolkit
target_link_libraries(
        ${TEMP_TARGET}
        PUBLIC
        Houdini
        CubbyFlow
        ${DEFAULT_LINKER_OPTIONS}
        ${DEFAULT_LIBRARIES}
)
target_link_directories(
        ${TEMP_TARGET}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/install/lib
)
target_include_directories(
        ${TEMP_TARGET}
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/install/include
        FLIP
        VDB
)
target_compile_options(
        ${TEMP_TARGET}
        PRIVATE
        -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION
)
houdini_configure_target(${TEMP_TARGET})
