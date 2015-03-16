/*
 * FormData.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: Nancy Minderman
 */

#include "formdata.h"
#include <stdlib.h>
#include <string.h>
#include <ucos.h>
#include <stdio.h>
#include <ctype.h>
/* Name: FormData Constructor
 * Description: Empty Constructor for the class
 * Inputs: 	none
 * Outputs: none
 */
FormData::FormData() {
	// TODO Auto-generated constructor stub

}

/* Name: FormData Destructor
 * Description: Empty Destructor for the class
 * Inputs:	none
 * Outputs: none
 */
FormData::~FormData() {
	// TODO Auto-generated destructor stub
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetMaxRPM(char * rpm) {
	int max = atoi(rpm);
	if ((max<=200)||(max>0)){
		int_maxrpm = max;
		maxRPM_valid = TRUE;
		return FORM_OK;
	}
	maxRPM_valid = FALSE;
	return FORM_ERROR;
}
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetMaxRPM(void){
	return int_maxrpm;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetMinRPM(char * rpm) {
	int min = atoi(rpm);
	if ((min<=200)||(min>0)){
		int_minrpm = min;
		minRPM_valid = TRUE;
		return FORM_OK;
	}
	minRPM_valid = FALSE;
	return FORM_ERROR;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetMinRPM(void) {
	return int_minrpm;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetSteps(char * steps) {
	steps_valid = TRUE;
	return FORM_OK;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int FormData::GetSteps (void) {
	return int_steps;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */

BYTE FormData::SetRotations(char * rot) {
	int rotations = atoi(rot);
	if ((rotations<=10000)||(rotations>0)){
		int_rotations = rotations;
		rotations_valid = TRUE;
		return FORM_OK;
	}
	rotations_valid = FALSE;
	return FORM_ERROR;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetRotations(void){
	return int_rotations;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetDirection(char * dir){
	if (*dir=='2'){
		direction = CW;
		direction_valid = TRUE;
		return FORM_OK;
	}else if (*dir=='1'){
		direction = CCW;
		direction_valid = TRUE;
		return FORM_OK;
	}else{
		direction_valid = FALSE;
		return FORM_ERROR;
	}
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::GetDirection(void){
	return direction;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::GetMode(void){
	return mode;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::Init(BYTE motor_mode){
	direction = CW;
	mode = motor_mode;
	int_maxrpm = 0;
	int_minrpm = 0;
	int_steps = 0;
	int_rotations = 0;
	//FLAGS
	direction_valid = FALSE;
	maxRPM_valid = FALSE;
	minRPM_valid = FALSE;
	rotations_valid = FALSE;
	motor_mode_valid = FALSE;
	steps_valid = FALSE;
	return FORM_OK;
}
