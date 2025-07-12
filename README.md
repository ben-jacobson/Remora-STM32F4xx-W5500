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
- PWM: Set up the max PWM value
- PWM: Test if using the same TIM/Channel, does the last frequency on the shared PWM set correctly as per docs below?
- PWM: Test if this work if we use the fixed period only? In which case the variable boolean should be used to lock down to fixed period to save on set point variables.  
- PWM: Another odd bug when setting period too high it defaults to 79us? Maybe an overflow?

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
Hardware PWM is available on wide variety of pins depending on your hardware target. Please see the tables below, when setting up your config.txt file, you may choose from the list below. Specific STM32 Timers and Channels will been allocated by the driver. There are some considerations to note: 
- Any two PWM pins on the same timer (TIMx) will share the same period, and will be set by the last pin defined in config.txt
- If you don't specify the period in your config.txt, the default is 200
- The number of PWM pins available will be limited by your remora-eth-3.0 component, which by default is 4. You can lift this limit by changing both remora-eth-3.h file and configuration.h file, make sure these match. You will need 2 variables for each PWM, first for the duty cycle and another for the variable period should you wish to use that. 
- All PWM pins will have their own dedicated and adjustable duty cycle/period. By default the PWM will be variable period enabling you to create fixed or variable depending on how you configure your HAL and use the set point variables in Remora.  
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
