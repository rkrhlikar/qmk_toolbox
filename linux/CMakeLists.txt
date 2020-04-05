cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

project(qmk_toolbox)

# Copy glade xml file and icon to binary directory
configure_file(assets/main.glade assets/main.glade COPYONLY)
configure_file(assets/icon.svg assets/icon.svg COPYONLY)

# Copy microcontrollers list to binary directory
configure_file(../common/mcu-list.txt config/mcu-list.txt COPYONLY)

# Copy avrdude and configuration to binary directory
configure_file(binaries/avrdude bin/avrdude COPYONLY)
configure_file(../common/avrdude.conf config/avrdude.conf COPYONLY)

# Copy dfu-programmer to binary directory
configure_file(binaries/dfu-programmer bin/dfu-programmer COPYONLY)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

include_directories(include)

add_executable(qmk_toolbox src/main.cpp
                           src/main_window.cpp
                           src/local_file_box.cpp
                           src/remote_file_grid.cpp
                           src/console_text_view.cpp
                           src/mcu_list_combo_box.cpp
                           src/user_configuration.cpp
                           src/device_handler.cpp
                           src/flashing_box.cpp
                           src/terminal_console.cpp)

target_link_libraries(qmk_toolbox ${GTKMM_LIBRARIES}
                                  curl
                                  curlpp
                                  stdc++fs
                                  udev
                                  pthread)

add_custom_target(deploy COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/deploy.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
                         DEPENDS qmk_toolbox)