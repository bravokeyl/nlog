# List of all the related files for the extended board.
EVALSTPM34SRC = EVALSTPM34/stm32f4_discovery.c \
EVALSTPM34/stm32f4xx_hal_msp.c\
EVALSTPM34/stm32f4xx_it.c \
EVALSTPM34/stm324xg_eval_sd.c \
EVALSTPM34/Metrology/drivers/src/metrology.c \
EVALSTPM34/Metrology/drivers/src/metrology_hal.c \
EVALSTPM34/Metrology/handlers/src/handler_metrology.c \
EVALSTPM34/Metrology/tasks/src/metroTask.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_cortex.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_crc.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_dma.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_flash.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_flash_ex.c\
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_gpio.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_i2c.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_iwdg.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_pwr.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_rcc.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_sd.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_spi.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_tim.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_tim_ex.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_uart.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_hal_usart.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_ll_fsmc.c \
EVALSTPM34/system/stm32f4-hal/stm32f4xx_ll_sdmmc.c\
EVALSTPM34/system/cmsis/system_stm32f4xx.c \
EVALSTPM34/Generic/handlers/src/handler_nvram.c

      
# Required include directories
EVALSTPM34INC = EVALSTPM34/system/stm32f4-hal/\
EVALSTPM34/system/arm/ \
EVALSTPM34/system/cmsis/ \
EVALSTPM34/system/\
EVALSTPM34/system/stm32f4-hal/ \
EVALSTPM34/ \
EVALSTPM34/Metrology/include/inc/ \
EVALSTPM34/Metrology/drivers/inc/ \
EVALSTPM34/Metrology/handlers/inc/ \
EVALSTPM34/Metrology/tasks/inc/ \
EVALSTPM34/Generic/handlers/inc
