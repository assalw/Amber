cmake_minimum_required(VERSION 2.6)
project(Amber)

find_package (PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(libvlc REQUIRED sdl2)

pkg_check_modules (GTK2 REQUIRED gtk+-2.0)

include_directories (${GTK2_INCLUDE_DIRS})
include_directories (${OPENGL_INCLUDE_DIRS})
include_directories (${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

link_directories (${GTK2_LIBRARY_DIRS})

add_executable(amber src/network.c src/render.c src/main.c)
add_definitions (${GTK2_CFLAGS_OTHER})
target_link_libraries (amber ${GTK2_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

install(TARGETS amber RUNTIME DESTINATION bin)
