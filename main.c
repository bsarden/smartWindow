/*
 * main.c
 *
 *  Created on: Jun 17, 2015
 *      Author: Bryce
 */

// local file includes
#include <adc14_single_conversion_repeat_timera_source.h>
#include "driverlib.h"
#include "systick_interrupt_gpio_blink.h"
#include "spi_3wire_incrementing_data-master.h"
#include <stdint.h>
#include <stdbool.h>

int main(void) {
	// Sets light to blink on P1.0 every .5 sec
	Heartbeat_Init();
	// takes in values depending upon the amount of sunlight
	ADC_Init();
	// sends output of fuzzy logic control to DAC/Voltage regulator
	SPI_Init();

	// wait in low power mode
	while(1) {
		MAP_PCM_gotoLPM0();
	}
}


