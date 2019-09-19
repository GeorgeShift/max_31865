/*
 * max31865.h
 *
 * Created: 13.9.2019 12:37:00
 * Author: strakos.p
 */ 

#ifndef MAX31865_H_
#define MAX31865_H_

	// Define your CPU frequency
	#define F_CPU 8000000
	/* ********************************* */
	
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/delay.h>
	
	// Define your SPI pins
	/* ********************************* */
	#define CS_PORT		PORTB
	#define CS_PIN		0
	#define DRDY_PORT	PORTB
	#define DRDY_PIN	1
	#define MOSI_PORT	PORTB
	#define MOSI_PIN	3
	#define MISO_PORT	PORTB
	#define MISO_PIN	4
	#define SCK_PORT	PORTB
	#define SCK_PIN		5
	
	//Registers defined in Table 1 on page 12 of the datasheet
	#define READ_CONFIGURATION				0b00000000	// 0x00H
	#define CONFIGURATION					0b10000000	// 0x80H
	#define READ_RTD_MSB					0b00000001	// 0x01H
	#define READ_RTD_LSB					0b00000010	// 0x02H
	#define READ_HIGH_FAULT_TRESHOLD_MSB	0b00000011	// 0x03H
	#define WRITE_HIGH_FAULT_TRESHOLD_MSB	0b10000011	// 0x83H
	#define READ_HIGH_FAULT_TRESHOLD_LSB	0b00000100	// 0x04H
	#define WRITE_HIGH_FAULT_TRESHOLD_LSB	0b10000100	// 0x84H
	#define READ_LOW_FAULT_TRESHOLD_MSB		0b00000101	// 0x05H
	#define WRITE_LOW_FAULT_TRESHOLD_MSB	0b10000101	// 0x85H
	#define READ_LOW_FAULT_TRESHOLD_LSB		0b00000110	// 0x06H
	#define WRITE_LOW_FAULT_TRESHOLD_LSB	0b10000110	// 0x86H
	#define FAULT_STATUS					0b00000111	// 0x07H
	
	extern char max_spi_read(char addr);
	extern void max_spi_write(char addr, char data);
	extern void max_init_port(void);
	extern uint8_t init_max(void);
	extern uint8_t max_fault_test(void);
	extern long max_get_data(char datatype);
		
#endif /* MAX31865_H_ */