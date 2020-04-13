cmake_minimum_required(VERSION 3.0.0)
project(DC_Motor_Control VERSION 0.1.0)

# Set the board file
set(CHIBIOS_BOARD_CMAKE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Chibios_18.2.x/os/hal/boards/STM32F103C8_MINIMAL/board.cmake)

# Set the toolchain
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Chibios_18.2.x/toolchain.cmake)

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions("-DOSAL_DBG_ENABLE_ASSERTS=false -DOSAL_DBG_ENABLE_CHECKS=false")
else()
    add_definitions("-DOSAL_DBG_ENABLE_ASSERTS=true -DOSAL_DBG_ENABLE_CHECKS=true")
endif()

# Include chibios_contrib hal file too
include(${CMAKE_CURRENT_SOURCE_DIR}/ChibiOS-Contrib/os/hal/hal.cmake)

# Include chibios_contrib platform directory
include(${CMAKE_CURRENT_SOURCE_DIR}/ChibiOS-Contrib/os/hal/ports/STM32/STM32F1xx/platform_contrib.cmake)

# Set Headers needed
set(HEADERS
    chconf.h
    halconf.h
    mcuconf.h
    usbcfg.h
    halconf_community.h
    mcuconf_community.h
    QuadEncoder.hpp
    PIDController.hpp
    LowpassFilter.hpp)

set(SOURCES
    ${CHIBIOS_CONTRIB_HAL_SRC}
    ${CHIBIOS_CONTRIB_PLATFORM_SRC}
    QuadEncoder.cpp
    usbcfg.c
    main.cpp)

include_directories(${CHIBIOS_CONTRIB_HAL_INCLUDE_DIR})
include_directories(${CHIBIOS_CONTRIB_PLATFORM_INCLUDE_DIR})
# Include chibios
include(${CMAKE_CURRENT_SOURCE_DIR}/Chibios_18.2.x/chibios.cmake)
# For printing float values
add_definitions("-DCHPRINTF_USE_FLOAT")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ros_lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
add_chibios_executable(DC_Motor_Control
                       ${HEADERS}
                       ${SOURCES})
