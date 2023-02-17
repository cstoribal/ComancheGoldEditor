cmake_minimum_required(VERSION 3.10)

#include(FetchContent)
#FetchContent_Declare(
#        conan_deps
#        GIT_REPOSITORY https://github.com/conan-io/cmake-conan.git
#        GIT_TAG        v0.15
#)
#FetchContent_MakeAvailable(conan_deps)
#conan_cmake_run(
#        REQUIRES qt/6.0.0@qt/stable
#        opencv/4.5.0@conan/stable
#        BASIC_SETUP CMAKE_TARGETS
#        BUILD missing
#)

# Create your project
project(MyProject)

# Use Qt6 and OpenCV as dependencies of your project
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

set(OpenCV_DIR "C:/Dev/opencv/mingw-build")
#set(OpenCV_DIR "C:/Dev/cygwin64/usr/x86_64-w64-mingw32/sys-root/mingw/lib/cmake/OpenCV")
find_package(OpenCV REQUIRED)
  #set(OpenCV_LIB_DIR /data/user/Programs/opencv/opencv-2.4.13.6/build)
message(" -- OPENCV include: " ${OpenCV_INCLUDE_DIRS})
message(" -- OPENCV libs dir: " ${OpenCV_LIB_DIR})
message(" -- OPENCV libs: " ${OpenCV_LIBS})
# semble inclure toute la librairie opencv
include_directories (${OPENCV_INCLUDE_DIRS})
message("    OpenCV version " ${OpenCV_VERSION})

#########################################################################



include_directories(planExp/)
include_directories(.)

# Add your sources and executables
add_executable(my_executable
    main.cpp
    core/tool/novaPCXencoding.h core/tool/novaPCXencoding.cpp
    gui/mainApplication.h      gui/mainApplication.cpp
    core/qt/qtFunctions.h      core/qt/qtFunctions.cpp
    gui/qtHelpers/guiSetup.h   gui/qtHelpers/guiSetup.cpp
    gui/qtHelpers/guiSetup.hpp
    gui/editors.cpp
    gui/mapEditor.h gui/mapEditor.cpp
    gui/pffEditor.h gui/pffEditor.cpp
    gui/pffEditorActions.h
    gui/pffEditorActions.cpp

    core/obj/gameMap.cpp
    core/obj/gameMap.h


## planExp Upgrades tested here

    planExp/miscdef.h
    planExp/miscdef.cpp
    planExp/tools/maths.cpp
    planExp/tools/dataStructure.h
    planExp/tools/dataStructure.cpp
    planExp/tools/utils.h
    planExp/tools/utils.cpp
    planExp/tools/threads.cpp
    Instanciator.cpp

    planExp/tools/textMgmt.cpp
    planExp/tools/textMgmt.h

    planExp/io/HexaParser.h
    planExp/io/HexaParserFunctions.hpp
    planExp/io/HexaParserFunctions.cpp
    planExp/io/HexaParser.cpp
    planExp/io/pffParser.hpp planExp/io/pffParser.cpp




)

# Link Qt6 and OpenCV to your executable
target_link_libraries(my_executable
    ${OpenCV_LIBS}
    Qt6::Core Qt6::Widgets Qt6::Gui
)
