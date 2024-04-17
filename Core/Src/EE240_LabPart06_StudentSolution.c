/*************
 *  Utilities to build the Assessment side of LabPart06, on Databases 
 ***************/

#include "main.h"
#include "MyMain.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

#include "EE240_Utilities.h"
#include "EE240_LabPart06_Utilities.h"
#include "EE240_LabPart06_StudentSolution.h"



/*********************************
 *  ProcessQueryCommand   Process a Query Command, taking actions as necessary.
 * Inputs:              pQueryControl,   A control structure, with system-wide information
 * 						pQueryCommand,
 * 						pCarsDatabase,
 * 						**ppCurrentWorkingList
 * Outputs:             0 on Success
 * Side Effects:        Depends on command
********************************/
int ProcessQueryCommand( QueryControl_t * pQueryControl, QueryCommand_t * pQueryCommand,  CarListNode_t * pCarsDatabase,  CarListNode_t ** ppCurrentWorkingList) {
    int nCarsSelected            = 0;
    int nCarsOnCurrentWorkingList, nCarsInDatabase;
    int ReturnValue;
    CarListNode_t * pNewCarsList = NULL;
    CarListNode_t * pTailOfCurrentWorkingList;

	switch ( pQueryCommand->eQueryCommand ) {
	case qc_Select :
		//  This case has to handle two possibilities.
		//  1)  There is already a current working list
		//  2)  There is no current working list
		if(*ppCurrentWorkingList == NULL) {
			pNewCarsList = SelectFromCarDatabase(pQueryCommand, pCarsDatabase, &nCarsSelected);
			*ppCurrentWorkingList = pNewCarsList;
		}
		else {
			pNewCarsList = SelectFromCarDatabase(pQueryCommand, *ppCurrentWorkingList, &nCarsSelected);
			FreeCarListNodes(*ppCurrentWorkingList);
			*ppCurrentWorkingList = pNewCarsList;
		}

		//SortCarList(*ppCurrentWorkingList, jVerbosity);
		/*******************
		*  Routine for this case needed
		*******************/

		break;
	case qc_Add :

		  //  Form a new list, based on the query,
		  //  Add it to the existing list (make sure the existing list is not NULL)


		/*******************
		*  Routine needed
		*******************/

		  // A list built this way might have duplcates.   Use the EliminateDuplicatesInCarList() tool

		  // Eliminate any duplicates
		  ReturnValue               = EliminateDuplicatesInCarList( *ppCurrentWorkingList, pQueryControl->jVerbosity );
		  // Sort the final list
		  ReturnValue               = SortCarList( *ppCurrentWorkingList , pQueryControl->jVerbosity );
		  break;

	case qc_Report :
		nCarsOnCurrentWorkingList = CountCarsOnList(*ppCurrentWorkingList);
		fprintf(stdout, "Pretty Printing p Current Working List, %d Cars \n", nCarsOnCurrentWorkingList);
		PrettyPrintListOfCars(stdout, *ppCurrentWorkingList );
        break;
                
	case qc_ReportDatabase :
		nCarsInDatabase = CountCarsOnList(pCarsDatabase);
		fprintf(stdout, "Pretty Printing p Car Database, %d Cars \n", nCarsInDatabase);
		PrettyPrintListOfCars(stdout, pCarsDatabase );
		break;

	case qc_Report_nCarsOnWorkingList :
		nCarsOnCurrentWorkingList = CountCarsOnList( *ppCurrentWorkingList );
		fprintf(stdout, "Current Working List has %d cars \n", nCarsOnCurrentWorkingList);
		break;

	case qc_Reset :
		FreeCarListNodes( *ppCurrentWorkingList );
		*ppCurrentWorkingList = NULL;
		break;

	case  qc_Help :
		PrintHelpMessage( stdout, pQueryControl );
		break;

	default:
		return FAILURE;
	}

	return 0;
}  // ProcessQueryCommand


/*********************************
 * SelectFromCarDatabase     Go down pListOfCars, test each Car to see if it satisfies the requirements
 *                           of the Query.  If it does satisfy, malloc a new CarListNode_t, set it up, and 
 *                              Add it to the list to be returned. 
 *
 * Inputs:              pQueryCommand,     A Query Command structure, indicating which Cars to select
 *                      pListOfCars,       The list of cars to select from
 *                      pnCarsSelected,    Pointer back to an integer,
 *
 * Outputs:             Pointer to new list.  
 * 
 * Side Effects:        CarListNodes are malloced as needed,  Populated and connected into a list. 
********************************/
CarListNode_t * SelectFromCarDatabase( QueryCommand_t * pQueryCommand, CarListNode_t * pListOfCars, int *pnCarsSelected ) {
  CarListNode_t * pSelectedCarsList;

  return pSelectedCarsList;
}  //  SelectFromCarDatabase


/*********************************
 *  SortCarList  Perform a bubble sort on the car list.
 *       Notes on Bubble Sort:
 *             Go down the list, compare two nodes, if they are out of order, swap them.
 *             Repeat until one can go all the way down the list with no swaps
 *
 *             Routine takes advantage of the fact that we can swap car records
 *               in list nodes, to avoid moving the actual list nodes.
 *
 * Inputs:              pCarList
 * Outputs:             0 for success
 * Side Effects:        The list is re-ordered.
********************************/
int SortCarList( CarListNode_t * pCarList, int jVerbosity ) {

  return SUCCESS;
}


/*********************************
 *  FreeCarListNodes   Free the nodes on a List
 * Inputs:              Pointer to a list of cars
 * Outputs:             Count of Car List Nodes Freed
 * Side Effects:        Frees Nodes from the list   (the data records are not affected)
********************************/
int FreeCarListNodes( CarListNode_t * pListOfCars ) {
  int nCarsOnList = 0;

  return nCarsOnList;
}

