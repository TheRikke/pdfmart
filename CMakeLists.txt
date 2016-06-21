project(pdfmart)
cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0020 NEW)

if(POLICY CMP0043)
   cmake_policy(SET CMP0043 NEW)
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

find_package(Qt5Widgets REQUIRED)
find_package(Poppler COMPONENTS Qt5)

INCLUDE_DIRECTORIES(${POPPLER_INCLUDE_DIRS} ${Qt5Widgets_INCLUDES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
add_definitions(-Wall -Wextra ${Qt5Widgets_DEFINITIONS})
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
   zoomablepageview.h
   zoomablepageview.cpp
   )

QT5_WRAP_UI(UIS_HDRS dialog.ui )
add_executable(${PROJECT_NAME} ${SRC_LIST} ${UIS_HDRS} )

qt5_use_modules( ${PROJECT_NAME} Widgets )
target_link_libraries(${PROJECT_NAME} ${POPPLER_LIBRARIES} ${Qt5Widgets_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
)
if(WIN32)
   get_target_property(WIDGET_DLL Qt5::Widgets IMPORTED_LOCATION)
   INSTALL(FILES "${WIDGET_DLL}" DESTINATION bin
                   COMPONENT applications)
   INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )
endif()

set(CPACK_PACKAGE_CONTACT "rikky@web.de")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.14), poppler-utils, libpoppler-qt5-1, tesseract-ocr")
INCLUDE(InstallRequiredSystemLibraries)
include(CPack)
