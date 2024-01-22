set(TEMP_TARGET SIM_CubbyFlowTemp)

add_library(${TEMP_TARGET} SHARED
        Entrance.cpp

        GAS_CF_FLIPSolver.cpp
        GAS_CF_FLIPSolver.h

        GAS_CF_ConfigureFLIPData.cpp
        GAS_CF_ConfigureFLIPData.h

        SIM_CF_FLIPData.cpp
        SIM_CF_FLIPData.h
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
        ../hdk
)
target_compile_options(
        ${TEMP_TARGET}
        PRIVATE
        -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION
)
houdini_configure_target(${TEMP_TARGET})
