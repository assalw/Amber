cmake_minimum_required(VERSION 2.6)
project(Amber)

set(LibVLC_DIR "${CMAKE_SOURCE_DIR}/cmake") 

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(LibVLC REQUIRED)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(libnanomsg REQUIRED libnanomsg)
pkg_search_module(GLIB REQUIRED glib-2.0)

include_directories(${GTK2_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
include_directories(${GLIB_INCLUDE_DIRS})

link_directories(${GTK2_LIBRARY_DIRS} )

add_executable(amber src/player.c src/network.c src/render.c src/main.c)
add_definitions(${GTK2_CFLAGS_OTHER})
target_link_libraries(amber ${GTK2_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} ${LIBVLC_LIBRARY} ${GLEW_LIBRARY})

install(TARGETS amber RUNTIME DESTINATION bin)
