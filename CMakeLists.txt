cmake_minimum_required(VERSION 3.13)
project(switch_bridge_pico)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
pico_sdk_init()

add_executable(switch_bridge_pico
    main.c
)

target_include_directories(switch_bridge_pico PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(switch_bridge_pico pico_stdlib tinyusb_device tinyusb_host tinyusb_board)

pico_add_extra_outputs(switch_bridge_pico)
