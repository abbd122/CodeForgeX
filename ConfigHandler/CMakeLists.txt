cmake_minimum_required(VERSION 3.24)
project(ConfigHandler)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)
set(GENERATE_TS ON)

set(CMAKE_PREFIX_PATH "D:/qt/5.15.2/msvc2019_64")

find_package(
    Qt5
    COMPONENTS
    Core
    Gui
    Widgets
    LinguistTools
    REQUIRED
)
message("Qt version: ${Qt5Core_VERSION}")

set(SOURCES
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt5::Widgets
)

target_sources(
  ${PROJECT_NAME}
  PRIVATE
  ConfigHandler.h
  ConfigHandler.cpp
  ValueHandler.h
  ValueHandler.cpp
)