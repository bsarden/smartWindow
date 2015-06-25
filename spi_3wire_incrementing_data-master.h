/*
 * spi_3wire_incrementing_data-master.h
 *
 *  Created on: Jun 23, 2015
 *      Author: Bryce
 */

#ifndef SPI_3WIRE_INCREMENTING_DATA_MASTER_H_
#define SPI_3WIRE_INCREMENTING_DATA_MASTER_H_


//*****************************************************************************
//
//! Initializes the SPI Communication with our 12 bit DAC
//! 	P1.3 -> Data Out (UCA0SIMO)
//! 	P1.2 -> Data In (UCA0SOMI)
//!		P1.1 -> Serial Clock Out (UCA0CLK)
//
//*****************************************************************************
void SPI_Init(void);


#endif /* SPI_3WIRE_INCREMENTING_DATA_MASTER_H_ */
