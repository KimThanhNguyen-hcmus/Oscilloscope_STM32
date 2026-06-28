idf_component_register(
    SRCS "Recieved.c"
         "system/connectivity/spi.c"
         "system/connectivity/usart.c"
         "system/screen/tft.c"
    INCLUDE_DIRS "."
                 "system/connectivity"
                 "system/screen"
)