project(pdfmart)
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0020 NEW)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

find_package(Qt5Widgets REQUIRED)
find_package(Poppler COMPONENTS Qt5)

INCLUDE_DIRECTORIES(${POPPLER_INCLUDE_DIRS})

QT5_WRAP_UI(UIS_HDRS
    dialog.ui )
add_definitions(-Wall -Wextra)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} WIN32 ${SRC_LIST} ${UIS_HDRS} )

target_link_libraries(${PROJECT_NAME} ${POPPLER_LIBRARIES})

qt5_use_modules( ${PROJECT_NAME} Widgets )
