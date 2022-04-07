cmake_minimum_required(VERSION 3.12)
project(gdmap_0_8_1)

find_package(GTK2 REQUIRED gtk)
find_package(LibXml2 REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -DHAVE_CONFIG_H=1 -DGDMAP_LOCALE_DIR=\"\\\"${PROJECT_SOURCE_DIR}/po/\\\"\"") 

set(SOURCE_FILES
    src/about.c
    src/colors.c
    src/gui_main.c
    src/gui_support.c
    src/main.c
    src/preferences.c
    src/tree.c
    src/utils.c)

include_directories(${GTK2_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} . ${PROJECT_SOURCE_DIR})
add_executable(gdmap_0_8_1 ${SOURCE_FILES})
target_link_libraries(gdmap_0_8_1 ${GTK2_LIBRARIES} ${LIBXML2_LIBRARIES} m)
