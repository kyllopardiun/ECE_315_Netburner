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
void Print_Task(void * pd);
void IRQIntInit(void);
void SetIntc(int intc, long func, int vector, int level, int prio);
}


#define PRINT_TASK_PRIO 	(MAIN_PRIO + 1)

const char * AppName="Rodrigo & Kirby";

/* Instantiate your Queue objects here */

OS_SEM print_sem;
DWORD Print_Task_Stack[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

Keypad  myKeypad;
Lcd		myLCD;


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

    IRQIntInit();

    iprintf("Application started: %s\n", AppName);

    myKeypad.Init();
    myLCD.Init(LCD_BOTH_SCR);
    myLCD.PrintString(LCD_UPPER_SCR, "ECE315 Lab #2 Winter 2015");
    OSTimeDly(TICKS_PER_SECOND*1);


    /* Initialize your queue and interrupt here */
    OSSemInit(&print_sem, 0);
    err = OS_NO_ERR;

	err = display_error("Start Print Task: ", OSTaskCreatewName(Print_Task,
			(void *) NULL, // task data - not usually used
			(void *) &Print_Task_Stack[USER_TASK_STK_SIZE], // task stack top
			(void *) &Print_Task_Stack[0], // task stack bottom
			PRINT_TASK_PRIO, "Print Task") // task priority and task name
			);

    while (1) {
    	/* Insert your queue usage stuff */
    	/* You may also choose to do a quick poll of the Data Avail line
    	 * of the encoder to convince yourself that the keypad encoder works.
    	 */

    	OSTimeDly(TICKS_PER_SECOND*100);
    }
}

void Print_Task(void * pd) {
	myLCD.PrintString(LCD_UPPER_SCR, "it works");
	iprintf("worrrrks");
	//while (1) {
		OSSemPend(&print_sem, 0);

		myLCD.PrintString(LCD_UPPER_SCR, "no");
		iprintf("dammmmmit");
	//}
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


	OSSemPost(&print_sem);
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
