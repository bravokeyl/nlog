*****************************************************************************
** ChibiOS/RT port for ARM-Cortex-M4 STM32F407.
** With FatFS lwIP stpm34                          
*****************************************************************************

** TARGET **

The code runs on a STM32F4-Discovery board + Embest DM-STF4BB (STM32F4-DISBB)


The MicroSD port on the Embest board is used for FATfs actions though SDIO.
    
1. Configure the IP address of the board in ChibiOS_2.6.1\os\various\lwip_bindings\lwipthread.h,
   or you can override it in lwipopts.h.
   
2. Configure the IP address of server that receives sensor data in senddata.h

3. I change the project with just make. You need to configure all the compiling options in Makefile
   So import the project with a Makefile project .
   
   
4. RTC Syn. https://techoverflow.net/2014/11/01/using-the-lwip-sntp-client-with-chibios/?q=/blog/2014/11/01/using-lwip-sntp-client-with-chibios/

5. see How pins are connected for four STPM34 Connection in readstpm.h USART1 2 3 6 are used.

5. UART4/5 and MicroSD cannot work simultaneously.

6. Be careful, Device 1 is only in charge of Channel 1 and 2; Device 2: channel 3 and 4; Device 3: channel 5 and 6; Device 4: channel 7 and 8;