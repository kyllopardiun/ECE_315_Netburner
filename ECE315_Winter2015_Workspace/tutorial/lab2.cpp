#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include <sim5234.h>
#include <cfinter.h>
#include "keypad.h"
#include "lcd.h"
#include <string.h>

extern "C" {
void UserMain(void * pd);
void changePosition(char * message, unsigned char *currentScreen, unsigned char *currentPosition);
void toggleScreen(unsigned char *currentScreen);
void IRQIntInit(void);
void SetIntc(int intc, long func, int vector, int level, int prio);
}

const char * AppName="Rodrigo & Kirby";

/* Instantiate Queue objects */

#define QSIZE (50)

OS_Q print_q;
void * QStorage[QSIZE];

Keypad  myKeypad;
Lcd		myLCD;

/* Cursor character */
const char cursor = '*';


void UserMain(void * pd) {
	BYTE err = OS_NO_ERR;

    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    StartHTTP();
    EnableTaskMonitor();

    #ifndef _DEBUG
    EnableSmartTraps();
    #endif

    #ifdef _DEBUG
    InitializeNetworkGDB_and_Wait();
    #endif

    iprintf("Application started: %s\n", AppName);

    myKeypad.Init();
    myLCD.Init(LCD_BOTH_SCR);
    OSTimeDly(TICKS_PER_SECOND*1);

    /* Declare interrupt. */
    IRQIntInit();

    /* Initialize your queue and interrupt here */
    err = display_error ("Initialize Queue: ",
            OSQInit(&print_q, QStorage, QSIZE)
	);

    /* cursor tracking state */
    unsigned char currentScreen = LCD_UPPER_SCR;
    unsigned char currentPosition = 50;

    while (TRUE) {
    	/* queue usage and LCD manipulation */
    	myLCD.Clear(LCD_BOTH_SCR);
    	myLCD.MoveCursor(currentScreen, currentPosition);
    	myLCD.PrintChar(currentScreen, cursor);

        char * message;
        err = display_error ("Pending on Queue: ",
            message = (char *) OSQPend(&print_q, WAIT_FOREVER, &err)
        );

    	/* Decode message and change screen and position. */
    	changePosition(message, &currentScreen, &currentPosition);

    	OSTimeDly(TICKS_PER_SECOND / 10);
    }
}

/* change cursor position by reference according to a message (either L, R, U, or D) */
void changePosition(char * message, unsigned char *currentScreen, unsigned char *currentPosition) {
	switch (message[0])
	{
	case 'L':
		iprintf("L");
		if (*currentPosition == 0) {
                        /* currently on top left corner of current screen */
			toggleScreen(currentScreen);
			*currentPosition = 79;
		} else {
			(*currentPosition)--;
		}
	break;
	case 'R':
		iprintf("R");
		if (*currentPosition == 79) {
                        /* currently on top right corner of current screen */
			toggleScreen(currentScreen);
			*currentPosition = 0;
		} else {
			(*currentPosition)++;
		}
	break;
	case 'U':
		iprintf("U");
		if (*currentPosition >= 40) {
			/* currently on bottom line of current screen */
			*currentPosition -= 40 ;
		} else {
			/* currently on top line of current screen */
			toggleScreen(currentScreen);
			*currentPosition += 40;
		}
	break;
	case 'D':
		iprintf("D");
		if (*currentPosition >= 40) {
			/* currently on bottom line of current screen */
			toggleScreen(currentScreen);
			*currentPosition -= 40;
		} else {
			/* currently on top line of current screen */
			*currentPosition += 40;
		}
	break;
	}

	iprintf("_%d", *currentPosition);
}

/* swap screen from upper to lower or vice versa */
void toggleScreen(unsigned char *currentScreen) {
	if (*currentScreen == LCD_UPPER_SCR) *currentScreen = LCD_LOWER_SCR;
	else *currentScreen = LCD_UPPER_SCR;
}

/* The INTERRUPT MACRO handles the house keeping for the vector table
 * and interrupt model.
 * Unfortunately the 0x2500 magic number must stay due to the MACRO syntax
 * The 0x2500 refers to the status register value that the microprocessor,
 * will have during the interrupt. 0x2500 to mask levels 1-5
 * Consult the NetBurner Runtime Libraries manual for information on this MACRO
 * Restrictions on what you can and cannot call inside an interrupt service routine
 * are listed there as well */

INTERRUPT(out_irq_pin_isr, 0x2500){
	sim.eport.epfr=0x08; /* Clear the interrupt edge 0 0 0 0 1 0 0 0 */

        /* Error printing omitted, since printing from ISR is ill advised. */
	OSQPost(&print_q, (void *) myKeypad.GetNewButtonString());
}
/* The 8-bit interrupt vector is formed using the following algorithm:
* For Interrupt Controller 0, vector_number = 64 + interrupt source number (3)
*/
#define INTERRUPT_VECTOR (3)
#define INTERRUPT_CONTROLLER_0 (0)
#define ISR_MASK_1 (1)
#define ISR_PRIO_1 (1)

/*
 * You also need to call SetIntC with the correct parameters.
 *
 * void SetIntc(int intc, long func, int vector, int level, int prio)
 * int intc is the interrupt controller number for our interrupt
 * long func is our isr by name
 * int vector is the source vector number for our interrupt (i.e edge port module pin X)
 * this is in a table in the interrupt controller chapter in the ColdFire reference manual
 * int level is the interrupt mask level (1-7). use 1 here
 * int prio is the interrupt service routine priority.  use 1 here as well
 * SetIntC is also documented in the NetBurner Runtime manual.
 *
 * Access the edge port registers by treating them as any variable
 * sim.eport.eppar = blah;
 *
 * The documentation on the edge port module will contain the information
 * on how to signal to the processor that it should return to normal processing.
 */
void IRQIntInit(void) {

	// Set pin for IRQ3 to rising edge triggered.
	sim.eport.eppar = 0x40; /* 00 00 00 00 01 00 00 00 - EPPA3 to 01 see table 11-13 in UM */
	sim.eport.epddr = 0x0; /* All edge port pins as inputs */
	sim.eport.epier = 0x8; /* Enable IRQ3 only 0 0 0 0 1 0 0 0 */

	// Register ISR with interrupt controller.
	SetIntc(INTERRUPT_CONTROLLER_0,
			(long)&out_irq_pin_isr,
			INTERRUPT_VECTOR,
			ISR_MASK_1,
			ISR_PRIO_1);
}
