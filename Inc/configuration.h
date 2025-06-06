#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define PRU_BASEFREQ    	40000           // PRU Base thread ISR update frequency (hz)
#define PRU_SERVOFREQ       1000            // PRU Servo thread ISR update freqency (hz)  Ideally this is 2x higher than the servo thread frequency of LCNC.

#define BASE_SLICE          0               // IRQ Slice used by the Base thread
#define SERVO_SLICE         1               // IRQ Slice used by the Servo thread

#define STEPBIT     		22            	// bit location in DDS accum
#define STEP_MASK   		(1L<<STEPBIT)

#define JSON_BUFF_SIZE	    10000			// Jason dynamic buffer size

#define JOINTS			    6				// Number of joints - set this the same as LinuxCNC HAL compenent. Max 8 joints
#define VARIABLES           4             	// Number of command values - set this the same as the LinuxCNC HAL compenent

#define PRU_DATA		    0x64617461 	    // "data" SPI payload
#define PRU_READ            0x72656164      // "read" SPI payload
#define PRU_WRITE           0x77726974      // "writ" SPI payload
#define PRU_ESTOP           0x65737470      // "estp" SPI payload
#define PRU_ACKNOWLEDGE		0x61636b6e	    // "ackn" payload
#define PRU_ERR		        0x6572726f	    // "erro" payload

#define DATA_ERR_MAX         40


// Data buffer configuration
#define BUFFER_SIZE 		64            	// Size of recieve buffer - same as HAL component, 64

#define PLL_SYS_KHZ (125 * 1000)    // 133MHz
#define SOCKET_MACRAW 0
#define PORT_LWIPERF 5001

// Location for storage of JSON config file in Flash
#define JSON_STORAGE_ADDRESS 0x8060000
#define JSON_UPLOAD_ADDRESS 0x8050000
#define USER_FLASH_LAST_PAGE_ADDRESS  	0x8060000
#define USER_FLASH_END_ADDRESS        	0x807FFFF
#define JSON_SECTOR FLASH_SECTOR_7


// PC_13 blink for non spi1 config on Nucleo F446RE
#define DEFAULT_CONFIG {0X7B,0X0A,0X09,0X22,0X42,0X6F,0X61,0X72,0X64,0X22,0X3A,0X20,0X22,0X46,0X6C,0X65,0X78,0X69,0X48,0X41,0X4C,0X22,0X2C,0X0A,0X09,0X22,0X4D,0X6F,0X64,0X75,0X6C,0X65,0X73,0X22,0X3A,0X5B,0X0A,0X09,0X7B,0X0A,0X09,0X22,0X54,0X68,0X72,0X65,0X61,0X64,0X22,0X3A,0X20,0X22,0X53,0X65,0X72,0X76,0X6F,0X22,0X2C,0X0A,0X09,0X22,0X54,0X79,0X70,0X65,0X22,0X3A,0X20,0X22,0X42,0X6C,0X69,0X6E,0X6B,0X22,0X2C,0X0A,0X09,0X09,0X22,0X43,0X6F,0X6D,0X6D,0X65,0X6E,0X74,0X22,0X3A,0X09,0X09,0X09,0X22,0X42,0X6C,0X69,0X6E,0X6B,0X79,0X22,0X2C,0X0A,0X09,0X09,0X22,0X50,0X69,0X6E,0X22,0X3A,0X09,0X09,0X09,0X09,0X22,0X50,0X43,0X5F,0X31,0X33,0X22,0X2C,0X0A,0X09,0X09,0X22,0X46,0X72,0X65,0X71,0X75,0X65,0X6E,0X63,0X79,0X22,0X3A,0X20,0X09,0X09,0X34,0X0A,0X09,0X7D,0X0A,0X09,0X5D,0X0A,0X7D}

/* Default config contents:

{
	"Board": "FlexiHAL",
	"Modules":[
	{
	"Thread": "Servo",
	"Type": "Blink",
		"Comment":			"Blinky",
		"Pin":				"PC_13",
		"Frequency": 		4
	}
	]
}

*/

#endif