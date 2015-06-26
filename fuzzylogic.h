/*
 * fuzzylogic.h
 *
 *  Created on: Jun 25, 2015
 *      Author: Bryce
 */

#ifndef FUZZYLOGIC_H_
#define FUZZYLOGIC_H_

// standard includes
#include <stdint.h>

//*****************************************************************************
//
//! Uses fuzzy logic to convert value coming in from the ADC into an output 
//! value for the DAC 
//
//*****************************************************************************
int32_t convert_to_system_output(int32_t adcVal);

#endif /* FUZZYLOGIC_H_
