# CMake minimum version
cmake_minimum_required(VERSION 3.5)

# OPTIONAL: Set the temperature scale (C | F | K)
set(SENSORS_SCALE "C")

# OPTIONAL: Enable Over The Air (OTA) updates (ON | OFF)
set(OTA_UPDATES ON)

# OPTIONAL: Set the project version
set(PROJECT_VER "1.0.0.0")

# Include sensor lib
include_directories(ds18b20)
set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/lib/ds18b20/)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(Lakeview)
