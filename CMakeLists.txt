project (ovpnsup)

cmake_minimum_required (VERSION 2.6)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s -std=gnu99 -pedantic -Wall -Wextra -Wformat=2 -Wformat-nonliteral -Wformat-security -Wshadow -Wpointer-arith -Wmissing-prototypes -Wcast-qual")

include_directories("${PROJECT_SOURCE_DIR}/ncmlib")
add_subdirectory(ncmlib)

file(GLOB OVPNSUP_SRCS "*.c")

add_executable(ovpnsup ${OVPNSUP_SRCS})
target_link_libraries(ovpnsup ncmlib)
