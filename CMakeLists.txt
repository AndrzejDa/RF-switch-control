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

find_package(Qt5 COMPONENTS Widgets REQUIRED SerialPort)

include_directories(inc)

file(GLOB HEADER_FILES "inc/*.hpp")
file(GLOB SOURCES "src/*.cpp")


set(CMAKE_BUILD_TYPE Debug)
add_executable(RFswitchController ${SOURCES} ${HEADER_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::SerialPort)