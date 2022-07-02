cmake_minimum_required(VERSION 3.20.0)
project(TrafficMapper
        VERSION 1.0.0
        LANGUAGES CXX
        DESCRIPTION "Traffic analysis using neural networks.")

set(CMAKE_CXX_STANDARD 23 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(MSVC_VERSION 1929)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra)
    if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        add_compile_options(-Werror)
    endif()
else()
    add_compile_options(/permissive-)
endif()

file(GLOB FOLDER_CONTENT ${CMAKE_SOURCE_DIR}/external/cmake/*)
foreach(ITEM ${FOLDER_CONTENT})
    if(IS_DIRECTORY ${ITEM})
        list(APPEND CMAKE_PREFIX_PATH ${ITEM})
    endif()
endforeach()

add_subdirectory(extras)

set(Qt_MODULES Widgets Quick QuickControls2 Gui Qml Charts Multimedia PrintSupport WebEngine WebEngineWidgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)

find_package(OpenCV REQUIRED)
find_package(Qt5 5.15 REQUIRED COMPONENTS ${Qt_MODULES})
find_package(Boost 1.77 REQUIRED)
find_package(Eigen3 3.4 REQUIRED NO_MODULE)

foreach(module ${Qt_MODULES})
    list(APPEND Qt_INCLUDE_DIRS ${Qt5${module}_INCLUDE_DIRS})
endforeach()
list(REMOVE_DUPLICATES Qt_INCLUDE_DIRS)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Qt_INCLUDE_DIRS})
message(STATUS ${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/includes)
include_directories(${CMAKE_SOURCE_DIR}/external/includes)

if(NOT DEFINED HEADERS)
    file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/src/*.hpp)
endif()

if(NOT DEFINED SOURCES)
    file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
endif()

qt5_add_resources(QT_RESOURCES extras/resources.qrc)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${QT_RESOURCES})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
foreach(module ${Qt_MODULES})
    target_link_libraries(${PROJECT_NAME} ${Qt${module}_LIBRARIES})
endforeach()
target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries (${PROJECT_NAME} Eigen3::Eigen)

foreach(ITEM ${CMAKE_PREFIX_PATH})
    if(ITEM MATCHES ".*([Oo][Pp][Ee][Nn])?_?[Cc][Vv][^\/]*")
        file(GLOB_RECURSE DLL_FILES ${ITEM}/*.dll)
        foreach(DLL_FILE ${DLL_FILES})
            # if(DLL_FILE MATCHES ".*\/vc15\/.*opencv_world[0-9]{3}\.dll$")
            if(DLL_FILE MATCHES ".*\/vc15\/.*opencv_world[0-9][0-9][0-9]\.dll$")
				set(OpenCV_dll_location ${DLL_FILE})
            endif()
        endforeach()
    endif()
endforeach()

get_target_property(qt_qmake_location Qt::qmake IMPORTED_LOCATION)
string(REPLACE "qmake" "windeployqt" windeployqt_location ${qt_qmake_location})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(CODE
    "execute_process(
        COMMAND ${windeployqt_location}
            --qmldir
            ${CMAKE_SOURCE_DIR}/extras/qml
            ${PROJECT_NAME}.exe
        WORKING_DIRECTORY
            ${CMAKE_SOURCE_DIR}/bin)"
)
install(
    FILES
        ${OpenCV_dll_location}
    DESTINATION
        ${CMAKE_SOURCE_DIR}/bin
)