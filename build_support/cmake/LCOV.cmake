# this module generates LCOV

include(findLLVMTools)

# compiler generate coverage information.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
################################################################################

# find neccesary tools
set(LLVM_COV "llvm-cov")
findLLVMTools(LLVM_COV)
get_property(LLVM_COV_BIN GLOBAL PROPERTY LLVM_COV_BIN)

set(LLVM_PROFDATA "llvm-profdata")
findLLVMTools(LLVM_PROFDATA)
get_property(LLVM_PROFDATA_BIN GLOBAL PROPERTY LLVM_PROFDATA_BIN)


################################################################################

# find all test cases
file(GLOB_RECURSE MY_TEST_CASES_SOURCE ${TEST_SOURCES_PATTERN})
message(STATUS "MY_TEST_CASES_SOURCE: ${MY_TEST_CASES_SOURCE}")

set(COVERAGE_COMMANDLINE_ARGUMENT "")
set(MY_TEST_CASES "")

foreach (test-case ${MY_TEST_CASES_SOURCE})
  get_filename_component(test-case ${test-case} NAME)
  string(REPLACE ".cc" "" test-case ${test-case})
  list(APPEND COVERAGE_COMMANDLINE_ARGUMENT -object "${CMAKE_BINARY_DIR}/test/${test-case}")
  list(APPEND MY_TEST_CASES ${test-case})
endforeach(test-case ${MY_TEST_CASES})

add_custom_target(coverage-report COMMAND ${LLVM_COV_BIN} show ${COVERAGE_COMMANDLINE_ARGUMENT} -format=html -instr-profile default.profdata > report.html)
add_dependencies(coverage-report build-tests)

add_custom_target(delete-testxmls COMMAND rm -rf ${CMAKE_BINARY_DIR}/test/*.xml)
add_dependencies(delete-testxmls build-tests)


add_custom_target(merge-profdata COMMAND ${LLVM_PROFDATA_BIN} merge -sparse -o default.profdata *.profraw)
add_dependencies(merge-profdata check-tests)
add_dependencies(coverage-report merge-profdata)

foreach(test-case ${MY_TEST_CASES})
  get_filename_component(test-case ${test-case} NAME)
  add_custom_target(
    coverage-${test-case}
    COMMAND
    LLVM_PROFILE_FILE=${test-case}.profraw ${CMAKE_BINARY_DIR}/test/${test-case}
    DEPENDS
    build-tests
  )
  add_dependencies(merge-profdata coverage-${test-case})
  message(STATUS "Adding coverage-${test-case}")
endforeach(test-case ${MY_TEST_CASES})

