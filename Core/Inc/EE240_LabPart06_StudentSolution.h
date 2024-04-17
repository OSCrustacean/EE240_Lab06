/*
 * EE240_LabPart06.h
 *
 *  Created on: Apr 15, 2023
 *      Author: bsra
 */

#ifndef INC_EE240_LABPART06_StudentSolution_H_
#define INC_EE240_LABPART06_StudentSolution_H_

#include <stdbool.h>

// Function Declarations
int ProcessQueryCommand( QueryControl_t * pQueryControl, QueryCommand_t * pQueryCommand,  CarListNode_t * pCarsDatabase,  CarListNode_t ** ppCurrentWorkingList);
CarListNode_t * SelectFromCarDatabase( QueryCommand_t * pQueryCommand, CarListNode_t * pListOfCars, int *pnCarsSelected );

int SortCarList( CarListNode_t * pCarList, int jVerbosity );
int FreeCarListNodes( CarListNode_t * pListOfCars );

#endif /* INC_EE240_LABPART06_StudentSolution_H_ */
