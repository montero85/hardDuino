################### ARDUINO UNO WIFI ###################################################

MMCU += atmega328p
BOARD_DEFINES += F_CPU=16000000L\
                 ARDUINO=10819\
                 ARDUINO_AVR_UNO_WIFI_DEV_ED\
                 ARDUINO_ARCH_AVR ESP_CH_UART\
                 ESP_CH_UART_BR=19200


