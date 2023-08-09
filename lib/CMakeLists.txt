add_subdirectory(constructor)
add_subdirectory(debugging)

add_library(cdi STATIC ${ALL_OBJECT_FILES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:cdi>
  PARENT_SCOPE
)
