This is the AVR module for basic interfacing MAX31865 temperature sensor.

Sensor using SPI interface to communicate. 4-pins are required to interface.

![alt text](https://github.com/GeorgeShift/max_31865/blob/master/typical_application.png)

## Install
- add files (max31865.h and max31865.c) to your project
- update search path for the include file (if required)
- set your SPI pins and CPU frequency settings in max31865.h
- do not forget include max31865.h in your project

## Setup
Here is example of use:

```
#define F_CPU 8000000

#include "max31865.h"
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    uint8_t temp = 0;
    uint8_t Fault_Error;
    uint8_t max_connected;
    long temperature;

    max_init_port();	
    _delay_ms(2000);
    
    // Initializes communication with max
    // If communication is done properly function returns 1, otherwise returns 0
    max_connected = init_max();
    
    if (max_connected) // Communication successful with max31865, do something
    else // Unable to communicate with the device, do something

    // Set max CONFIGURATION register - datasheet page 13
    /* 
            D7 - Vbias (1-on, 0-off)
            D6 - Conversion mode (1-auto, 0-normally off)
            D5 - Single shot (1-1shot)
            D4 - Wiring (1-3wire, 0-2/4wire)
            D3,2 - Fault detection cycle control (datasheet page 14, table 3)
            D1 - Fault status clear (1 - clear fault status register)
            D0 - 50Hz/60Hz filter (1-50Hz, 0-60Hz)
    */
    max_spi_write(CONFIGURATION, 0b11000000); // Vbias on, auto mode
		
    while (1) 
    {	
        // Test if there is any fault detected
        if (max_fault_test() == 0)	// No fault
        {
            // Read data with parameter 't'-temperature
            // Returns temperature multiplied by 10
            temperature = max_get_data('t');

            // Check is there is new conversion available
            if (temperature == 1000) // No conversion available, do domething
            else // Do something with temperature
        }
        // Any fault have been detected
        else
        {
            // Here you can read the type of fault from fault status register
            // Datasheet page 16
            Fault_Error = max_spi_read(FAULT_STATUS);
            
            temp = Fault_Error & 0x80;      
            if(temp>0) // Fault bit D7 is Set
            temp = Fault_Error & 0x40;
            if(temp>0) // Fault bit D6 is Set
            temp = Fault_Error & 0x20;
            if(temp>0) // Fault bit D5 is Set
            temp = Fault_Error & 0x10;
            if(temp>0) // Fault bit D4 is Set
            temp = Fault_Error &0x08;
            if(temp>0) // Fault bit D3 is Set
            temp = Fault_Error &0x04;
            if(temp>0) // Fault bit D2 is Set            

            // Fault register isn't cleared automatically. Users are expected to clear it after every fault.
            max_spi_write(CONFIGURATION, 0b10000010); 
            _delay_ms(700);

            // Setting the device in auto configuration again.
            max_spi_write(CONFIGURATION, 0b11000000); 
            _delay_ms(700);			
        }

        _delay_ms(1000);		
    }
}
```
