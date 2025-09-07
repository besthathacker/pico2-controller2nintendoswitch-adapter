cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pico_usb_uart C CXX ASM)
pico_sdk_init()

add_executable(pico_usb_uart
    main.c
)

target_include_directories(switch_bridge_pico PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(switch_bridge_pico pico_stdlib tinyusb_device tinyusb_host tinyusb_board)

pico_add_extra_outputs(switch_bridge_pico)
