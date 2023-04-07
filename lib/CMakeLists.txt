cmake_minimum_required(VERSION 3.18.4)

set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(CMAKE_BUILD_TYPE Debug)

###########################
# add your own library here
###########################

add_library(
  hostinfo SHARED
  include/hostinfo.h
  src/hostinfo.c
)
target_include_directories(hostinfo PUBLIC "${LIB_INCLUDE_DIR}")

add_library(
  inet-helper SHARED
  include/inet-helper.h
  src/inet-helper.c
)
target_include_directories(inet-helper PUBLIC "${LIB_INCLUDE_DIR}")

add_library(
  sbuf SHARED
  include/sbuf.h
  src/sbuf.c
)
target_include_directories(sbuf PUBLIC "${LIB_INCLUDE_DIR}")

# # add a shared library, foo
# add_library(
#   foo SHARED
#   include/foo.h
#   src/foo.c
# )
# target_include_directories(foo PUBLIC "${LIB_INCLUDE_DIR}")

###########################
# external link_libraries
###########################

# add a shared library as the external library, csapp
add_library(
  csapp SHARED
  include/csapp.h
  src/csapp.c
)
target_include_directories(csapp PUBLIC "${LIB_INCLUDE_DIR}")
target_link_libraries(csapp -lpthread)

# aggregate all auxiliary libraries into libcomm 
# todo: add library to import in libcomm
add_library(
  libexternal INTERFACE
)

target_link_libraries(
  libexternal INTERFACE
  csapp
)
target_include_directories(libexternal INTERFACE "${LIB_INCLUDE_DIR}")
