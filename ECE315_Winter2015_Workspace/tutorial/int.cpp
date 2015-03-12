/*
 * int.cpp
 *
 *	Created on: May 27th, 2013
 *	Author: 	Nancy Minderman
 *				nancy.minderman@ualberta.ca
 *
 */
#include <stdio.h>
#include <string.h>
#include <startnet.h>
#include <cfinter.h>
#include "sim5234.h"
#include "lcd.h"

#define WAIT_FOREVER			0
#define LEDS_ON 	0xff
#define LEDS_OFF 	0x00
#define INTC0 0
#define SOURCE_VECTOREPORT1 1
#define SOURCE_VECTORGRA 32
#define LEVEL3 3
#define LEVEL4 4
#define PRIORITY1 1
#define MOVE40 40

#define HBERR 0x80000000
#define BABR 0x40000000
#define BABT 0x20000000
#define GRA 0x10000000
#define TXF 0x8000000
#define TXB 0x4000000
#define RXF 0x2000000
#define RXB 0x1000000
#define MII 0x800000
#define EBERR 0x400000
#define LC 0x200000
#define RL 0x100000
#define UN 0x80000

extern "C" {
	// This routine helps you to initialise the interrupt controller properly
	void SetIntc( int intc, long func, int vector, int level, int prio );
	void printStatsonLCD();
	void printMasked(int mask);
}

extern Lcd myLCD;
extern OS_SEM IrqPostSem;

//PrintMasked : prints state of the register on the upper screen of the  LCD
//@param mask[int] boolean value of the register
/* Name:  printMasked
 * Description: print status of the register value on LOWER LCD
 * Inputs: register mask used to get the value from EIMR
 * Outputs: none
 */
void printMasked(int mask) {
	if (sim.fec.eimr & mask) {
			myLCD.PrintString(LCD_UPPER_SCR, "1");
		} else {
			myLCD.PrintString(LCD_UPPER_SCR, "0");
		}
}

/* Name:  QueryIntsFEC
 * Description: Interrupt querying method.
 * Inputs: none
 * Outputs: none
 */
void QueryIntsFEC(void){
	myLCD.Clear(LCD_BOTH_SCR);

	// print EIMR HBERR notification
	myLCD.PrintString(LCD_UPPER_SCR, "HBERR:");
	printMasked(HBERR);

	// print EIMR BABR notification
	myLCD.PrintString(LCD_UPPER_SCR, " BABR:");
	printMasked(BABR);

	// print EIMR BABT notification
	myLCD.PrintString(LCD_UPPER_SCR, " BABT:");
	printMasked(BABT);

	// print EIMR GRA notification
	myLCD.PrintString(LCD_UPPER_SCR, " GRA:");
	printMasked(GRA);

	// print EIMR TXF notification
	myLCD.PrintString(LCD_UPPER_SCR, " TXF:");
	printMasked(TXF);

	// print EIMR TXB notification
	myLCD.PrintString(LCD_UPPER_SCR, " TXB:");
	printMasked(TXB);

	// print EIMR RXF notification
	myLCD.PrintString(LCD_UPPER_SCR, " RXF:");
	printMasked(RXF);

	// print EIMR RXB notification
	myLCD.PrintString(LCD_UPPER_SCR, " RXB:");
	printMasked(RXB);

	// print EIMR MII notification
	myLCD.PrintString(LCD_UPPER_SCR, " MII:");
	printMasked(MII);

	// print EIMR EBERR notification
	myLCD.PrintString(LCD_UPPER_SCR, " EBERR:");
	printMasked(EBERR);

	// print EIMR LC notification
	myLCD.PrintString(LCD_UPPER_SCR, " LC:");
	printMasked(LC);

	// print EIMR RL notification
	myLCD.PrintString(LCD_UPPER_SCR, " RL:");
	printMasked(RL);

	// print EIMR UN notification
	myLCD.PrintString(LCD_UPPER_SCR, " UN:");
	printMasked(UN);

	printStatsonLCD();
}

/* Name:  printStatsonLCD
* Description: Print Network Statistics on the lower screen of the LCD
* Inputs: none
* Outputs: none
*/
void printStatsonLCD(){
	unsigned long unicast = sim.fec_rmon_t.packets -  sim.fec_rmon_t.bc_pkt - sim.fec_rmon_t.mc_pkt;
	char buff[80];
	myLCD.PrintString(LCD_LOWER_SCR,"| Total  | Unicast|Multicast|Broadcast|");
	myLCD.MoveCursor(LCD_LOWER_SCR,MOVE40);
	sprintf(buff,"|%8lu|%8lu|%9lu|%9lu|",sim.fec_rmon_t.packets, unicast, sim.fec_rmon_t.mc_pkt, sim.fec_rmon_t.bc_pkt );
	myLCD.PrintString(LCD_LOWER_SCR, buff);


}

// INTERRUPT ISR MACROS

INTERRUPT(gra_isr, 0x2400) /* GRA Mask */
{
	sim.fec.eir |= 0x10000000;
	putleds(LEDS_ON);
}

INTERRUPT(irq1_isr, 0x2300) /* IRQ 1 Mask */
{
	sim.eport.epfr = 0x02; // 0000 0010 - Clear the interrupt edge
	OSSemPost(&IrqPostSem);
}


/* Name:  InitializeIntsFEC
 * Description: Initialize the GRA interrupt.
 * Inputs: none
 * Outputs: none
 * Addtional details: level = 4 and prio =1
 */

void InitializeIntsFEC(void){
	// enable GRA condition
	sim.fec.eimr |= 0x10000000;

	SetIntc(INTC0, // The intc number
			(long) &gra_isr, // Our interrupt function
			SOURCE_VECTORGRA, // Interrupt source vector
			LEVEL4, // Interrupt level
			PRIORITY1); // Interrupt priority
}
/* Name:  InitializeIntsIRQ1
 * Description: Initialize the IRQ interrupt.
 * Inputs: none
 * Outputs: none
 * Addtional details: level = 3 and prio =1
 */

void InitializeIntsIRQ1(void) {

	sim.eport.eppar = 0x0008; // 0000 0000 0000 1000 - See Table
	// 15-3 in the MCF5235 Ref Man
	sim.eport.epddr = 0x00; // 0000 0000 - All edge port pins as
	// inputs
	sim.eport.epier = 0x02; // 0000 0010 - Enable IRQ1 only
	//
	// Now enable the actual interrupt controller. We are going to use
	// the BSP helper function declared above and defined in
	// \Nburn\MOD5234\system\bsp.c
	//
	SetIntc(INTC0, // The intc number
			(long) &irq1_isr, // Our interrupt function
			SOURCE_VECTOREPORT1, // Interrupt source vector
			LEVEL3, // Interrupt level
			PRIORITY1); // Interrupt priority
}
