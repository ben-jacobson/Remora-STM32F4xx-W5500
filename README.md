# Remora NucleoF446RE w/ W5500 Ethernet interface fork. 
------------------------------------------

Ported from Expatria Technologies fork of Remora, specifically the version maintained by cakeslob. Remora was originally written by Scotta

# Status
- Stepgen, blink and digital IO are tested and working. 
- Hardware PWM is in development, it functions but there are bugs to fix
- Software PWM yet to be ported.
- MPG module yet to be ported.

# Todos
- Switch out unit tests to use whatever UART and other tested peripherals to what is defined in platformIO.ini
- Set up ability to configure UART peripheral in platformio.ini, just like we have with SPI and CS/RST pins

# Wiznet W5500 connection
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
Hardware PWM is available on a wide variety of pins depending on your hardware target. When setting up your config.txt file, you must choose a PWM enabled pin from the list below. Specific STM32 Timers and Channels will been allocated by the driver behind the scenes. Some important details about this: 
- PWM pins can be set to variable or fixed period. Configuration documentation can be found here https://remora-docs.readthedocs.io/en/latest/configuration/Setup-Config-File.html#pwm
- You may set up more than one PWM pin on the same timer (TIMx), however the period will be shared and defined by the last pin you set in your config.txt. 
- If you don't specify a fixed period in your config.txt, or if your LinuxCNC intialises this as zero, the default will become 200us
- By default, PWM automatically starts as soon as you press the eStop. You will need to configure LinuxCNC to stop and start on other conditions. For example when using it as a 0-10v Spindle control. HAL config can be found here: https://remora-docs.readthedocs.io/en/latest/software/hal-examples.html#pwm-to-0-10v-spindle-control-simple
- How many PWM pins available will be limited by your remora-eth-3.0 component, which defaults to 4. You can raise this limit by changing both remora-eth-3.h file and configuration.h file, but be sure these. You will need 1 variable for each fixed period PWM pin, but 2 for variable duty PWM.
- All PWM timers are either 16 or 32 bits wide depending on which TIMx is used. 16 bit is more than enough for very fine control over duty cycle.

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
- Nucleo F446ZE : Soon to be implemented, and will become the main supported board

------------------------------------------

# Installation instructions
Start by building the firmware from source, or use the built in STLink to upload the firmware from the build directory to your Nucleo Board.

This firmware uses the Remora-eth-0.3.0 ethernet component avaialable on the NVEM Remora port found here: https://github.com/scottalford75/Remora-RT1052-cpp/tree/main/LinuxCNC/components/Remora-eth

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

All credit to Scotta, Expatria Technologies, Cakeslob and Terje IO. I didn't write this project, only contributed some UART, PWM code and other bug fixes. 
