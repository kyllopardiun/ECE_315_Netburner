/* Rev:$Revision: 1.1 $ */
/******************************************************************************
 * Copyright 1998-2008 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivitives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non NetBurner Hardware. Please contact Licensing@Netburner.com
 *   for more information.
 *
 *   NetBurner makes no representation or warranties with respect to the
 *   performance of this computer program, and specifically disclaims any
 *   responsibility for any damages, special or consequential, connected
 *   with the use of this program.
 *
 *   NetBurner, Inc
 *   5405 Morehouse Drive
 *   San Diego Ca, 92121
 *   http://www.netburner.com
 *
 *****************************************************************************/

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <taskmon.h>
#include <dhcpclient.h>
#include <string.h>
#include "FormData.h"
#include <eTPU.h>
#include <ETPUInit.h>
#include <eTPU_sm.h>
#include <sim5234.h>
#include <cfinter.h>
#include "motorconstants.h"
#include "images.h"
#define MOTOR_MODE_FULL 0
const char *AppName = "Kirby & Rodrigo";
const char *ok = OK_IMAGE;
char const *fail = FAIL_IMAGE;

extern "C"
{
   void UserMain( void *pd );
   void IRQIntInit(void);
   void SetIntc(int intc, long func, int vector, int level, int prio);
   void DisplayLameCounter( int sock, PCSTR url );
   void outputMaxRPM(int sock, PCSTR url);
   void outputMinRPM(int sock, PCSTR url);
   void outputRotation(int sock, PCSTR url);
   void outputDirection(int sock, PCSTR url);
   void getMotorMode(int sock, PCSTR url);
}

extern void RegisterPost();

#define MAX_COUNTER_BUFFER_LENGTH 100
#ifndef IS_FORWARD
#define IS_FORWARD	getdipsw()==0
#endif
#ifndef WAIT_FOREVER
#define WAIT_FOREVER	0
#endif

FormData myData;
OS_SEM form_sem;

void UserMain( void *pd )
{
	InitializeStack();
	OSChangePrio( MAIN_PRIO );
	EnableAutoUpdate();
	StartHTTP();
	EnableTaskMonitor();
	iprintf("%s",AppName);
	OSSemInit(&form_sem, 1);
	myData.Init(MOTOR_MODE_FULL);
	//Call a registration function for our form code
	// so POST requests are handled properly.
	RegisterPost();



	while ( TRUE )
	{
		OSTimeDly(TICKS_PER_SECOND*100);
	}
}

/* Name: outputMaxRPM
 * Description: Displays the image with an X or a checkmark.
 * Meant to be called using <!-- FUNCTIONCALL outputMaxRPM--> in the
 * index.htm file that contains our forms.
 * Inputs: int sock is a file descriptor for the socket that we are
 * using to communicate with the client.
 * Outputs: none
 */
void outputMaxRPM(int sock, PCSTR url){
	iprintf("\nvalidateMax");
	OSSemPend(&form_sem, WAIT_FOREVER /* Wait forever */);
	if((sock > 0) && (url != NULL)) {
			writestring(sock,(const char *) myData.isMaxValid() ? ok : fail);
	}
	OSSemPost(&form_sem);
}
/* Name: outputMinRPM
 * Description: Displays the image with an X or a checkmark.
 * Meant to be called using <!-- FUNCTIONCALL outputMinRPM--> in the
 * index.htm file that contains our forms.
 * Inputs: int sock is a file descriptor for the socket that we are
 * using to communicate with the client.
 * Outputs: none
 */
void outputMinRPM(int sock, PCSTR url){
	iprintf("min");
	OSSemPend(&form_sem, WAIT_FOREVER /* Wait forever */);
	iprintf("\nvalidateMin");
	if((sock > 0) && (url != NULL)) {
			writestring(sock,(const char *) myData.isMinValid() ? ok : fail);
	}
	OSSemPost(&form_sem);
	}
/* Name: outputRotation
 * Description: Displays the image with an X or a checkmark.
 * Meant to be called using <!-- FUNCTIONCALL outputRotation--> in the
 * index.htm file that contains our forms.
 * Inputs: int sock is a file descriptor for the socket that we are
 * using to communicate with the client.
 * Outputs: none
 */
void outputRotation(int sock, PCSTR url){
	iprintf("\nvalidateRot");
	OSSemPend(&form_sem, WAIT_FOREVER /* Wait forever */);
	if((sock > 0) && (url != NULL)) {
			writestring(sock,(const char *) myData.isRotationValid() ? ok : fail);
	}
	OSSemPost(&form_sem);
	}
/* Name: outputDirection
 * Description: Displays the image with an X or a checkmark.
 * Meant to be called using <!-- FUNCTIONCALL outputDirection--> in the
 * index.htm file that contains our forms.
 * Inputs: int sock is a file descriptor for the socket that we are
 * using to communicate with the client.
 * Outputs: none
 */
void outputDirection(int sock, PCSTR url){
	iprintf("\nvalidateDir");
	OSSemPend(&form_sem, WAIT_FOREVER /* Wait forever */);
	if((sock > 0) && (url != NULL)) {
			writestring(sock,(const char *) myData.isDirValid() ? ok : fail);
	}
	OSSemPost(&form_sem);
}
/* Name: getMotorMode
 * Description: Displays the image with an X or a checkmark.
 * Meant to be called using <!-- FUNCTIONCALL getMotorMode--> in the
 * index.htm file that contains our forms.
 * Inputs: int sock is a file descriptor for the socket that we are
 * using to communicate with the client.
 * Outputs: none
 */
void getMotorMode(int sock, PCSTR url){
	iprintf("\nvalidateMotor");
	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];
	if((sock > 0) && (url != NULL)) {
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH,IS_FORWARD? "FULL STEP" : "HALF STEP");
		writestring(sock,(const char *) buffer);

	}
}








