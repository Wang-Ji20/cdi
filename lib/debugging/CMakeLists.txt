add_library(
  cdi_debugging
  OBJECT
  tracer.cc
  symbolize.cc
)

set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cdi_debugging>
  PARENT_SCOPE
)
