add_subdirectory(maybe)

add_library(cdi STATIC ${ALL_OBJECT_FILES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
