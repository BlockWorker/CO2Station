/**
  PIN MANAGER Generated Driver File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.c

  @Summary:
    This is the generated manager file for the PIC24 / dsPIC33 / PIC32MM MCUs device.  This manager
    configures the pins direction, initial state, analog setting.
    The peripheral pin select, PPS, configuration is also handled by this manager.

  @Description:
    This source file provides implementations for PIN MANAGER.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  PIC24FJ512GU405
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/


/**
    Section: Includes
*/

#include <xc.h>
#include <stdio.h>
#include "pin_manager.h"

/**
 Section: File specific functions
*/
void (*MEASURE_SW_InterruptHandler)(void) = NULL;
void (*POWER_SW_InterruptHandler)(void) = NULL;
void (*USB_VBUS_InterruptHandler)(void) = NULL;

/**
 Section: Driver Interface Function Definitions
*/
void PIN_MANAGER_Initialize (void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATB = 0x0040;
    LATC = 0x0000;
    LATD = 0x0000;
    LATE = 0x0000;
    LATF = 0x0000;
    LATG = 0x0080;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISB = 0x0533;
    TRISC = 0x3000;
    TRISD = 0x0CF9;
    TRISE = 0x00E3;
    TRISF = 0x0092;
    TRISG = 0x010C;

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPDB = 0x0000;
    CNPDC = 0x0000;
    CNPDD = 0x0000;
    CNPDE = 0x0000;
    CNPDF = 0x0000;
    CNPDG = 0x0000;
    CNPUB = 0x0000;
    CNPUC = 0x0000;
    CNPUD = 0x0000;
    CNPUE = 0x0000;
    CNPUF = 0x0000;
    CNPUG = 0x0000;
    IOCPDB = 0x0000;
    IOCPDC = 0x0000;
    IOCPDD = 0x0000;
    IOCPDE = 0x0000;
    IOCPDF = 0x0090;
    IOCPDG = 0x0100;
    IOCPUB = 0x0000;
    IOCPUC = 0x0000;
    IOCPUD = 0x0000;
    IOCPUE = 0x0003;
    IOCPUF = 0x0000;
    IOCPUG = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCB = 0x0000;
    ODCC = 0x0000;
    ODCD = 0x0000;
    ODCE = 0x0000;
    ODCF = 0x0000;
    ODCG = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSB = 0x0400;
    ANSD = 0x0CC0;
    ANSE = 0x0000;
    ANSG = 0x0000;
    
    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS

    RPOR3bits.RP7R = 0x0007;    //RB7->SPI1:SDO1
    RPOR4bits.RP8R = 0x0008;    //RB8->SPI1:SCK1
    RPINR20bits.SCK1R = 0x0008;    //RB8->SPI1:SCK1

    __builtin_write_OSCCONL(OSCCON | 0x40); // lock PPS
    
    /****************************************************************************
     * Interrupt On Change: positive
     ***************************************************************************/
    IOCPFbits.IOCPF7 = 1;    //Pin : RF7
    /****************************************************************************
     * Interrupt On Change: negative
     ***************************************************************************/
    IOCNEbits.IOCNE0 = 1;    //Pin : RE0
    IOCNEbits.IOCNE1 = 1;    //Pin : RE1
    /****************************************************************************
     * Interrupt On Change: flag
     ***************************************************************************/
    IOCFEbits.IOCFE0 = 0;    //Pin : RE0
    IOCFEbits.IOCFE1 = 0;    //Pin : RE1
    IOCFFbits.IOCFF7 = 0;    //Pin : RF7
    /****************************************************************************
     * Interrupt On Change: config
     ***************************************************************************/
    PADCONbits.IOCON = 1;    //Config for PORTE
    
    /* Initialize IOC Interrupt Handler*/
    MEASURE_SW_SetInterruptHandler(&MEASURE_SW_CallBack);
    POWER_SW_SetInterruptHandler(&POWER_SW_CallBack);
    USB_VBUS_SetInterruptHandler(&USB_VBUS_CallBack);
    
    /****************************************************************************
     * Interrupt On Change: Interrupt Enable
     ***************************************************************************/
    IFS1bits.IOCIF = 0; //Clear IOCI interrupt flag
    IEC1bits.IOCIE = 1; //Enable IOCI interrupt
}

void __attribute__ ((weak)) MEASURE_SW_CallBack(void)
{

}

void __attribute__ ((weak)) POWER_SW_CallBack(void)
{

}

void __attribute__ ((weak)) USB_VBUS_CallBack(void)
{

}

void MEASURE_SW_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC1bits.IOCIE = 0; //Disable IOCI interrupt
    MEASURE_SW_InterruptHandler = InterruptHandler; 
    IEC1bits.IOCIE = 1; //Enable IOCI interrupt
}

void MEASURE_SW_SetIOCInterruptHandler(void *handler)
{ 
    MEASURE_SW_SetInterruptHandler(handler);
}

void POWER_SW_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC1bits.IOCIE = 0; //Disable IOCI interrupt
    POWER_SW_InterruptHandler = InterruptHandler; 
    IEC1bits.IOCIE = 1; //Enable IOCI interrupt
}

void POWER_SW_SetIOCInterruptHandler(void *handler)
{ 
    POWER_SW_SetInterruptHandler(handler);
}

void USB_VBUS_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC1bits.IOCIE = 0; //Disable IOCI interrupt
    USB_VBUS_InterruptHandler = InterruptHandler; 
    IEC1bits.IOCIE = 1; //Enable IOCI interrupt
}

void USB_VBUS_SetIOCInterruptHandler(void *handler)
{ 
    USB_VBUS_SetInterruptHandler(handler);
}

/* Interrupt service routine for the IOCI interrupt. */
void __attribute__ (( interrupt, no_auto_psv )) _IOCInterrupt ( void )
{
    if(IFS1bits.IOCIF == 1)
    {
        if(IOCFEbits.IOCFE1 == 1)
        {
            if(MEASURE_SW_InterruptHandler) 
            { 
                MEASURE_SW_InterruptHandler(); 
            }
            
            IOCFEbits.IOCFE1 = 0;  //Clear flag for Pin - RE1

        }
        
        if(IOCFEbits.IOCFE0 == 1)
        {
            if(POWER_SW_InterruptHandler) 
            { 
                POWER_SW_InterruptHandler(); 
            }
            
            IOCFEbits.IOCFE0 = 0;  //Clear flag for Pin - RE0

        }
        
        if(IOCFFbits.IOCFF7 == 1)
        {
            if(USB_VBUS_InterruptHandler) 
            { 
                USB_VBUS_InterruptHandler(); 
            }
            
            IOCFFbits.IOCFF7 = 0;  //Clear flag for Pin - RF7

        }
        
        
        // Clear the flag
        IFS1bits.IOCIF = 0;
    }
}

