/*
 * -------------------------------------------
 *    MSP432 DriverLib - v2_20_00_08 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 SPI - 3-wire Master Incremented Data
 *
 * This example shows how SPI master talks to SPI slave using 3-wire mode.
 * Incrementing data is sent by the master starting at 0x01. Received data is
 * expected to be same as the previous transmission.  eUSCI RX ISR is used to
 * handle communication with the CPU, normally in Sleep. Because all execution 
 * after Sleep is in ISRs, initialization waits for DCO to stabilize against 
 * ACLK.
 *
 * Note that in this example, EUSCIA0 is used for the SPI port. If the user
 * wants to use EUSCIB for SPI operation, they are able to with the same APIs
 * with the EUSCI_BX parameters.
 *
 * ACLK = ~32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2
 *
 * Use with SPI Slave Data Echo code example.
 *
 *                MSP432P401
 *              -----------------
 *             |                 |
 *             |                 |
 *             |                 |
 *             |             P1.3|-> Data Out (UCA0SIMO)
 *             |                 |
 *             |             P1.2|<- Data In (UCA0SOMI)
 *             |                 |
 *             |             P1.1|-> Serial Clock Out (UCA0CLK)
 * Author: Timothy Logan
*******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>

#include <stdbool.h>

/* Global Definitions of the SSI bytes */
volatile uint32_t RXData = 0;
volatile uint32_t TXData = 0;

/* SPI Master Configuration Parameter */
const eUSCI_SPI_MasterConfig spiMasterConfig = {
        EUSCI_A_SPI_CLOCKSOURCE_ACLK,              // ACLK Clock Source
        32768,                                     // ACLK = LFXT = 32.768khz
        500000,                                    // SPICLK = 500khz
        EUSCI_A_SPI_MSB_FIRST,                     // MSB First
        EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase
        EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH, // High polarity
        EUSCI_A_SPI_3PIN                           // 3Wire SPI Mode
};

//*****************************************************************************
//
//! Initializes the SPI Communication with our 12 bit DAC
//! 	P1.3 -> Data Out (UCA0SIMO)
//! 	P1.2 -> Data In (UCA0SOMI)
//!		P1.1 -> Serial Clock Out (UCA0CLK)
//
//*****************************************************************************
void SPI_Init(void) {
    volatile uint32_t ii;

    /* Halting WDT  */
    WDT_A_holdTimer();

    /* Starting and enabling LFXT (32kHz) */
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
            GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32768, 0);
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_startLFXT(CS_LFXT_DRIVE0);

    /* Selecting P1.1 P1.2 and P1.3 in SPI mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI in 3wire master mode */
    SPI_initMaster(EUSCI_A0_MODULE, &spiMasterConfig);

    /* Enable SPI module */
    SPI_enableModule(EUSCI_A0_MODULE);

    /* Enabling interrupts */
    SPI_enableInterrupt(EUSCI_A0_MODULE, EUSCI_A_SPI_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);
    Interrupt_enableSleepOnIsrExit();

    /* Delaying waiting for the module to initialize */
    for(ii=0;ii<1000;ii++);

    TXData = 0x1;

    /* Polling to see if the TX buffer is ready */
    while (!(SPI_getInterruptStatus(EUSCI_A0_MODULE,EUSCI_A_SPI_TRANSMIT_INTERRUPT)));

    /* Transmitting data to slave
     * Starts the communication process
     * Initializes data transmission with slave on first receive interrupt
     */
    SPI_transmitData(EUSCI_A0_MODULE, TXData);

//    PCM_gotoLPM0();
//    __no_operation();
}

void euscia0_isr(void) {
    uint32_t status = SPI_getEnabledInterruptStatus(EUSCI_A0_MODULE);
    uint32_t jj;
    //static uint32_t data = 0;

    SPI_clearInterruptFlag(EUSCI_A0_MODULE, status);

    if(status & EUSCI_A_SPI_RECEIVE_INTERRUPT)
    {
        /* USCI_A0 TX buffer ready? */
        while (!(SPI_getInterruptStatus(EUSCI_A0_MODULE, EUSCI_A_SPI_TRANSMIT_INTERRUPT)));

        RXData = SPI_receiveData(EUSCI_A0_MODULE);

        /* Send the next data packet */
        SPI_transmitData(EUSCI_A0_MODULE, ++TXData);

        /* Delay between transmissions for slave to process information */
        for(jj=50;jj<50;jj++);
    }

}
