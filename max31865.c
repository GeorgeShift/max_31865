/*
 * max31865.c
 *
 * Created: 13.9.2019 12:34:48
 * Author : strakos.p
 */ 

// Includes
#include "max31865.h"

// Constant macro
#define DDR(x) (*(&x - 1))   // address of data direction register of port x
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
/* on ATmega64/128 PINF is on port 0x00 and not 0x60 */
#define PIN(x) ( &PORTF==&(x) ? _SFR_IO8(0x00) : (*(&x - 2)) )
#else
#define PIN(x) (*(&x - 2))  // address of input register of port x
#endif

// Reading data from SPI
char max_spi_read(char addr){
	
	CS_PORT &= ~_BV(CS_PIN);	// CS_LOW - activate slave
	
	SPDR = addr;				// register address to read
	while(!(SPSR & (1<<SPIF)));	// wait for complete sending

	SPDR = 0xFF;				// dummy data to provide SPI clock signals to read
	while(!(SPSR & (1<<SPIF)));	// wait for complete sending
	
	CS_PORT |= _BV(CS_PIN);		// CS_HIGH - deactivate slave
	return SPDR;				// delete flag SPIF and return data
}

// Writing data to SPI
void max_spi_write(char addr, char data){
	
	CS_PORT &= ~_BV(CS_PIN);	// CS_LOW - activate slave
	
	SPDR = addr;				// register address to write
	while(!(SPSR & (1<<SPIF)));	// wait for complete sending
	
	SPDR = data;				// data to write
	while(!(SPSR & (1<<SPIF)));	// wait for complete sending
	
	CS_PORT |= _BV(CS_PIN);		// CS_HIGH - deactivate slave
}

// Port initialization
void max_init_port(void)
{
	// SPI setting
	/* Enable SPI, Master, set mode 3, set clock rate fck/128 */
	SPCR =
	(0 << SPIE) |
	(1 << SPE)  |	// SPI enabled
	(0 << DORD) |
	(1 << MSTR) |	// Master
	(1 << CPOL) |	// Clock polarity
	(1 << CPHA) |	// Clock phase
	(1 << SPR1) |
	(1 << SPR0);	// Clock F_CPU/128

	// output DDRs
	DDR(CS_PORT) |= _BV(CS_PIN);
	DDR(MOSI_PORT) |= _BV(MOSI_PIN);
	DDR(SCK_PORT) |= _BV(SCK_PIN);
	
	// set outputs
	CS_PORT |= _BV(CS_PIN);	// CS HIGH, slave disabled
	
	// input DDRs
	DDR(DRDY_PORT) &= ~_BV(DRDY_PIN);
	DDR(MISO_PORT) &= ~_BV(MISO_PIN);
}

// Initializes communication with max
uint8_t init_max(void)
{
	/*
		If communication is done properly function returns 1, otherwise returns 0
	*/
	
	uint8_t conf = 0;
	
	max_spi_write(CONFIGURATION, 0b10000000);	// Enable V bias
	conf = max_spi_read(READ_CONFIGURATION);	// Reading Configuration register to verify communication
	
	if(conf == 0b10000000)
	{
		max_spi_write(WRITE_HIGH_FAULT_TRESHOLD_MSB, 0xFF);	// Writing High Fault Threshold MSB
		max_spi_write(WRITE_HIGH_FAULT_TRESHOLD_LSB, 0xFF);	// Writing High Fault Threshold LSB
		max_spi_write(WRITE_LOW_FAULT_TRESHOLD_MSB, 0x00);		// Writing Low Fault Threshold MSB
		max_spi_write(WRITE_LOW_FAULT_TRESHOLD_LSB, 0x00);		// Writing Low Fault Threshold LSB
		
		return 1;
	}
	else
	{
		return 0;
	}
}

// Fault test
uint8_t max_fault_test(void)
{
	/*
		Returns 0 if there is not fault detected
	*/
	
	uint8_t  lsb_rtd;
	
	lsb_rtd = max_spi_read(READ_RTD_LSB);
	
	return lsb_rtd&0x01;	
}

// Reading data from max
long max_get_data(char datatype)
{
	/*
		Parameter datatype is for choose between temperature and RTD as return value, type:
		'r' for RTD
		't' for temperature.
		
		In normal operation function returns RTD or temperature multiplexed by 10
		If new conversion result is not available - DRDY is high, returns 1000
	*/
	
	uint8_t lsb_rtd, msb_rtd;
	uint8_t DRDY_state;
	
	int RTD;
	long temperature;

	DRDY_state = DRDY_PORT&(1 << DRDY_PIN);

	if (DRDY_state == 0)
	{
		lsb_rtd = max_spi_read(READ_RTD_LSB);
		msb_rtd = max_spi_read(READ_RTD_MSB);
			
		RTD = ((msb_rtd << 7)+((lsb_rtd & 0xFE) >> 1));
		if (datatype == 'r') return RTD;
			
		// Basic temperature calculation, the accuracy for temperatures 
		// between -75°C - 100°C is max +-1.5°C
		temperature = ( (long)10*RTD >> 5) - 2560;	// RTD / 32 - 256
		if (datatype == 't') return temperature;
	}
	// return 1000 if not conversion available
	else return 1000;
}

