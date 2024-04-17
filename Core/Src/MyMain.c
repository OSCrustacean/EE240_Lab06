/*
 * MyRoutine.c
 *
 *  Created on: Jan 10, 2023
 *      Author: bsraX
 */


#include "main.h"
#include "MyMain.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "EE240_Utilities.h"
#include "EE240_LabPart06_Utilities.h"
#include "UART5_StreamIO.h"


// Persistent variables
extern 	TIM_HandleTypeDef htim2;   	// Allows User programs to measure time.
extern UART_HandleTypeDef huart5;   // Comms to Assessment computer



/*********************************
 *  MyMain,   Plays the role of main(),  allowing main(), in the auto-generated main.c
 * 					to do hardware initializations.   Lab Part 05
 * Inputs:              None
 * Outputs:             None
 * Side Effects:
********************************/
int MyMain(void) {
  int 	jVerbosity;
  int 	ReturnValue;
  int  	nCarsInDatabase = 0, nCarsOnCurrentWorkingList = 0;
  char 	sInputString[ N_InputCharacters ];
  FILE *pFileIn;
  char *pFgetsReturnValue;
  bool	bKeepProcessingCommands;

  CarRecord_t     * pCarRecords   = Get_pCarRecords();
  CarListNode_t   * pCarDatabase = NULL;
  CarListNode_t   * pCurrentWorkingList = NULL;
  QueryCommand_t  	AQueryCommand;
  QueryControl_t 	AQueryControl, *pQueryControl = &AQueryControl;


	// State 0
  jVerbosity = 2;
  ClearOutBits();
  ClearNUCLEO_LEDs();
  Initialize_cBufferIn( ' ' );
  Initialize_cBufferOut( ' ' );
  InitCarRecordsAndModels();

  // State 2
  MarkEvenState;
  pCarDatabase 	  = ReceiveAListOfCarRecords( pCarRecords, N_CarRecords, 1, 0, jVerbosity );
  nCarsInDatabase = CountCarsOnList(pCarDatabase);
  InitializeQueryControl( pQueryControl, NULL,  0, nCarsInDatabase, jVerbosity );


  	  // Show the database on console
  if (jVerbosity >= 1) {
    fprintf(stdout, "Car Database:\n");
    PrettyPrintListOfCars(stdout, pCarDatabase);
    fprintf(stdout, " ----  Car Database, %d Cars  ---- \n", nCarsInDatabase);
  }

  // State 4
  MarkEvenState;
  WriteOutBit(1, 0);  // Lower handshake bit, indicating reception complete


  bKeepProcessingCommands = true;
  while ( bKeepProcessingCommands ) {

	  // Wait for State 5,  Assessment Computer ready to send command
	  while ( ReadInBit(0) != 1 )        	// An error occurs if Student computer is booted while the Assesssment computer is mid-test
		  FlashNUCLEO_LED(jNUCLEO_GreenLED);	// So block here until assessment computer reaches state 1
	  WriteNUCLEO_LED(jNUCLEO_GreenLED, 0);
	  MarkOddState;

	  // State 5
	  ReturnValue = Start_UART5_Reception();
	  WriteNUCLEO_LED(jNUCLEO_BlueLED, 1);  // Indicates listening

	  // State 6
	  WriteOutBit(1, 1);  // Raise handshake bit
	  MarkEvenState;


	  // State 6, Assessment Computer transmitting.
	  // Waiting for State 7, Assessment Computer done transmitting
	  while ( ReadInBit(0) != 0 )        	// An error occurs if Student computer is booted while the Assesssment computer is mid-test
		  FlashNUCLEO_LED(jNUCLEO_BlueLED);	// So block here until assessment computer reaches state 1

	  // State 7
	  MarkOddState;
	  ReturnValue       = Stop_UART5_Reception();
	  WriteNUCLEO_LED(jNUCLEO_BlueLED, 0);  // Indicates not listening


	  WriteOutBit(1, 0);  // Lower handshake bit

	  // State 8, Process the command
	  MarkEvenState;

	  pFileIn           = UART5_OpenInStream();
	  pFgetsReturnValue = fgets(sInputString, N_InputCharacters, pFileIn);
	  fclose(pFileIn);
	  Initialize_cBufferIn( ' ' );
	  SuppressCharactersOffEndOfString(sInputString, " \n\r\t", pQueryControl->jVerbosity >= 13);

	  fprintf(stdout, "Command: >%s< \n", sInputString);

	  ReturnValue = ParseQueryCommandLine( pQueryControl, &AQueryCommand,  sInputString );
	  if (pQueryControl->jVerbosity >=  4) {
		  fprintf(stdout, "  MyMain:  ParseQueryCommandLine returned %d Criteria \n", AQueryCommand.nCategoryRanges);
		  PrettyPrintAQueryCommand(stdout, &AQueryCommand);
	  }

	  ProcessQueryCommand( pQueryControl, &AQueryCommand, pCarDatabase, &pCurrentWorkingList ) ;
	  nCarsOnCurrentWorkingList = CountCarsOnList( pCurrentWorkingList );

	  if (pQueryControl->jVerbosity >= 11) fprintf(stdout, "\nMyMain:  SelectFromCarDatabase found %d cars from a list of %d cars.\n",
			  nCarsOnCurrentWorkingList, nCarsInDatabase );

	  if (pQueryControl->jVerbosity >= 2) {
		  PrettyPrintListOfCars(stdout, pCurrentWorkingList);
		  fprintf(stdout, "At end of processing cycle, Current Working List of %d Cars: \n", nCarsOnCurrentWorkingList);

	  }

	  if ( strcasecmp( sInputString, "Report:" ) == 0 ) {

		  // State 81
		  WriteOutBit(3, 1);  // Raise handshake bit
		  MarkOddState;

		  // Wait for State 82 For Ready to Receive
		  while ( ReadInBit(2) != 1 )
				FlashNUCLEO_LED(jNUCLEO_BlueLED);
		  MarkEvenState;

		  Transmit_CarRecords( pCurrentWorkingList, 3, 2, pQueryControl->jVerbosity );

		  	  // In state 83,
		  MarkOddState;
		  WriteNUCLEO_LED(jNUCLEO_BlueLED, 0);  // Done transmitting

			  //Wait for State 84, Db Processed
		  while ( ReadInBit(2) != 0 )
				FlashNUCLEO_LED(jNUCLEO_GreenLED);

	  }

  }

  return  0;
}





