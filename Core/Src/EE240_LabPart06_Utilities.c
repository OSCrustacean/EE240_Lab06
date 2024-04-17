/*************
 *   EE240_LabPart06_Utilities.c
 *  Utilities to build the Assessment side of LabPart06, on Databases 
 ***************/

#include "main.h"
#include "MyMain.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "ctype.h"
//#include "stdbool.h"
//#include "stm32f4xx.h"
#include "EE240_Utilities.h" 
#include "EE240_Context.h"
#include "UART5_StreamIO.h"

#include "EE240_LabPart06_Utilities.h"

#ifdef B_IS_ASSESSMENT
#include "EE240_LiquidCrystal_I2C.h"
#endif

//#define  PossibleRigorousHalt  while (1) ;
#define PossibleRigorousHalt {}


// Persistent variables 
extern 	TIM_HandleTypeDef  htim2;   	// Allows User programs to measure time.
extern  UART_HandleTypeDef huart5;		// Assessment <-> Student computer communication


// Persistent storage for Model Records and Car Records
// Note, to use of "model":  1)  A ModelRecord is something like Ford Mustang EngineSizes, MinMSRP, ...
//           2)  A string in Make and Model

CarRecord_t   CarRecords  [ N_CarRecords   ];
ModelRecord_t ModelRecords[ N_ModelRecords ];



// Template
/*********************************
 *
 * Inputs:              None
 * Outputs:             None
 * Side Effects:
********************************/


/*********************************
 *  InitCarRecordsAndModels
 * Inputs:              none
 * Outputs:             none
 * Side Effects:        Zero the data storage space for the CarRecords and Models
********************************/
void InitCarRecordsAndModels( void ) {
	memset( &CarRecords[0], 0, sizeof( CarRecords) );
	memset( &ModelRecords[0], 0, sizeof( ModelRecords) );
}


/*********************************
 * Get_pModelRecords
 * Inputs:              none
 * Outputs:             Return a pointer to the Car Models
 * Side Effects:        none
********************************/
ModelRecord_t * Get_pModelRecords( void ){
	return ModelRecords;
}


/*********************************
 *  Get_pCarRecords
 * Inputs:              none
 * Outputs: 			Return a pointer to the Car Records
 * Side Effects:        none
********************************/
CarRecord_t * Get_pCarRecords( void ){
	return CarRecords;
}


/*********************************
 *  PrintStringsFromAList,  Prints Strings on a line
 * Inputs:              none
 * Outputs:             none
 * Side Effects:        none
 * *******************/
void PrintStringsFromAList( FILE * pFile, char * sStrings[], int nStrings ) {
	int jString;
	for (jString=0; jString < nStrings; jString++) {
		fprintf(pFile, "%-13s, ", sStrings[ jString ]);
	}
}

/*********************************
 *  PrintHelpMessage,  Print a message about input options
 * Inputs:              none
 * Outputs:             none
 * Side Effects:        none
********************************/
void PrintHelpMessage( FILE * pFile,  QueryControl_t * pQueryControl ) {
	fprintf(pFile, "\n=================================================\n");
	fprintf(pFile, "Lab Part 06 Help Message, the Input Options are: \n ---------\n");

	fprintf(pFile, "       Commands:  ");
	PrintStringsFromAList(pFile, pQueryControl->sQueryCommands, pQueryControl->nQueryCommands);
	fprintf(pFile, "\n");

	fprintf(pFile, "Category labels:  ");
	PrintStringsFromAList(pFile, pQueryControl->sCategoryLabels, pQueryControl->nCategoryLabels);
	fprintf(pFile, "\n");

	fprintf(pFile, "     Make names:  ");
	PrintStringsFromAList(pFile, pQueryControl->sMakeNames, pQueryControl->nMakeNames);
	fprintf(pFile, "\n");

	fprintf(pFile, "    Model names:  ");
	PrintStringsFromAList(pFile, pQueryControl->sModelNames, pQueryControl->nModelNames);
	fprintf(pFile, "\n");
	fprintf(pFile, "=================================================\n\n");

}  // PrintHelpMessage


/*********************************
 *  CharacterToPrintableString   Returns something to print for characters
 *  								translates \n to "\n", etc.
 * Inputs:              none
 * Outputs:             none
 * Side Effects:        none
********************************/
int CharacterToPrintableString(char AChar, char sString[]) {

	// In the interest of speed
	if ( isprint( AChar) ) {
	  sprintf(sString, "%c", AChar);
	  return 0;
	}

	if (AChar == '\a')
		sString = "\a";
	else if (AChar == '\b')
		sString = "\b";
	else if (AChar == '\t')
		sString = "\t";
	else if (AChar == '\n')
		sString = "\n";
	else if (AChar == '\v')
		sString = "\v";
	else if (AChar == '\f')
		sString = "\f";
	else if (AChar == '\r')
		sString = "\r";
	else if (AChar == '\e')
		sString = "\e";
	else sprintf(sString, "%2x", AChar);
	return 0;
}



/*********************************
 *  SuppressCharactersOffEndOfString,   remove indicated characters from the end of a string
 * Inputs:              Starting String,   List of Characters to remove
 * Outputs:             Count of characters removed
 * Side Effects:        May set final characters to \0
********************************/
int SuppressCharactersOffEndOfString( char sString[], char sCharactersToSuppress[], bool bVerboseReporting ) {
	int    nStringCharacters, nCharactersToSuppress;
	int    jCharacterToSuppress;
	bool   bKeepGoing, bHaveSuppressedACharacter;
	int	   nCharactersSuppressed = 0;
	char   sStringToPrint[6];

	nCharactersToSuppress = strlen( sCharactersToSuppress );

	bKeepGoing = true;
	while ( bKeepGoing ) {
		nStringCharacters     = strlen( sString );
		bHaveSuppressedACharacter = false;
		for (jCharacterToSuppress = 0; jCharacterToSuppress < nCharactersToSuppress; jCharacterToSuppress++) {
			if ( sString[ nStringCharacters - 1] == sCharactersToSuppress[ jCharacterToSuppress ]) {
				sString[ nStringCharacters - 1] = '\0';
				bHaveSuppressedACharacter = true;
				if ( bVerboseReporting ) {
					CharacterToPrintableString( sCharactersToSuppress[ jCharacterToSuppress ], sStringToPrint);
					fprintf(stdout, "  SuppressCharactersOffEndOfString: Suppressed '%s' off %s \n", sStringToPrint, sString);
				}
			}
		}  //  for
		if ( bHaveSuppressedACharacter == false )
			bKeepGoing = false;  //  Stop
	}

	return nCharactersSuppressed;
}  // SuppressCharactersOffEndOfString


/*********************************
 *  GobbleCharactersFromBeginningOfStream,   remove indicated characters from the beginning of a steam
 * Inputs:              Starting String,   List of Characters to remove
 * Outputs:             Count of characters removed
 * Side Effects:        May set final characters to \0
********************************/
int GobbleCharactersFromBeginningOfStream( FILE *pStream, char sCharactersToGobble[], bool bVerboseReporting ) {
	int    nCharactersToGobble;
	int    jCharacterToGobble;
	bool   bKeepGoing, bHaveGobbleedACharacter;
	int	   nCharactersGobbleed = 0;
	int	   ACharacter;

	nCharactersToGobble = strlen( sCharactersToGobble );

	bKeepGoing = true;
	while ( bKeepGoing ) {
		bHaveGobbleedACharacter = false;
		ACharacter = getc( pStream );

		if ( bVerboseReporting ) {
			fprintf(stdout, "    GobbleCharactersFromBeginningOfStream:  getc AChar got: x%02x", ACharacter);
			if ( isprint(ACharacter) )  //  If character is printable, show it
				fprintf(stdout, " >%c< ", ACharacter);
			fprintf(stdout, "\n");
		}

		for (jCharacterToGobble = 0; jCharacterToGobble < nCharactersToGobble; jCharacterToGobble++) {
			if ( ACharacter == sCharactersToGobble[jCharacterToGobble] ) {
				//  We'll gobble
				bHaveGobbleedACharacter = true;
				break; // Don't need to examine other characters
			}  //  if
		}      //  for

		if ( bHaveGobbleedACharacter == false )  {
			ungetc( ACharacter, pStream );   //  Put character back
			bKeepGoing = false;  			 //  Stop
			if ( bVerboseReporting ) {
				fprintf(stdout, "    GobbleCharactersFromBeginningOfStream:  ungetc: x%02x ", ACharacter);
				if ( isprint(ACharacter) )  //  If character is printable, show it
					fprintf(stdout, " >%c< ", ACharacter);
				fprintf(stdout, "\n");
			}
		} else {
			nCharactersGobbleed++;
		}
	}   //  while

	return nCharactersGobbleed;
}




/*********************************
 *  ParseStringPopulateCarRecord   Parse a string with a specific layout, populate a car record
 *    Based on processing a line that looks like
 *  Toyota     Camry  ,   Year: 2016,  Engine: 2.5L,  Milage: 131318,  Asking Price:   7700,  Status: Used, Index: 4
 *     The routine requires correct spelling of all non-name strings, such as Engine, or Used.
 *
 * Inputs:              String, pointer to car record
 * Outputs:             0 for Success
 * Side Effects:        Populates the car record
********************************/
int ParseStringPopulateCarRecord( CarRecord_t *pCarRecord, char *sInputString, int jVerbosity) {
	char    sMakeModelName[ N_CharactersMakeModelName ];
	char    sFieldName[ N_InputCharacters ];
	char    sFieldName2[ N_InputCharacters ];
	char    sFieldValue[ N_InputCharacters ];
	FILE  * pMemFile;
	int     nCharacters;
	int 	nCharactersToMalloc;
	float   FieldValue;
	int     jFieldValue;
	int 	jField;
	int     nFields = 6;  //  There are 6 fields in a car record, including the index
	int		ReturnValue;

	if (jVerbosity >= 10) fprintf(stdout, "ParseStringPopulateCarRecord: Entering\n%s\n", sInputString);

	nCharacters = strlen( sInputString );
	pMemFile    = fmemopen( sInputString, nCharacters, "r" );

	// Scan, allocate space for, and copy the Car Make
	ReturnValue = fscanf( pMemFile, "%s", sMakeModelName);
	// If we got an empty line, it marks that we are done.
	if (ReturnValue == EOF) {
		fclose( pMemFile );
		return EOF;
	}
	nCharactersToMalloc = strlen(sMakeModelName) + 1;
	pCarRecord->sMake = malloc( nCharactersToMalloc );
	strcpy( pCarRecord->sMake, sMakeModelName );

	// Scan, allocate space for, and copy the Car Model
	fscanf( pMemFile, "%s", sMakeModelName);
	// Deal with possible ','
	SuppressCharactersOffEndOfString(sMakeModelName,  ", ", jVerbosity>=jVerbosity_For_SuppressCharacters);

	nCharactersToMalloc = strlen(sMakeModelName) + 1;
	pCarRecord->sModel = malloc( nCharactersToMalloc );
	strcpy( pCarRecord->sModel, sMakeModelName );

	// Now process fields
	for (jField=0; jField < nFields; jField++){
		// Optionally gobble white space and commas.
		GobbleCharactersFromBeginningOfStream(pMemFile, "\t\r\n ,", jVerbosity>=jVerbosity_For_GobbleCharacters);

		FieldValue = 0.0; jFieldValue = 0; strcpy(sFieldValue, "");
		if (jField == 1) {
		  ReturnValue = fscanf(pMemFile, "%s %fL",  sFieldName, &FieldValue);
		  if (ReturnValue != 2) while (1) ;
		} else if (jField == 3) {
			  ReturnValue = fscanf(pMemFile, "%s %s %d",  sFieldName2, sFieldName, &jFieldValue);
			  if (ReturnValue != 3) while (1) ;
		} else if (jField == 4) {
			  ReturnValue = fscanf(pMemFile, "%s %s",  sFieldName, sFieldValue);
			  if (ReturnValue != 2) while (1) ;
			  SuppressCharactersOffEndOfString(sFieldValue,  ",", jVerbosity>=jVerbosity_For_SuppressCharacters);
		} else {
			ReturnValue = fscanf(pMemFile, "%s %d",  sFieldName, &jFieldValue);
			if (ReturnValue != 2) while (1) ;
		}

		SuppressCharactersOffEndOfString(sFieldName,  ":", jVerbosity>=jVerbosity_For_SuppressCharacters);
		if (jVerbosity >= 15)
			printf("Got: %12s %8.1f or %8d or >%s< \n", sFieldName, FieldValue, jFieldValue, sFieldValue);

		if ( strcasecmp(sFieldName, "Year") == 0 )
			pCarRecord->jYear = jFieldValue;
		else if ( strcasecmp(sFieldName, "Engine") == 0 )
			pCarRecord->EngineSize = FieldValue;
		else if ( strcasecmp(sFieldName, "Milage") == 0 )
			pCarRecord->jMilage = jFieldValue;
		else if ( strcasecmp(sFieldName, "Price") == 0 )
			pCarRecord->jAskingPrice = jFieldValue;
		else if ( strcasecmp(sFieldName, "Status") == 0 )
			pCarRecord->bCarIsNew = strcasecmp( sFieldValue, "New") == 0;
		else if ( strcasecmp(sFieldName, "Index") == 0 )
			pCarRecord->jRecordIndex = jFieldValue;

	}  //  for jField

	fclose( pMemFile );

return SUCCESS;
}  //  ParseStringPopulateCarRecord




/*********************************
 *  FindStringOnList
 *
 * Inputs:              none
 * Outputs:             none
 * Side Effects:        none
********************************/
char *FindStringOnList(char * sString, char *ListOfStrings[], int nStringsOnList, int * pjStringOnList ) {
  int bNoMatch;

  for (*pjStringOnList=0; *pjStringOnList < nStringsOnList; (*pjStringOnList)++) {
	  bNoMatch = strcasecmp( sString, ListOfStrings[*pjStringOnList] );
	  if ( bNoMatch == 0) {
		  //  Match !!
		  return ListOfStrings[ *pjStringOnList ];
	  }
  }
  *pjStringOnList = -1;  // Also indicate no match found
  return NULL;  		 // No match found
}

/*********************************
 *  DetermineOrderOfTwoCarRecords   Evaluate the rank of two cars
 *                                  The ranking rules are:   New, then used
 *                                    Make, Alphabetically
 *                                    Model, Alphabetically
 *                                    EngineSize,  Largest to Smallest
 *                                    Year,  Newest to Oldest
 *                                    Mileage, 	   Least to Most
 *                                    Price,       Most to Least

 * Inputs:              Two pointers to car records
 * Outputs:             -1 if  *pCarRecord1 < *pCarRecord2,  >0 if *pCarRecord1 > *pCarRecord2,   0 if *pCarRecord1 == *pCarRecord2,
 *                      (swap if 1)
 * Side Effects:        none
********************************/
int DetermineOrderOfTwoCarRecords( CarRecord_t * pCarRecord1,  CarRecord_t * pCarRecord2 ) {
  int   jCompare;
  float  Compare;

	// New / Used
	if ( ! pCarRecord1->bCarIsNew && pCarRecord2->bCarIsNew )  // 2 comes before 1
		return 1;

	if ( pCarRecord1->bCarIsNew && ! pCarRecord2->bCarIsNew )  // 2 comes before 1
		return -1;

	// Make, Alphabetical
	jCompare = strcasecmp( pCarRecord1->sMake, pCarRecord2->sMake );
	if ( jCompare != 0 )
		return jCompare;

	// Model, Alphabetical
	jCompare = strcasecmp( pCarRecord1->sModel, pCarRecord2->sModel );
	if ( jCompare != 0 )
		return jCompare;

	jCompare =  pCarRecord1->jYear - pCarRecord2->jYear;
	if ( jCompare > 0)
		return -1;
	if ( jCompare < 0)
		return 1;

	Compare =  pCarRecord1->EngineSize - pCarRecord2->EngineSize;
	if ( Compare > 0)
		return -1;
	if ( Compare < 0)
		return 1;


	jCompare = pCarRecord1->jMilage - pCarRecord2->jMilage;
	if ( jCompare > 0)
		return 1;
	if ( jCompare < 0)
		return -1;

	jCompare = pCarRecord1->jAskingPrice < pCarRecord2->jAskingPrice;
	if ( jCompare > 0)
		return -1;
	if ( jCompare < 0)
		return 1;

	return 0;
} //  DetermineOrderOfTwoCarRecords





/*********************************
 *  EliminateDuplicatesInCarList    Look for identical record pointers to detect duplicates
 * Inputs:              pCarList, jVerbosity
 * Outputs:             Count of nodes eliminated, or -1 for Failure
 * Side Effects:        Detects and frees duplicate nodes
********************************/
int EliminateDuplicatesInCarList( CarListNode_t * pCarList, int jVerbosity ) {
	int				nDuplicateRecordsFound = 0;
	CarListNode_t * pCarList2;
	CarListNode_t * pCarList2_Previous;

	if ( pCarList == NULL ) {
	  if (jVerbosity >= 8)  fprintf(stdout, "EliminateDuplicatesInCarList: Processing an empty list.\n");
	  return nDuplicateRecordsFound;
	}

	while ( pCarList->pNextCarListNode != NULL ) {
		pCarList2 = pCarList->pNextCarListNode;  // Initial value, can not be NULL
		pCarList2_Previous = pCarList;
		while (pCarList2->pNextCarListNode != NULL) {
			if ( pCarList->pCarRecord == pCarList2->pCarRecord ) {
				nDuplicateRecordsFound++;
				if (jVerbosity >= 11)  fprintf(stdout, "EliminateDuplicatesInCarList:  Detected a duplicate, %3d and %3d \n",
														pCarList->pCarRecord->jRecordIndex, pCarList2->pCarRecord->jRecordIndex);
				//Cut out pCarList2
				pCarList2_Previous->pNextCarListNode = pCarList2->pNextCarListNode;
				free( pCarList2 );
			}  //  if Records are duplicate
			pCarList2_Previous = pCarList2;
			pCarList2 = pCarList2->pNextCarListNode;
		}  // Inner while loop
		pCarList = pCarList->pNextCarListNode;
	}  //  Outer while
	return nDuplicateRecordsFound;
}  // EliminateDuplicatesInCarList


/*********************************
 * CompareTwoListsOfCars   Compares two lists of cars.   Cars must have the same order.
 *
 * Inputs:              pFirstCarList, pSecondCarList
 * Outputs:				0 for match, -1 for list lengths are different,  Count of Position in list of first Car to not match, if a non-match
 * Side Effects:		None
********************************/
int CompareTwoListsOfCars( CarListNode_t *pFirstCarList, CarListNode_t *pSecondCarList, int jVerbosity) {
	int  nCarsOnFirstList, nCarsOnSecondList;
	int  jCarOnList;
	int	 ReturnValue;

	nCarsOnFirstList  = CountCarsOnList(pFirstCarList);
	nCarsOnSecondList = CountCarsOnList(pSecondCarList);

	if ( nCarsOnFirstList != nCarsOnSecondList ) {
		if (jVerbosity >= 11) fprintf(stdout, "  CompareTwoListsOfCars: Got lists lengths of %d, %d \n", nCarsOnFirstList, nCarsOnSecondList);
		return -1;
	}

	jCarOnList = 1;
	while ( pFirstCarList != NULL ) {
	  ReturnValue = DetermineOrderOfTwoCarRecords( pFirstCarList->pCarRecord, pSecondCarList->pCarRecord );
	  if ( ReturnValue != 0) {
		  if (jVerbosity >= 11) fprintf(stdout, "  CompareTwoListsOfCars: Got different cars, position %d \n", jCarOnList);
		  if (jVerbosity >= 13) {
			  PrettyPrintACar(stdout, pFirstCarList->pCarRecord);
			  PrettyPrintACar(stdout, pSecondCarList->pCarRecord);
		  }

		  return jCarOnList;
	  }  //   if ( ReturnValue != 0)
	  pFirstCarList  = pFirstCarList->pNextCarListNode;
	  pSecondCarList = pSecondCarList->pNextCarListNode;
	  jCarOnList++;
	}  //  while
	return 0;
 }  //  CompareTwoListsOfCars



/*********************************
 * InitializeQueryControl
 *
 * Inputs:              none
 * Outputs:             none
 * Side Effects:        none
********************************/
int InitializeQueryControl( QueryControl_t *pQueryControl, ModelRecord_t *pModelRecords, int nModelRecords, int nCarRecords, int jVerbosity ) {
  int  jModelRecord;
  int  jMakeOrModelString;
  char * sReturnValue;

  // Zero out the query control
  memset(pQueryControl, 0, sizeof(QueryControl_t));

  pQueryControl->nModelRecords = nModelRecords;
  pQueryControl->nCarRecords   = nCarRecords;

  // Setup String spaces
  for (jMakeOrModelString=0; jMakeOrModelString < N_MakesOrModels; jMakeOrModelString++) {
	  pQueryControl->sMakeNames[jMakeOrModelString]  = &pQueryControl->StringSpaceForMakeAndModelNames[jMakeOrModelString*2*N_CharactersMakeModelName];
	  pQueryControl->sModelNames[jMakeOrModelString] = &pQueryControl->StringSpaceForMakeAndModelNames[jMakeOrModelString*2*N_CharactersMakeModelName + N_CharactersMakeModelName];
  }


  // Setup Query Commands
  pQueryControl->sQueryCommands[0] = "Select";
  pQueryControl->sQueryCommands[1] = "Add";
  pQueryControl->sQueryCommands[2] = "Report";
  pQueryControl->sQueryCommands[3] = "Report_Database";
  pQueryControl->sQueryCommands[4] = "Report_nCarsOnWorkingList";
  pQueryControl->sQueryCommands[5] = "Reset";
  pQueryControl->sQueryCommands[6] = "Help";
  pQueryControl->nQueryCommands    = 7;
  if (pQueryControl->nQueryCommands > N_QueryCommands)  while (1) ;  //  error

  // Setup Category Labels  The indices in this list must match the enum assignments in SetCategoryLabel
  pQueryControl->sCategoryLabels[0] = "Year";
  pQueryControl->sCategoryLabels[1] = "Price";
  pQueryControl->sCategoryLabels[2] = "EngineSize";
  pQueryControl->sCategoryLabels[3] = "Milage";
  pQueryControl->sCategoryLabels[4] = "Status";
  pQueryControl->nCategoryLabels    = 5;
  if (pQueryControl->nCategoryLabels > N_CategoryLabels) while (1) ;  //  error

  pQueryControl->jVerbosity         = jVerbosity;

  // Build a list of makes
  for (jModelRecord=0; jModelRecord < nModelRecords; jModelRecord++) {
	sReturnValue = FindStringOnList( pModelRecords[jModelRecord].sMake, pQueryControl->sMakeNames, pQueryControl->nMakeNames,  &jMakeOrModelString ) ;
	if ( sReturnValue == NULL ) {
		// Make was not in the list of MakeNames, so add it
		if ( pQueryControl->nMakeNames  >= N_MakesOrModels ) while (1);  // error
		strcpy( pQueryControl->sMakeNames[pQueryControl->nMakeNames++], pModelRecords[jModelRecord].sMake );
	}
  }  //  for

  // Build a list of models
  for (jModelRecord=0; jModelRecord < nModelRecords; jModelRecord++) {
	sReturnValue = FindStringOnList( pModelRecords[jModelRecord].sModel, pQueryControl->sModelNames, pQueryControl->nModelNames, &jMakeOrModelString ) ;
	if ( sReturnValue == NULL ) {
		// Model was not in the list of ModelNames, so add it
		if ( pQueryControl->nModelNames  >= N_MakesOrModels) while (1);  // error
		strcpy( pQueryControl->sModelNames[pQueryControl->nModelNames++], pModelRecords[jModelRecord].sModel );
	}
  }  //  for


  return SUCCESS;
}  //  InitializeQueryControl


/*********************************
 *  SetQueryCommand  Match sString to a query command, and set the category label enum in pCategoryRange
 * Inputs:              pQueryControl, pCategoryRange, sString
 * Outputs:             0 for SUCCESS, else Failure
 * Side Effects:        On success, Sets Category label in Category Range
********************************/
int SetQueryCommand( QueryControl_t *pQueryControl, QueryCommand_t * pQueryCommand, char * sString ) {
  int 		jQueryCommand;
  char 	 * sReturnValue;

	// Figure out which Category Label
  sReturnValue = FindStringOnList( sString, pQueryControl->sQueryCommands, pQueryControl->nQueryCommands,  &jQueryCommand ) ;
  if (sReturnValue != NULL ) {
	  switch (jQueryCommand) {
	  case 0 :
		  pQueryCommand->eQueryCommand = qc_Select;
		  break;
	  case 1 :
		  pQueryCommand->eQueryCommand = qc_Add;
		  break;
	  case 2 :
		  pQueryCommand->eQueryCommand = qc_Report;
		  break;
	  case 3 :
		  pQueryCommand->eQueryCommand = qc_ReportDatabase;
		  break;
	  case 4 :
		  pQueryCommand->eQueryCommand = qc_Report_nCarsOnWorkingList;
		  break;
	  case 5 :
		  pQueryCommand->eQueryCommand = qc_Reset;
		  break;
	  case 6 :
		  pQueryCommand->eQueryCommand = qc_Help;
		  break;
	  default:
		  pQueryCommand->eQueryCommand = qc_QueryCommandNotSet;
		  return FAILURE;
	  }  // switch
  }  else  {//  if
	pQueryCommand->eQueryCommand = qc_QueryCommandNotSet;
	if (pQueryControl->jVerbosity >= 1) fprintf(stderr, "SetQueryCommand:  Processed: >%s<, could not find a valid query command \n", sString);
	return FAILURE;
  }
  return SUCCESS;
}  //  SetQueryCommand




/*********************************
 *  GetQueryCommandName
 * Inputs:              eQueryCommand
 * Outputs:             none
 * Side Effects:        Return a pointer to a string with Query Command name.
********************************/
char * GetQueryCommandName( PossibleQueryCommands_t eQueryCommand ) {
	switch ( eQueryCommand ) {
	case qc_QueryCommandNotSet 			: return "Query Command not set";
	case qc_Select             			: return "Select";
	case qc_Add             			: return "Add";
	case qc_Report             			: return "Report";
	case qc_ReportDatabase             	: return "ReportDatabase";
	case qc_Report_nCarsOnWorkingList  	: return "Report nCars On Working List";
	case qc_Reset             			: return "Reset";
	case qc_Help             			: return "Help";
	}
	return "";
}  //  GetCategoryLabel


/*********************************
 *  SetCategoryLabel  Match sString to a category label, and set the category label enum in pCategoryRange
 * Inputs:              pQueryControl, pCategoryRange, sString
 * Outputs:             0 for SUCCESS, else Failure
 * Side Effects:        On success, Sets Category label in Category Range
********************************/
int SetCategoryLabel( QueryControl_t *pQueryControl, CategoryRange_t * pCategoryRange, char * sString ) {
  int 		jCategoryLabel;
  char 	 * sReturnValue;

	// Figure out which Category Label
  sReturnValue = FindStringOnList( sString, pQueryControl->sCategoryLabels, pQueryControl->nCategoryLabels,  &jCategoryLabel ) ;
  if (sReturnValue != NULL ) {
	  switch (jCategoryLabel) {
	  case 0 :
		  pCategoryRange->eCategoryLabel = cl_Year;
		  break;
	  case 1 :
		  pCategoryRange->eCategoryLabel = cl_Price;
		  break;
	  case 2 :
		  pCategoryRange->eCategoryLabel = cl_EngineSize;
		  break;
	  case 3 :
		  pCategoryRange->eCategoryLabel = cl_Milage;
		  break;
	  case 4 :
		  pCategoryRange->eCategoryLabel = cl_Status;
		  break;
	  default:
		  pCategoryRange->eCategoryLabel = cl_CategoryLabelNotSet;
		  return FAILURE;
	  }  // switch
  }  else  {//  if
	pCategoryRange->eCategoryLabel = cl_CategoryLabelNotSet;
	return FAILURE;
  }
  return SUCCESS;
}  //  SetCategoryLabel




/*********************************
 *  GetCategoryLabel
 * Inputs:              eCategoryLabel
 * Outputs:             none
 * Side Effects:        Return a pointer to a string with Category label name.
********************************/
char * GetCategoryLabel( CategoryLabels_t eCategoryLabel ) {
	switch ( eCategoryLabel ) {
	case cl_CategoryLabelNotSet : return "Category not set";
	case cl_Year   		: return "Year";
	case cl_Price  		: return "Price";
	case cl_Milage 		: return "Milage";
	case cl_EngineSize	: return "Engine Size";
	case cl_Status 		: return "Status";
	}
	return "";
}  //  GetCategoryLabel






/*********************************
 *  SetRelationalOperator
 * Inputs:              pQueryControl, pCategoryRange, sString
 * Outputs:             0 for SUCCESS, else Failure
 * Side Effects:        On success, Sets Category label in Category Range
********************************/
int SetRelationalOperator( QueryControl_t *pQueryControl, CategoryRange_t * pCategoryRange, char * sString ) {

  if ( strcmp(sString, "<") == 0 ) {
	  pCategoryRange->eRelation = re_LessThan;
  } else
	  if ( strcmp(sString, "<=") == 0 ) {
		  pCategoryRange->eRelation = re_LessThanOrEqualTo;
	  } else
		  if( strcmp(sString, "=") == 0 ) {
			  pCategoryRange->eRelation = re_EqualTo;
		  } else
			  if( strcmp(sString, ">=") == 0 ) {
				  pCategoryRange->eRelation = re_GreaterThanOrEqualTo;
			  } else
				  if( strcmp(sString, ">") == 0 ) {
					  pCategoryRange->eRelation = re_GreaterThan;
				  } else {

					return FAILURE;
				  }
  return SUCCESS;
}


char * GetRelationOperator( Relations_t eRelation ) {
	switch ( eRelation )  {
	case re_RelationNotSet       : return "Relation not set";
	case re_LessThan             : return "<";
	case re_LessThanOrEqualTo    : return "<=";
	case re_EqualTo              : return "=";
	case re_GreaterThanOrEqualTo : return ">=";
	case re_GreaterThan          : return ">";
	}
	return "";
}


/*********************************
 *  ParseQueryCommandLine
 *
 * Inputs:              pQueryControl, pQueryCommand, sInputString
 * Outputs:             0 for Success, -1 for no command found
 * Side Effects:        Processes string, loads QueryCommand
********************************/
int ParseQueryCommandLine(  QueryControl_t *pQueryControl, QueryCommand_t * pQueryCommand, char sInputString[] ) {
	int		ReturnValue;
	FILE  * pMemFile;
	char    sString[N_InputCharacters];
	char 	sQueryCommand[N_CharactersQueryCommand];
	int     jColon;
	int 	nChars_sModel;
	bool    bGotIllFormedString = false;

	if (pQueryControl->jVerbosity > 5) fprintf(stdout, "  ParseLine: string: >%s< \n", sInputString);

	// Clear the Query record
	memset(pQueryCommand, 0, sizeof(QueryCommand_t));

	pMemFile = fmemopen(sInputString, strlen(sInputString), "r");

	GobbleWhiteSpace(pMemFile, pQueryControl->jVerbosity >= jVerbosity_For_GobbleCharacters);  // Gobble any leading white space

	ReturnValue = fscanf(pMemFile, "%s:", sQueryCommand);
	if (ReturnValue != 1) {
		// In this case we did not get any command string, probably a missing :
	  if (pQueryControl->jVerbosity >= 1) fprintf(stdout, "  ParseLine: Did not get a query command string, possibly missing : \n");
	  fclose( pMemFile );
	  return FAILURE;
	}

	jColon      = strlen( sQueryCommand ) - 1;
	if ( sQueryCommand[jColon] != ':' ) {
	  if (pQueryControl->jVerbosity >= 1) fprintf(stdout, "  ParseLine: query command string did not have a : at end (should be impossible)\n");
	  fclose( pMemFile );
	  return -2;
	}

	sQueryCommand[ jColon ] = '\0';  //  Replace the final character (a :) with the null termination.
	ReturnValue = SetQueryCommand( pQueryControl, pQueryCommand, sQueryCommand );
	if ( ReturnValue != 0)	return ReturnValue;

	if ( pQueryCommand->eQueryCommand == qc_Select || pQueryCommand->eQueryCommand == qc_Add ) {

		// Process a query string
		// First comes the make and model
		ReturnValue = fscanf( pMemFile, "%s %s", pQueryCommand->sMake, pQueryCommand->sModel);
		if (ReturnValue != 2) {
		  if (pQueryControl->jVerbosity >= 1) fprintf(stdout, "  ParseLine: Did not get  Make and Model strings.\n");
		  bGotIllFormedString = true;
		  fclose( pMemFile );
		  PossibleRigorousHalt;
		  return -3;
		}

		nChars_sModel = strlen( pQueryCommand->sModel );
		// Remove possible comma from sModel, and put it back into pMemFile stream.
		if ( pQueryCommand->sModel[ nChars_sModel - 1] == ',' ) {
			pQueryCommand->sModel[ nChars_sModel - 1] = '\0';
			ungetc(',', pMemFile );
			if (pQueryControl->jVerbosity >= 10) fprintf(stdout, "  ParseLine: Found ',' on sModel string, pushing back onto stream \n");
		}

			// bMakeIsAny and bModelIsAny are initialized to false
		if ( strcasecmp( pQueryCommand->sMake, "Any" ) == 0 )
			pQueryCommand->bMakeIsAny  = true;
		if ( strcasecmp( pQueryCommand->sModel, "Any" ) == 0 )
			pQueryCommand->bModelIsAny = true;


		if (pQueryControl->jVerbosity >= 10) fprintf(stdout, "  ParseLine: Got sMake: >%s<, and sModel: >%s< \n", pQueryCommand->sMake, pQueryCommand->sModel);
		GobbleWhiteSpace( pMemFile, pQueryControl->jVerbosity >= jVerbosity_For_GobbleCharacters );  //  Possibly set EOF Flag

		// A Comma indicates that there is a Category Range
		while ( ! feof( pMemFile ) ) {

		  // Try to get next category string.   This may set eof.
		  GobbleCharactersFromBeginningOfStream( pMemFile, " ,", pQueryControl->jVerbosity >= jVerbosity_For_GobbleCharacters);
		  ReturnValue = fscanf(pMemFile, "%s", sString);
		  if (ReturnValue != 1) {
		    // This is typically how we will exit the loop
			if ( feof(pMemFile) ) {
				if (pQueryControl->jVerbosity >= 11) fprintf(stdout, "ParseLine:  Got to the end of the line scanning for Category label, possibly missing ',' \n");
				bGotIllFormedString = true;
				PossibleRigorousHalt;
				break;
			}

			if (pQueryControl->jVerbosity >= 11) fprintf(stderr, "  ParseLine:  did not get a Category label, possibly missing ','  \n");
			bGotIllFormedString = true;
			PossibleRigorousHalt;
			break;
		  }

			// Determine and set the enum for this category range
		  	// nCategoryRanges is the index to the Category Range currently being populated
		 if (pQueryControl->jVerbosity >= 11) fprintf(stdout, "  ParseLine:  got Category label: >%s< \n", sString);
		 ReturnValue = SetCategoryLabel( pQueryControl, &(pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges]), sString );
		 if ( ReturnValue != 0)  {
			if (pQueryControl->jVerbosity >= 11) fprintf(stdout, "ParseLine: did not get a valid Category label \n");
			bGotIllFormedString = true;
			PossibleRigorousHalt;
			break;
		 }

		   // Scan for relation
		 ReturnValue = fscanf(pMemFile, "%s", sString);
		 if (ReturnValue != 1) {
		   fprintf(stderr, "ParseLine: Did not get relation string \n");
			bGotIllFormedString = true;
			PossibleRigorousHalt;
			break;
		  }

		  // Determine and set the enum for this category range
		 if (pQueryControl->jVerbosity >= 11) fprintf(stdout, "ParseLine:  Got Relation string: '%s' \n", sString);
		 SetRelationalOperator( pQueryControl, &(pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges]), sString );
		 if (pQueryControl->jVerbosity >= 11) fprintf(stdout, "  ParseLine:  Got Relation: %s \n",
				  GetRelationOperator( pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges].eRelation ) );

		 	 // Now scan for the value
		 if ( pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges].eCategoryLabel != cl_Status ) {

		     ReturnValue = fscanf(pMemFile, "%f", &(pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges].Value));
			 if (ReturnValue != 1) {
			   fprintf(stderr, "ParseLine: Did not get a value \n");
				bGotIllFormedString = true;
				PossibleRigorousHalt;
				break;
			  }

		     if (pQueryControl->jVerbosity >= 11) fprintf(stdout, "  ParseLine:  Got Value: %4.1f \n",
		    		 	 	 	 	 	 	 	 	 	  pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges].Value);

		 } else {
			 //  We do have cl_Status
			 // Check that our relation is EqualTo
			 if ( pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges].eRelation  != re_EqualTo ) {
			   fprintf(stderr, "ParseLine: Did not get Relation EqualTo with Category label Status. \n");
			   bGotIllFormedString = true;
				PossibleRigorousHalt;
			   break;
			 }

			 ReturnValue = fscanf(pMemFile, "%s", sString);
			 SuppressCharactersOffEndOfString(sString, ",", pQueryControl->jVerbosity>=jVerbosity_For_SuppressCharacters);

			 if ( stricmp(sString, "New" ) == 0)  //  Case insensitive comparison
				 //  1 for New  (matchine bCarIsNew
				 pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges].Value = 1;
			  else if ( stricmp(sString, "Used" ) == 0)
				  // 0 for Used
				  pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges].Value = 0;
			  else {
			    fprintf(stderr, "ParseLine: Did not get New or Used as value for Status. \n");
			    bGotIllFormedString = true;
				PossibleRigorousHalt;
			    break;
			  }
		      if (pQueryControl->jVerbosity >= 11) fprintf(stdout, "  ParseLine:  Got Car Status of %s \n",
					 (  pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges].Value != 0 ? "New" : "Used") );
		 }  //  if ( pQueryCommand->CategoryRanges[ pQueryCommand->nCategoryRanges].eCategoryLabel != cl_Status )


		 // Get ready to process the next Category Range
		 GobbleWhiteSpace( pMemFile, pQueryControl->jVerbosity >= jVerbosity_For_GobbleCharacters);
		 pQueryCommand->nCategoryRanges++;
		}  //  while ( !feof(pMemFile) )

		if ( bGotIllFormedString ) {
		   if (pQueryControl->jVerbosity >= 1) fprintf(stderr, "  ParseLine:  Got an Ill Formed String, >%s< \n", sInputString);
		   fclose( pMemFile );
		   return FAILURE;
		}
	}  // 	if ( pQueryCommand->eQueryCommand == qc_Select || pQueryCommand->eQueryCommand == qc_Add )

	fclose( pMemFile );
	return SUCCESS;
}  //  ParseQueryCommandLine




float GetCategoryValueForCar( CategoryLabels_t eCategoryLabel, CarRecord_t *pCarRecord ) {
	switch ( eCategoryLabel ) {
	case cl_Year :
		return pCarRecord->jYear;
		break;
	case cl_Price :
		return pCarRecord->jAskingPrice;
		break;
	case cl_EngineSize :
		return pCarRecord->EngineSize;
		break;
	case cl_Milage :
		return pCarRecord->jMilage;
		break;
	case cl_Status :
		return pCarRecord->bCarIsNew;
		break;
	default :
		while (1) ;
	}
}


bool VerifyCategoryIsInRange( CategoryRange_t * pCategoryRange, CarRecord_t * pCarRecord ) {
  float CarValue, RangeValue;


  if ( pCategoryRange->eCategoryLabel != cl_Status ) {

	  //  Fetch the value on this category from the Car Record
	  CarValue   = GetCategoryValueForCar( pCategoryRange->eCategoryLabel, pCarRecord );
	  RangeValue = pCategoryRange->Value;

	  switch(pCategoryRange->eRelation) {
	  case re_LessThan :
		  if ( CarValue < RangeValue )   return true;  else return false;
	  case re_LessThanOrEqualTo:
		  if ( CarValue <= RangeValue )  return true;  else return false;
	  case re_EqualTo:
		  if ( CarValue == RangeValue )  return true;  else return false;
	  case re_GreaterThanOrEqualTo:
		  if ( CarValue >= RangeValue )  return true;  else return false;
	  case re_GreaterThan:
		  if ( CarValue > RangeValue )   return true;  else return false;
	  default:
		  while (1) ; // Error
	  }  // swtich
  } else {
	  // is cl_Status
	  CarValue   = pCarRecord->bCarIsNew ;
	  RangeValue = pCategoryRange->Value != 0.0;
	  if ( CarValue == RangeValue )  return true;  else return false;
  }

	return FAILURE;
}



/*********************************
 * TestWhetherCarMatchesQueryValues  Determine whether a car matches a query.
 *                           See list of return values
 * Inputs:              Pointer to a Query Command, pointer to a Car Record
 * Outputs:             0 on match, integer indicates what didn't match
 * Side Effects:        none
********************************/
int TestWhetherCarMatchesQueryValues(  QueryCommand_t * pQueryCommand,  CarRecord_t * pCarRecord ) {
	int jCategoryRange;
	bool bCarMatchesCriterion;

		// Check to see if Make matches.  Fail test if car passes (passing test leads to return)
	if ( strcasecmp( pQueryCommand->sMake, "Any")  != 0 && strcasecmp( pQueryCommand->sMake, pCarRecord->sMake ) != 0 )
		return 1;

	if ( strcasecmp( pQueryCommand->sModel, "Any") != 0 && strcasecmp( pQueryCommand->sModel, pCarRecord->sModel ) != 0 )
		return 2;

	for ( jCategoryRange=0; jCategoryRange < pQueryCommand->nCategoryRanges; jCategoryRange++ ) {
	  bCarMatchesCriterion = VerifyCategoryIsInRange( &(pQueryCommand->CategoryRanges[jCategoryRange]), pCarRecord );
	  if ( bCarMatchesCriterion == false )
		  return 3 + jCategoryRange;
	}

	return 0;
}





/*********************************
 *  sNewOrUsed			Return string "New" or "Used"
 * Inputs:              boolean bIsNew
 * Outputs:             Pointer to "New" if bIsNew is true, else pointer to "Used"
 * Side Effects:        none
********************************/
char * sNewOrUsed(bool bIsNew) {
	if ( bIsNew )
		return "New";
	else
		return "Used";
}  //  sNewOrUsed


CarListNode_t * ReceiveAListOfCarRecords( CarRecord_t *pCarRecords, int nSpaceForCarRecords, int jSelfHandshakeBit, int jOtherHandshakeBit, int jVerbosity ) {
	CarListNode_t   * pCarDatabase = NULL,  * pCarListTail = NULL, * pCarListNode;
	int   	ReturnValue;

	FILE * pFileIn;
	char * pFgetsReturnValue;
	char sStringIn[ N_InputCharacters ];
	bool	bKeepGoing;
	int   nCarRecords = 0;

	if (jVerbosity >= 10) fprintf(stdout, "\n"); // Put a blank line in, to separate from other material

	bKeepGoing = true;
	while ( bKeepGoing ) {

		while ( ReadInBit(jOtherHandshakeBit) != 1 )        	// An error occurs if Student computer is booted while the Assesssment computer is mid-test
			FlashNUCLEO_LED(jNUCLEO_GreenLED);	// So block here until assessment computer reaches state 1
		WriteNUCLEO_LED(jNUCLEO_GreenLED, 0);

		// State 1
		ReturnValue = Start_UART5_Reception();
		WriteNUCLEO_LED(jNUCLEO_BlueLED, 1);  // Indicates listening

		// State 2
		WriteOutBit(jSelfHandshakeBit, 1);  // Raise handshake bit

		// State 21, Assessment Computer done transmitting.
		while ( ReadInBit(jOtherHandshakeBit) != 0 )        	// An error occurs if Student computer is booted while the Assesssment computer is mid-test
			FlashNUCLEO_LED(jNUCLEO_BlueLED);	// So block here until assessment computer reaches state 1

		ReturnValue       = Stop_UART5_Reception();
		// State 22
		WriteOutBit(jSelfHandshakeBit, 0);  // Lower handshake bit

		pFileIn           = UART5_OpenInStream();
		pFgetsReturnValue = fgets(sStringIn, N_InputCharacters, pFileIn);
		fclose(pFileIn);
		Initialize_cBufferIn( ' ' );

		if ( nCarRecords >= nSpaceForCarRecords )
			while (1) ;   // More car records transmitted than maximum capacity
		ReturnValue = ParseStringPopulateCarRecord( &pCarRecords[nCarRecords], sStringIn, jVerbosity);
		if (ReturnValue == 0) {
			// In this case, add the car record to the Car Database
			// Note that the database is a linked list, while pCarRecords is an array
			// Allocate a new node structure.   Then initialize it, and add it to the list.
			pCarListNode =  malloc ( sizeof( CarListNode_t ) );
			if ( pCarListNode == NULL ) {
				while (1) ;   // Malloc Error
			} else {
				memset( pCarListNode, 0, sizeof( CarListNode_t) );
				pCarListNode->pCarRecord = &( pCarRecords[nCarRecords] );
				if (pCarDatabase == NULL) {
					pCarDatabase = pCarListNode;
					pCarListTail  = pCarListNode;
				} else {
					pCarListTail->pNextCarListNode = pCarListNode;
					pCarListTail                   = pCarListNode;
				}
			}
			nCarRecords++;
		} else if (ReturnValue == EOF)
			bKeepGoing = false;
		else
			while (1) ;  // Error, 0 and EOF should be only possibilities

		//  Wait for State 1,  Wait for OutBit1 to go to 1
		while ( bKeepGoing && ReadInBit(jOtherHandshakeBit) != 1 )
			FlashNUCLEO_LED(jNUCLEO_RedLED);

	}  // while bKeepGoing
	return pCarDatabase;
} // ReceiveAListOfCarRecords





/*********************************
 *  Transmit_CarRecords  Transmit Car Records on UART5, with Bit0 / Bit1 handshaking
 * Inputs:              none
 * Outputs:             Number of Car Records transmitted
 * Side Effects:        none
********************************/
int Transmit_CarRecords( CarListNode_t *pCarList, int jSelfHandshakeBit, int jOtherHandshakeBit, int jVerbosity ) {
  int jCarRecord = 0;
  CarListNode_t * pWorkingCarNode = pCarList;
  int nCarRecordsTransmitted = 0;
  FILE * pFileOut;

#ifdef B_IS_ASSESSMENT
  char	sHD44780String[N_InputCharacters];
  //                                         01234567890123456789
  HD44780_SetCursor(0,2);  HD44780_PrintStr(" Transmitting dB");
#endif

  while ( pWorkingCarNode != NULL ) {

	  //State 21
	  WriteOutBit(jSelfHandshakeBit, 1);  // Raise handshake bit

	 		  //Wait for State 22, ready to receive
	  while ( ReadInBit(jOtherHandshakeBit) != 1 )
		  FlashNUCLEO_LED(jNUCLEO_BlueLED);

	  if (jVerbosity >= 10) {
		  fprintf(stdout, "Transmit_CarRecords %3d \n", jCarRecord);
		  PrettyPrintACar( stdout, pWorkingCarNode->pCarRecord );
	  }

	  // Transmit in State 22
	  pFileOut = UART5_OpenOutStream();
	  PrettyPrintACar( pFileOut, pWorkingCarNode->pCarRecord );
	  UART5_CloseOutStream( pFileOut );
	  BlockingTransmit_cBufferOut( );

	  //State 23, transmit finished
	  WriteOutBit(jSelfHandshakeBit, 0);  // Lower handshake bit, indicating sent

	  // Wait for State 24, Receive finished.
	  while ( ReadInBit(jOtherHandshakeBit) != 0 )
			FlashNUCLEO_LED(jNUCLEO_BlueLED);

	  nCarRecordsTransmitted++;

#ifdef  B_IS_ASSESSMENT
	  HD44780_SetCursor(16,2);
	  sprintf(sHD44780String, " %3d", jCarRecord+1);
	  HD44780_PrintStr(sHD44780String);
#endif
	  // Increment down the list
	  pWorkingCarNode = pWorkingCarNode->pNextCarListNode;
	  jCarRecord++;

  	  }  //  while ( pWorkingCarNode != NULL )

  	  // Send Termination
  	  //State 21
  WriteOutBit(jSelfHandshakeBit, 1);  // Raise handshake bit

	 		  //Wait for State 22, ready to receive
  while ( ReadInBit(jOtherHandshakeBit) != 1 )
	  FlashNUCLEO_LED(jNUCLEO_BlueLED);

	  // Transmit in State 22
  pFileOut = UART5_OpenOutStream();
  fprintf( pFileOut, " ");
  UART5_CloseOutStream( pFileOut );
  BlockingTransmit_cBufferOut( );

	  //State 23, transmit finished
  WriteOutBit(jSelfHandshakeBit, 0);  // Lower handshake bit, indicating sent

	  // Wait for State 24, Receive finished.
  while ( ReadInBit(jOtherHandshakeBit) != 0 )
	  FlashNUCLEO_LED(jNUCLEO_BlueLED);


  return nCarRecordsTransmitted;
} // Transmit_CarRecords



/*********************************
 *  CountCarsOnList   Count the Cars On a List
 * Inputs:              Pointer to a list of cars
 * Outputs:             Count of cars on the list
 * Side Effects:        none
********************************/
int CountCarsOnList( CarListNode_t * pListOfCars ) {
  int nCarsOnList = 0;

  if (pListOfCars == NULL)
	  return nCarsOnList;

  while ( pListOfCars != NULL ) {
	  pListOfCars = pListOfCars->pNextCarListNode;
	  nCarsOnList++;
  }
  return nCarsOnList;
}


/*********************************
 *  FindTailOfCarList   Go down list of cars, to the last one.   Return the pointer to the last one.
 * Inputs:              Pointer to a list of cars
 * Outputs:             Pointer to the tail of the list, NULL if the list is NULL.
 * Side Effects:        none
********************************/
CarListNode_t * FindTailOfCarList( CarListNode_t * pListOfCars ) {

  if (pListOfCars == NULL)
	  return NULL;

  while ( pListOfCars->pNextCarListNode != NULL ) {
	  pListOfCars = pListOfCars->pNextCarListNode;
  }
  return pListOfCars;
}


/*********************************
 *  PrettyPrintACar  Pretty Print one car to pFile
 * Inputs:				pFile, CarRecord_t * pCarRecord
 * Outputs:             none
 * Side Effects:        Printing one line
********************************/
void PrettyPrintACar(FILE *pFile, CarRecord_t * pCarRecord) {
	fprintf(pFile, "%-10s %-7s,   Year: %4d,  Engine: %.1fL,  Milage: %6d,  Asking Price: %6d,  Status: %-4s, Index: %d \n",
			pCarRecord->sMake, 		pCarRecord->sModel, pCarRecord->jYear,  pCarRecord->EngineSize,
			pCarRecord->jMilage, pCarRecord->jAskingPrice,
			sNewOrUsed( pCarRecord->bCarIsNew ), pCarRecord->jRecordIndex );
}  //  PrettyPrintACar


/*********************************
 * PrettyPrintListOfCars  Pretty Print a list of cars to pFile
 * Inputs:              pFile, pListOfCars
 * Outputs:             nCarsPrinted
 * Side Effects:        Printing n lines
********************************/
int PrettyPrintListOfCars( FILE *pFile,  CarListNode_t * pListOfCars) {
	int  nCarsPrinted  			= 0 ;
	CarListNode_t * pCarToPrint	= pListOfCars;

	while ( pCarToPrint != NULL ) {
		PrettyPrintACar(pFile, pCarToPrint->pCarRecord);
		pCarToPrint = pCarToPrint->pNextCarListNode;
	}
	return nCarsPrinted;
}  //  PrettyPrintListOfCars


/*********************************
 *  PrettyPrintAQueryCommand
 * Inputs:              pFile, A Query Record
 * Outputs:             none
 * Side Effects:        Printing a query record
********************************/
void PrettyPrintAQueryCommand(FILE *pFile, QueryCommand_t *pQueryCommand) {

	//   What gets printed will depend on what the command is
  fprintf(stdout, "Pretty Print Query Command: %s  \n", GetQueryCommandName( pQueryCommand->eQueryCommand ) );

  if ( pQueryCommand->eQueryCommand == qc_Select || pQueryCommand->eQueryCommand == qc_Add ) {
	  fprintf(stdout, "  Make and Model: %s %s \n", pQueryCommand->sMake, pQueryCommand->sModel);
	  for (int jCategoryRange=0; jCategoryRange < pQueryCommand->nCategoryRanges; jCategoryRange++) {
		  if ( pQueryCommand->CategoryRanges[jCategoryRange].eCategoryLabel != cl_Status )
			fprintf(stdout, "  %-8s %2s %.1f \n",
					GetCategoryLabel( pQueryCommand->CategoryRanges[jCategoryRange].eCategoryLabel ),
					GetRelationOperator(  pQueryCommand->CategoryRanges[jCategoryRange].eRelation ),
					pQueryCommand->CategoryRanges[jCategoryRange].Value );
		  else
			fprintf(stdout, "  %-8s %2s %s \n",
					GetCategoryLabel( pQueryCommand->CategoryRanges[jCategoryRange].eCategoryLabel ),
					GetRelationOperator(  pQueryCommand->CategoryRanges[jCategoryRange].eRelation ),
					sNewOrUsed( pQueryCommand->CategoryRanges[jCategoryRange].Value != 0.0 ) );
	  }		// for jCategoryRange
  } 		// if Select or Add
} // PrettyPrintAQueryCommand


