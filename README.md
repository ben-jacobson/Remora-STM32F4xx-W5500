# Remora NucleoF446RE w/ W5500 Ethernet interface fork. 
------------------------------------------

Ported from Expatria Technologies fork of Remora, specifically the version maintained by cakeslob. 

# Status
- Stepgen, blink and digital IO are tested and working. 
- Software PWM yet to be ported.
- MPG module yet to be ported.

# W5500 connection
- PA_5: SCK
- PA_6: MISO
- PA_7: MOSI
- SPI_RST and SPI_CS can be configured in Platformio.ini, but by default they are PB_5 and PB_6 respectively. 
- SPI_INT is not used by this firmware

# UART connections
- PC_5: UART_RX - Firmware doesn't make use of this, but left for completeness and future expansion
- PC_10: UART_TX

# Allocation of Step Generators, IO and PWM
Please refer to the Remora documentation to configure GPIO to perform various functions like stepgen, digital IO and PWM: https://remora-docs.readthedocs.io/en/latest/configuration/configuration.html
Example config.txt files can be found in the LinuxCNC_Configs folder. 

# Software PWM
Software PWM is still WIP, please use Hardware PWM for the time being. 

# Hardware PWM
Hardware PWM is available on wide variety of pins depending on your hardware target. Please see the tables below and select the appropriate pin. Specific STM32 Timers and Channels have been allocated by the module but there are limitations to note: 
- Any two PWM pins on the same timer (TIMx) will share the same frequency, this frequency will be revert to the last pin you defined in your config.txt
- If you don't specify the frequency in your config.txt, the default is TODO
- The number of PWM pins avaialble is limited by your hardware target, with the largest being 12 PWM pins. 
- All PWM pins will have their own dedicated and adjustable duty cycle/period. All PWM is variable duty cycle enabling you to create fixed or variable depending on how you configure your HAL.  
- All PWM timers use a 16 bit wide ARR, allowing for enough fine control for things like spindle speed controls.

TODO - documentation seems to indicate max 8 PWM and this is shared with RCServo? Go back and read the docs. https://remora-docs.readthedocs.io/en/latest/configuration/Setup-Config-File.html#pwm

**Nucleo F446RE:**
| GPIO | Timer | Channel |
| ----------- | ----------- |  -----------  |
| PA_8 | TIM1 | CH1 |
| PA_9 | TIM1 | CH2 |
| PA_10 | TIM1 | CH3 |
| PA_11 | TIM1 | CH4 |
| PA_0 | TIM2 | CH1 |
| PA_1 | TIM2 | CH2 |
| PB_10 | TIM2 | CH3 |
| PB_2 | TIM2 | CH4 |
| PC_6 | TIM3 | CH1 |
| PC_7 | TIM3 | CH2 |
| PB_0 | TIM3 | CH3 |
| PB_1 | TIM3 | CH4 |

# Boards
- Nucleo F446RE : Working
- Nucleo F446ZE : Soon to be implemented, will soon become the main supported board

------------------------------------------

# Installation instructions
Start by building the firmware from source, or use the built in STLink to upload the firmware from the build directory to your Nucleo Board.

This firmware uses an almost unchanged version of the Remora-eth-0.3.0 ethernet component avaialable on the NVEM Remora port found here: https://github.com/scottalford75/Remora-RT1052-cpp/tree/main/LinuxCNC/components/Remora-eth

From your linuxcnc home folder, copy over the LinuxCNC_Configs/NucleoHat into your own config folder. Then move to the Remora-eth component folder.

Compile the component using halcompile
```
sudo halcompile --install remora-eth-3.0.c
```

Configs are loaded via tftpy, using the upload_config.py script that should be in your configuration folder:
```
pip3 install tftpy # If not using virtualenv you may get an error about breaking system packages, use the --break-system-packages flag if needed
python3 upload_config.py NucleoF411RE-Config.txt
```

Refer to the remora documents for more information
https://remora-docs.readthedocs.io/en/latest/firmware/ethernet-config.html

Board will not start until ethernet connection is established. 

All credit to Scotta, Expatria Technologies, Cakeslob and Terje IO. I didn't write this project, simply ported it and customised it to the needs of the Nucleo Hat
