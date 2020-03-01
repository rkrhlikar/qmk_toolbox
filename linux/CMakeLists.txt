cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

project(qmk_toolbox)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})
pkg_check_modules(XMLPP libxml++-2.6)
link_directories(${XMLPP_LIBRARY_DIRS})
include_directories(${XMLPP_INCLUDE_DIRS})

include_directories(include)

add_executable(qmk_toolbox src/main.cpp
                           src/main_window.cpp
                           src/local_file_box.cpp
                           src/remote_file_grid.cpp
                           src/console_text_view.cpp
                           src/user_configuration.cpp)

target_link_libraries(qmk_toolbox ${GTKMM_LIBRARIES} ${XMLPP_LIBRARIES} curl curlpp stdc++fs)