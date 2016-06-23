cmake_minimum_required(VERSION 3.3)
project(gdmap_0_8_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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

include_directories(/usr/include/gtk-2.0/ /usr/include/glib-2.0)

add_executable(gdmap_0_8_1 ${SOURCE_FILES})