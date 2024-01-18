set(TEST_TARGET TEST_CUBBYFLOW)

add_executable(${TEST_TARGET} test_cubbyflow.cpp)

# Link Houdini Toolkit
target_link_libraries(
        ${TEST_TARGET}
        PUBLIC
        Houdini
        CubbyFlow
        ${DEFAULT_LINKER_OPTIONS}
        ${DEFAULT_LIBRARIES}
)
target_link_directories(
        ${TEST_TARGET}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/install/lib
)
target_include_directories(
        ${TEST_TARGET}
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/install/include
)
target_compile_options(
        ${TEST_TARGET}
        PRIVATE
        -D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION
)
houdini_configure_target(${TEST_TARGET})
