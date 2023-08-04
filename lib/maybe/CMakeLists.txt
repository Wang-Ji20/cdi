add_library(
  cdi_maybe
  OBJECT
  maybe.cc
)

set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cdi_maybe>
  PARENT_SCOPE
)
