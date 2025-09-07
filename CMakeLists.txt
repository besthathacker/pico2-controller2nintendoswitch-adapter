cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(switch_bridge_pico C CXX ASM)

pico_sdk_init()

add_executable(switch_bridge_pico
    main.c
)

target_link_libraries(switch_bridge_pico
    pico_stdlib
    pico_unique_id
    hardware_gpio
    hardware_irq
    pico_tinyusb_device
    pico_tinyusb_host
)

pico_add_extra_outputs(switch_bridge_pico)
