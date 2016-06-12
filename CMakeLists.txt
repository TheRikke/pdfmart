project(pdfmart)
cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0020 NEW)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

find_package(Qt5Widgets REQUIRED)
find_package(Poppler COMPONENTS Qt5)

INCLUDE_DIRECTORIES(${POPPLER_INCLUDE_DIRS})

QT5_WRAP_UI(UIS_HDRS dialog.ui )
add_definitions(-Wall -Wextra)
set(SRC_LIST
   IMerge.h
   main.cpp
   MergePDF.cpp
   MergePDF.h
   OptionDialog.h
   OptionDialog.cpp
   PageList.h
   PopplerTools.cpp
   PopplerTools.h
   PDFMergeModel.h
   PDFMergeModel.cpp
   PDFPageItemDelegate.cpp
   PDFPageItemDelegate.h
   PDFPagesModel.h
   PDFPagesModel.cpp
   PMSettings.h
   PMSettings.cpp
   OcrHandler.h
   OcrHandler.cpp
   ToolsTest.h
   ToolsTest.cpp
   testfiles.qrc
   Logger.cpp
   Logger.h
   )
add_executable(${PROJECT_NAME} WIN32 ${SRC_LIST} ${UIS_HDRS} )

qt5_use_modules( ${PROJECT_NAME} Widgets )
target_link_libraries(${PROJECT_NAME} ${POPPLER_LIBRARIES} Qt5::Widgets)

get_target_property(WIDGET_DLL Qt5::Widgets IMPORTED_LOCATION)
INSTALL(FILES "${WIDGET_DLL}" DESTINATION bin
                COMPONENT applications)
INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )

INCLUDE(InstallRequiredSystemLibraries)
include(CPack)
