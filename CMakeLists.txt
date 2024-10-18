set(TARGET_NAME IotMacroPad)

add_executable(IotMacroPad
        IotMacroPad.c
        dev/keypad.c
        dev/buzzer.c
        dev/encoder.c
        dev/ST7735_TFT.c
        dev/ST7735_TFT_Font.c
        dev/ST7735_TFT_graphics.c
        ui.c
        )

include_directories(${CMAKE_CURRENT_LIST_DIR}/dev)

target_link_libraries(IotMacroPad PRIVATE
        pico_stdlib
        hardware_spi
        hardware_dma
        hardware_pwm
        ETHERNET_FILES
        IOLIBRARY_FILES
        DHCP_FILES
        DNS_FILES
        MQTT_FILES
        TIMER_FILES
        )

pico_add_extra_outputs(IotMacroPad)

pico_enable_stdio_usb(IotMacroPad 1)
pico_enable_stdio_uart(IotMacroPad 0)