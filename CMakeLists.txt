cmake_minimum_required(VERSION 3.3)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
  cmake_policy(VERSION 3.12)
endif()

project (ovpnsup DESCRIPTION "openvpn supervisor" LANGUAGES C)

set(CMAKE_C_FLAGS "-std=gnu99 -fno-strict-overflow -pedantic -Wall -Wextra -Wimplicit-fallthrough=0 -Wformat=2 -Wformat-nonliteral -Wformat-security -Wshadow -Wpointer-arith -Wmissing-prototypes -Wcast-qual -Wno-unused-parameter")

add_subdirectory(ncmlib)

add_executable(ovpnsup)
target_sources(ovpnsup PRIVATE
  "ovpnsup.c"
)
include_directories(ovpnsup PRIVATE "${PROJECT_SOURCE_DIR}/ncmlib")
target_link_libraries(ovpnsup PUBLIC ncmlib)
