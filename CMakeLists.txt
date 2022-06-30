cmake_minimum_required(VERSION 3.20.0)
project(TrafficMapper
        VERSION 1.0.0
        LANGUAGES CXX
        DESCRIPTION "Traffic analysis using neural networks.")

set(CMAKE_CXX_STANDARD 23 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MSVC_VERSION 1929)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra)
    if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        add_compile_options(-Werror)
    endif()
else()
    add_compile_options(/permissive-)
endif()


file(GLOB_RECURSE resource_files ${CMAKE_CURRENT_SOURCE_DIR}/*.*)
foreach(res_file ${resource_files})
	file(RELATIVE_PATH rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${res_file})
	string(FIND ${rel_path} / is_valid)
	if (NOT is_valid EQUAL -1)
		list(APPEND rel_paths ${rel_path})
	endif()
endforeach()

set(qrc_file ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)
file(WRITE ${qrc_file} "")
file(APPEND ${qrc_file} "<RCC>\n    <qresource prefix=\"/\">\n")
foreach(rel_path ${rel_paths})
	file(APPEND ${qrc_file} "        <file>${rel_path}</file>\n")
endforeach()
file(APPEND ${qrc_file} "    </qresource>\n</RCC>")


<<<<<<< HEAD
#list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake/OpenCV_4.4.0)
#list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake/Qt_5.15.2_msvc2019_64)
#list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake/boost_1_78_0)
#list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/cmake/Eigen3)
=======
list(APPEND CMAKE_PREFIX_PATH D:/Frameworks/OpenCV_4.4.0)
list(APPEND CMAKE_PREFIX_PATH D:/Frameworks/Qt/5.15.2/msvc2019_64)
list(APPEND CMAKE_PREFIX_PATH D:/Frameworks/boost_1_78_0)
list(APPEND CMAKE_PREFIX_PATH D:/Frameworks/Eigen3)
>>>>>>> b11c027b401d519d5082617939a6bc347c6a1b8c

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
find_package(Qt5 COMPONENTS ${Qt_MODULES} REQUIRED)
find_package(Boost 1.77 REQUIRED)
find_package(Eigen3 3.4 REQUIRED NO_MODULE)

foreach(module ${Qt_MODULES})
    list(APPEND Qt5_INCLUDE_DIRS ${Qt5${module}_INCLUDE_DIRS})
endforeach()
list(REMOVE_DUPLICATES Qt5_INCLUDE_DIRS)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Qt5_INCLUDE_DIRS})
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
    target_link_libraries(${PROJECT_NAME} ${Qt5${module}_LIBRARIES})
endforeach()
target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries (${PROJECT_NAME} Eigen3::Eigen)


install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(CODE
    "execute_process(
<<<<<<< HEAD
        COMMAND ${CMAKE_SOURCE_DIR}/external/cmake/Qt_5.15.2_msvc2019_64/bin/windeployqt.exe
=======
        COMMAND ${CMAKE_SOURCE_DIR}/external/Qt_5.15.2_msvc2019_64/bin/windeployqt.exe
>>>>>>> b11c027b401d519d5082617939a6bc347c6a1b8c
            --qmldir
            ${CMAKE_SOURCE_DIR}/extras/qml
            ${PROJECT_NAME}.exe
        WORKING_DIRECTORY
            ${CMAKE_SOURCE_DIR}/bin)"
)
install(
    FILES
<<<<<<< HEAD
        ${CMAKE_SOURCE_DIR}/external/cmake/OpenCV_4.4.0/x64/vc16/bin/opencv_world440.dll
=======
        ${CMAKE_SOURCE_DIR}/external/OpenCV_4.4.0/x64/vc16/bin/opencv_world440.dll
>>>>>>> b11c027b401d519d5082617939a6bc347c6a1b8c
    DESTINATION
        ${CMAKE_SOURCE_DIR}/bin
)