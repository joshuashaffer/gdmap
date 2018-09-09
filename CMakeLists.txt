cmake_minimum_required(VERSION 3.3)
project(gdmap_0_8_1)


# do pkg-config 
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED cairo pango glib-2.0 gtk+-2.0 libxml-2.0)


# Probably a more cmake-onic way of doing this ...
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -I. -I.. -O3 -march=native ")

set(SOURCE_FILES
    src/about.c
    src/about.h
    src/colors.c
    src/colors.h
    src/gui_main.c
    src/gui_main.h
    src/gui_support.c
    src/gui_support.h
    src/l_i18n.h
    src/main.c
    src/preferences.c
    src/preferences.h
    src/tree.c
    src/tree.h
    src/utils.c
    src/utils.h
    config.h)

# Hack to use old autoconf crap.
add_definitions(-DHAVE_CONFIG_H=1 -DGDMAP_LOCALE_DIR="./po/")

add_executable(gdmap_0_8_1 ${SOURCE_FILES})
target_link_libraries(gdmap_0_8_1 ${GTK_LIBRARIES} m)
target_include_directories(gdmap_0_8_1 PUBLIC ${GTK_INCLUDE_DIRS})
target_compile_options(gdmap_0_8_1 PUBLIC ${GTK_CFLAGS} )
