/*
 * adc.h
 *
 *  Created on: Jun 17, 2015
 *      Author: Bryce
 */

#ifndef ADC14_SINGLE_CONVERSION_REPEAT_TIMERA_SOURCE_H_
#define ADC14_SINGLE_CONVERSION_REPEAT_TIMERA_SOURCE_H_

//*****************************************************************************
//
//! Enables a 14bit ADC that takes:
//! 	1) a reference V+(3.3) at P5.6
//! 	2) a reference V-(Gnd) at P5.7
//!		3) analog input signal at P5.5
//
//*****************************************************************************
void ADC_Init(void);

#endif /* ADC14_SINGLE_CONVERSION_REPEAT_TIMERA_SOURCE_H_ */
