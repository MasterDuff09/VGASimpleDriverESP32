# VGASimpleDriver For ESP32
- Project started on Wed Apr 23, 2025
- Hardware required: VGA CABLE, ESP32 and TV with VGA Port, eletrical resistors

# NOTES:
- MCU used ESP32-WROOM32 (better if ESP32-S3)
- ESP-IDF Framework for bare-metal programming and debugging (C language)
- DMA & I2S parallel mode for fast switching digital pins and responsiveness
- Built-in DAC used to get to required voltage levels for RGB signals


This project has been started for an University Project (Università La Sapienza Di Roma). It implements a bare VGA driver for an ESP32. It will follow an update once I close this project.
The main goal for this project is to create a way to send ASCII characters from the Host PC to the MCU through Serial Comm, handle it and then send it from the MCU to a TV through VGA. 

This project is based on the following works, researches and websites:
- VGA timing table http://www.tinyvga.com/vga-timing
- Ben Eater Youtube Video 1 https://youtu.be/l7rce6IQDWs?si=zNHAmS7S074blLvp
- Ben Eater Youtube VIdeo 2 https://youtu.be/uqY3FMuMuRo?si=dUs1h0dOo-sPSS3M
- Bitluni Youtube Video 1 https://youtu.be/G70CZLPjsXU?si=ko8MXyLOYsmnInwa
- Bitluni Youtube Video 2 https://youtu.be/qJ68fRff5_k?si=IU3CtBIWtU_ItHb_
- Bitluni Github Repo https://github.com/bitluni/ESP32Lib



# SPECIFICS
BASIC VGA works by syncronizing every RGB pixel by some specific periods given by the resolution and frequency choosen. The full explanation can be found on the videos I've linked above. Following the table of various timings, I choose for this project a resolution of 800x600 which has a pixel clock at 40 Mhz, a common frequency that a MCU such as the ESP32 can handle without any kind of concerns.
It must be said that normal digital toggling is not used (for H-SYNC and V-SYNC) but a combination of I2S, a peripheral which used mostly for audio sampling (fast sampling and toggling signals) and DMA (Direct Memory Access) so that CPU is bypassed, thus no delays are encountered. Other specifics can be found by looking at the code.






