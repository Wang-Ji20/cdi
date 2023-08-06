add_library(
  cdi_constructor
  OBJECT
  maybe.cc
)

set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cdi_constructor>
  PARENT_SCOPE
)
