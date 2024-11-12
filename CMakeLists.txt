cmake_minimum_required(VERSION 3.1)
project(RFswitchController)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "-lthr ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
endif()

option(UHD_USE_STATIC_LIBS OFF)

find_package(UHD 4.1.0 REQUIRED)

set(UHD_BOOST_REQUIRED_COMPONENTS
    program_options
    system
    thread
    filesystem
)
set(BOOST_MIN_VERSION 1.65)
include(UHDBoost)

find_package(Qt5 COMPONENTS Widgets REQUIRED SerialPort)

include_directories(
    inc
    ${Boost_INCLUDE_DIRS}
    ${UHD_INCLUDE_DIRS}
)
link_directories(${Boost_LIBRARY_DIRS})

file(GLOB HEADER_FILES "inc/*.hpp")
file(GLOB SOURCES "src/*.cpp")

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/rx_samples_to_file.cpp")
    message(STATUS "File rx_samples_to_file.cpp exists.")
    list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/rx_samples_to_file.cpp")
endif()

set(CMAKE_BUILD_TYPE Debug)
add_executable(RFswitchController ${SOURCES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::SerialPort)

if(NOT UHD_USE_STATIC_LIBS)
    message(STATUS "Linking against shared UHD library.")
    target_link_libraries(${PROJECT_NAME} ${UHD_LIBRARIES} ${Boost_LIBRARIES})
else(NOT UHD_USE_STATIC_LIBS)
    message(STATUS "Linking against static UHD library.")
    target_link_libraries(${PROJECT_NAME}
        # We could use ${UHD_LIBRARIES}, but linking requires some extra flags,
        # so we use this convenience variable provided to us
        ${UHD_STATIC_LIB_LINK_FLAG}
        # Also, when linking statically, we need to pull in all the deps for
        # UHD as well, because the dependencies don't get resolved automatically
        ${UHD_STATIC_LIB_DEPS}
    )
endif(NOT UHD_USE_STATIC_LIBS)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/rx_samples_to_file.cpp")
    add_executable(rx_samples_to_file src/rx_samples_to_file.cpp)
    target_link_libraries(rx_samples_to_file ${UHD_LIBRARIES} ${Boost_LIBRARIES})
endif()