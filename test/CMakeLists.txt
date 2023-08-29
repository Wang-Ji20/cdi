include_directories(${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include)

include(GoogleTest)

add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

file(GLOB_RECURSE CDI_TEST_SOURCES ${TEST_SOURCES_PATTERN})

message(STATUS "Found ${CDI_TEST_SOURCES} test sources: ${PROJECT_SOURCE_DIR}/test/")

foreach (cdi_test_source ${CDI_TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(cdi_test_filename ${cdi_test_source} NAME)
    string(REPLACE ".cc" "" cdi_test_name ${cdi_test_filename})

    # Add the test target separately and as part of "make check-tests".
    add_executable(${cdi_test_name} EXCLUDE_FROM_ALL ${cdi_test_source})
    add_dependencies(build-tests ${cdi_test_name})
    add_dependencies(check-tests ${cdi_test_name})

    gtest_discover_tests(${cdi_test_name}
            EXTRA_ARGS
            --gtest_color=auto
            --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${cdi_test_name}.xml
            --gtest_catch_exceptions=0
            DISCOVERY_TIMEOUT 120
            PROPERTIES
            TIMEOUT 120
            )

    target_link_libraries(${cdi_test_name} cdi gtest gmock_main)

    # Set test target properties and dependencies.
    set_target_properties(${cdi_test_name}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
            COMMAND ${cdi_test_name}
            )
endforeach ()
