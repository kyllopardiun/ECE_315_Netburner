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
#define MAXRPM 200
#define DEFAULTRPM 40
#define MINRPM 1
#define MAXROTATIONS 10000
#define DEFAULTROTATIONS 500
#define MINROTATIONS 1

/* Name: FormData Constructor
 * Description: Empty Constructor for the class
 * Inputs: 	none
 * Outputs: none
 */
FormData::FormData() {
	// Initialization happens by calling the function FormData::Init()

}

/* Name: FormData Destructor
 * Description: Empty Destructor for the class
 * Inputs:	none
 * Outputs: none
 */
FormData::~FormData() {
	// No resources to clean
}


BYTE FormData::SetMaxRPM(char * rpm) {
	int max = atoi(rpm);
	if ((max<=MAXRPM)&&(max>=MINRPM)&&(hasOnlyNumbers(rpm))){
		int_maxrpm = max;
		maxRPM_valid = TRUE;
		return FORM_OK;
	}
	maxRPM_valid = FALSE;
	return FORM_ERROR;
}

int  FormData::GetMaxRPM(void){
	return int_maxrpm;
}

BYTE FormData::SetMinRPM(char * rpm) {
	int min = atoi(rpm);
	if ((min<=MAXRPM)&&(min>=MINRPM)&&(min<=int_maxrpm)&&hasOnlyNumbers(rpm)){
		int_minrpm = min;
		minRPM_valid = TRUE;
		return FORM_OK;
	}
	minRPM_valid = FALSE;
	return FORM_ERROR;
}

int  FormData::GetMinRPM(void) {
	return int_minrpm;
}

BYTE FormData::SetSteps(char * steps) {
	steps_valid = TRUE;
	return FORM_OK;
}

int FormData::GetSteps (void) {
	return int_steps;
}

BYTE FormData::SetRotations(char * rot) {
	int rotations = atoi(rot);
	if ((rotations<=MAXROTATIONS)&&(rotations>=MINROTATIONS)&&hasOnlyNumbers(rot)){
		int_rotations = rotations;
		rotations_valid = TRUE;
		return FORM_OK;
	}
	rotations_valid = FALSE;
	return FORM_ERROR;
}

int  FormData::GetRotations(void){
	return int_rotations;
}

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

BYTE FormData::GetDirection(void){
	return direction;
}

BYTE FormData::GetMode(void){
	return mode;
}
/* Name:  isMaxValid
 * Description: checks if field max RPM is valid
 * Inputs: void
 * Outputs: 1 if valid else 0
 */
char  FormData::isMaxValid(void){
	return maxRPM_valid;
}
/* Name:  isMinValid
 * Description: checks if field min RPM is valid
 * Inputs: void
 * Outputs: 1 if valid else 0
 */
char  FormData::isMinValid(void){
	return minRPM_valid;
}/* Name:  isDirValid
 * Description: checks if field directions is valid
 * Inputs: void
 * Outputs: 1 if valid else 0
 */
char  FormData::isDirValid(void){
	return direction_valid;
}/* Name:  isRotationValid
 * Description: checks if field directions is valid
 * Inputs: void
 * Outputs: 1 if valid else 0
 */
char  FormData::isRotationValid(void){
	return rotations_valid;
}/* Name:  isModeValid
 * Description: checks if field rotation is valid
 * Inputs: void
 * Outputs: 1 if valid else 0
 */
char  FormData::isModeValid(void){
	return motor_mode_valid;
}/* Name:  isStepsValid
 * Description: checks if field motor_mode is valid
 * Inputs: void
 * Outputs: 1 if valid else 0
 */
char  FormData::isStepsValid(void){
	return steps_valid;
}
/* Name:  hasOnlyNumbers
 * Description: checks if the inputs contains only numbers and space
 * Inputs: char*
 * Outputs: 1 if valid else 0
 */
char  FormData::hasOnlyNumbers(char *string){
	int i=0;
	char number_detected=FALSE;
	int trailing=FALSE;
	while(string[i]!='\0'){
		if(!((string[i]>='0' && string[i]<='9')||(string[i]==' ')))
			return FALSE;
		else{
			iprintf("%c",string[i]);
			if (string[i]>='0' && string[i]<='9')
			 number_detected = TRUE;
			if(number_detected && string[i]==' '){
				trailing = TRUE;
			}
			if(trailing && (string[i]>='0' && string[i]<='9')){
				return FALSE;
			}
		}
		++i;
	}
	return TRUE;
}


/* Name: Init
 * Description: initializes the form data
 * Inputs: BYTE Motor Mode
 * Outputs:none
 */
BYTE FormData::Init(BYTE motor_mode){
	direction = CW;
	mode = motor_mode;
	int_maxrpm = DEFAULTRPM;
	int_minrpm = DEFAULTRPM;
	int_steps = 0; // Unused on this lab
	int_rotations = DEFAULTROTATIONS;
	//FLAGS
	direction_valid = TRUE;
	maxRPM_valid = TRUE;
	minRPM_valid = TRUE;
	rotations_valid = TRUE;
	motor_mode_valid = TRUE;
	steps_valid = TRUE;
	return FORM_OK;
}
