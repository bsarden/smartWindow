/*
 * systick_interrupt_gpio_blink.h
 *
 *  Created on: Jun 17, 2015
 *      Author: Bryce
 */

#ifndef SYSTICK_INTERRUPT_GPIO_BLINK_H_
#define SYSTICK_INTERRUPT_GPIO_BLINK_H_


//*****************************************************************************
//
//! Enables a heartbeat LED on P1.0 that toggles every .5sec using SysTick
//
//*****************************************************************************
void Heartbeat_Init(void);

#endif /* SYSTICK_INTERRUPT_GPIO_BLINK_H_ */
