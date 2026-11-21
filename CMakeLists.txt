cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(LevelCollaborations VERSION 1.0.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE MODULES CONFIGURE_DEPENDS src/*.cppm)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME}
    PRIVATE ${SOURCES}
    PRIVATE FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${MODULES}
)

target_include_directories(${PROJECT_NAME} PRIVATE src include)

if(NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define the GEODE_SDK environment variable to point to Geode.")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})

option(CW_TESTING "Enable local testing mode" ON)
if(CW_TESTING)
    target_compile_definitions(${PROJECT_NAME} PRIVATE CW_TESTING=1)
endif()

CPMAddPackage("gh:dankmeme01/cue#main")
CPMAddPackage("gh:Cheeseworks/rhatts-geode-sillies#main")
target_link_libraries(${PROJECT_NAME} cue rhatt)
