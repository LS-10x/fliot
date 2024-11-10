# fliot
FLIOT s a rasperry pi pico based fligth controller, using pi pico, servos, esc, mpu6500, gps module, data logger and nrf24l01 to build a super ligth weight yet somewhat usefull and programmable flight controller. Its goal is to perform return to home missions safely and to be an easy to extend platform.
I migth create librarys for all the components to make them integrate easier and as fast as possible as runtime is key.

slay queen

Pin assignement as of now:

GPIO	Physical	Assigned To
GP0	    Pin 1	    MPU6050 SDA
GP1	    Pin 2	    MPU6050 SCL
GP2	    Pin 4	    Unused
GP3	    Pin 5	    Unused
GP4	    Pin 6	    Servo Channel 2
GP5	    Pin 7	    Servo Channel 3
GP6	    Pin 9	    Servo Channel 4
GP7	    Pin 10	    Servo Channel 5
GP8	    Pin 11	    GPS Module RX (UART1 TX)
GP9	    Pin 12	    GPS Module TX (UART1 RX)
GP10    Pin 14	    ESC Signal
GP11	Pin 15	    Unused
GP12	Pin 17	    SD Card MISO (SPI1 RX)
GP13	Pin 18	    SD Card CS
GP14	Pin 19	    SD Card SCK (SPI1 SCK)
GP15	Pin 20	    SD Card MOSI (SPI1 TX)
GP16	Pin 21	    nRF24L01 MISO (SPI0 RX)
GP17	Pin 22	    nRF24L01 CSN
GP18	Pin 24	    nRF24L01 SCK (SPI0 SCK)
GP19	Pin 25	    nRF24L01 MOSI (SPI0 TX)
GP20	Pin 26	    Unused
GP21	Pin 27	    Unused (SWD Clock - avoid if using SWD)
GP22	Pin 29	    nRF24L01 CE
GP25	Pin 31	    Onboard LED
GP26	Pin 32	    Unused
GP27	Pin 33	    Unused
GP28	Pin 34	    Unused

