# this CMake module contains functions finding commands like `llvm-cov-${NUMBER < 18}` or `llvm-cov` in the system

message(
  STATUS
  "module findLLVMTools"
)

##################################################
# private
##################################################
function(getClangMajorVersion)

  string(
    REGEX
    MATCH
    "^[0-9]+"
    clangMajorVersion
    "${CMAKE_CXX_COMPILER_VERSION}"
    PARENT_SCOPE
  )
  set_property(GLOBAL PROPERTY PRIVATEVARCLANGMAJORVERSION "${clangMajorVersion}")
  message(
    STATUS
    "Found clang major version: ${clangMajorVersion}"
  )

endfunction(getClangMajorVersion)

##################################################
# private
##################################################
function(findLLVMToolsCanonicalName shortName)

  getClangMajorVersion()
  get_property(ClangMajorVersion GLOBAL PROPERTY PRIVATEVARCLANGMAJORVERSION)
  set(${${shortName}}Canonical "${${shortName}}-${ClangMajorVersion}")
  message(
    STATUS
    "Canonical name for ${${shortName}}: ${${${shortName}}Canonical}"
  )
  set_property(GLOBAL PROPERTY ${shortName}Canonical "${${${shortName}}Canonical}")

endfunction(findLLVMToolsCanonicalName shortName)

##################################################
# public
# return by ${shortname}_BIN
##################################################
function(findLLVMTools shortName)

  findLLVMToolsCanonicalName("${shortName}")
  get_property(${shortName}Canonical GLOBAL PROPERTY ${shortName}Canonical)
  find_program(
    ${shortName}_BIN
    NAMES
    "${${shortName}Canonical}"
    "${shortName}"
  )
  mark_as_advanced(${shortName}_BIN)
  set_property(GLOBAL PROPERTY ${shortName}_BIN "${${shortName}_BIN}")
  message(
    STATUS
    "Found ${shortName} binary: ${${shortName}_BIN}"
  )

endfunction(findLLVMTools shortName)

##################################################
