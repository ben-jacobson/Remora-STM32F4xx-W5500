# Remora NucleoF446RE w/ W5500 Ethernet interface fork. 
------------------------------------------

Ported from Expatria Technologies fork of Remora, specifically the version maintained by cakeslob. 

# Status
- Stepgen, blink and digital IO are working. 
- PWM and Encoder modules to be ported.
- MPG module to be ported.

# W5500 connection
- PA_5 SCK
- PA_6 MISO
- PA_7 MOSI
- SPI_RST and SPI_CS can be configured in Platformio.ini, but by default they are PB_5 and PB_6 respectively. 
- SPI_INT is not used by this firmware

# UART connections
- PC_5 UART_RX - Firmware doesn't make use of this, but left for completeness and future expansion
- PC_10 UART_TX

 # Step generators and IO connections
- All configured with the NucleoF411RE-Config.txt file

# Boards
- Nucleo F446RE : working, connects to linuxcnc needs more testing
  
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
