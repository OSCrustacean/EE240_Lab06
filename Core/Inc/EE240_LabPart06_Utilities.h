/*
 * EE240_LabPart06.h
 *
 *  Created on: Apr 3, 2023
 *      Author: bsra
 */

#ifndef INC_EE240_LABPART06_UTILITIES_H_
#define INC_EE240_LABPART06_UTILITIES_H_

#include <stdio.h>
#include <stdbool.h>

// Constants
#define N_EngineSizes 				3
#define N_ModelRecords 				6
#define N_CarRecords 				101
#define N_InputCharacters  			128
#define N_CharactersMakeModelName 	12
#define N_CharactersCategoryLabel 	12
#define N_CharactersStatusValue		12
#define N_MakesOrModels 			 6
#define N_CharactersQueryCommand	27
#define N_QueryCommands              7
// Number of different category labels
#define N_CategoryLabels 		 	 5
	// Maximum number of category ranges in a single query
#define N_MaxCategoryRanges			 6
#define jThisYear					2023

#define	jVerbosity_For_GobbleCharacters    23
#define	jVerbosity_For_SuppressCharacters  25

// Structure for a record of a car
typedef struct CarRecord_s {
	char  *	sMake;   		// String
	char  *	sModel;      	// String
	int     jYear;			// Year
	int   	jMilage; 		// Miles
	int	jAskingPrice;		// Dollars
	float  	EngineSize;     // tenths of liter
	int     jRecordIndex;	// A Record ID, for debugging
	bool	bCarIsNew;
} CarRecord_t ;

typedef struct CarListNode_s {
 CarRecord_t 		  *	pCarRecord;
 struct CarListNode_s *	pNextCarListNode;
} CarListNode_t;


// Structure for information to build a database of cars
typedef struct  ModelRecord_s {
	char  *	sMake;   		// String
	char  *	sModel;      	// String
	int 	MinMiles, MaxMiles;
	int		MinMSRP;
	float 	MSRP_Multiplier;
	float	EngineSizes[N_EngineSizes];
	int		nEngineSizes;
	bool	bIsExpensiveSportsCar;
} ModelRecord_t;


// Types of possible Query Commands
typedef enum PossibleQueryCommands_e {
	qc_QueryCommandNotSet,
	qc_Select,
	qc_Add,
	qc_Report,
	qc_ReportDatabase,
	qc_Report_nCarsOnWorkingList,
	qc_Reset,
	qc_Help,
} PossibleQueryCommands_t;

typedef enum CategoryLabels_e {
	cl_CategoryLabelNotSet,
	cl_Year,
	cl_Price,
	cl_EngineSize,
	cl_Milage,
	cl_Status
} CategoryLabels_t;

typedef enum Relations_e {
	re_RelationNotSet,
	re_LessThan,
	re_LessThanOrEqualTo,
	re_EqualTo,
	re_GreaterThanOrEqualTo,
	re_GreaterThan
} Relations_t;




typedef struct CategoryRange_s {
	CategoryLabels_t	eCategoryLabel;
	Relations_t  		eRelation;
	float	       		Value;
} CategoryRange_t;

typedef struct QueryCommand_s {
	char sMake[N_CharactersMakeModelName];
	char sModel[N_CharactersMakeModelName];
	CategoryRange_t    CategoryRanges[N_MaxCategoryRanges];
	PossibleQueryCommands_t 	eQueryCommand;
	int nCategoryRanges;
	bool  bMakeIsAny;
	bool  bModelIsAny;
}  QueryCommand_t;

typedef struct QueryControl_s {
	char * 	sMakeNames[N_MakesOrModels];
	char * 	sModelNames[N_MakesOrModels];
	char * 	sQueryCommands[N_QueryCommands];
	char * 	sCategoryLabels[N_CategoryLabels];
	char   	StringSpaceForMakeAndModelNames[2 * N_MakesOrModels * (N_CharactersMakeModelName+1) ];  // Space for names
	int		nModelRecords;
	int		nCarRecords;
	int 	nMakeNames;
	int     nModelNames;
	int 	nQueryCommands;
	int  	nCategoryLabels;
	int 	jVerbosity;
} QueryControl_t;

// Function Declarations
void InitCarRecordsAndModels( void );
ModelRecord_t * Get_pModelRecords( void );
CarRecord_t * Get_pCarRecords( void );

int DetermineOrderOfTwoCarRecords( CarRecord_t * pCarRecord1,  CarRecord_t * pCarRecord2 );
void PrintStringsFromAList( FILE * pFile, char * sStrings[], int nStrings );
void PrintHelpMessage( FILE * pFile,  QueryControl_t * pQueryControl );

int CharacterToPrintableString(char AChar, char sString[]);
int SuppressCharactersOffEndOfString( char sString[], char sCharactersToSuppress[], bool bVerboseReporting ) ;
int ParseStringPopulateCarRecord( CarRecord_t *pCarRecord, char *sInputString, int jVerbosity);

char *FindStringOnList(char * sString, char *ListOfStrings[], int nStringsOnList, int * pjStringOnList );
int SortCarList( CarListNode_t * pCarList, int jVerbosity );
int EliminateDuplicatesInCarList( CarListNode_t * pCarList, int jVerbosity );
int CompareTwoListsOfCars( CarListNode_t *pFirstCarList, CarListNode_t *pSecondCarList, int jVerbosity);

int InitializeQueryControl(  QueryControl_t *pQueryControl, ModelRecord_t *pModelRecords, int nModelRecords, int nCarRecords, int jVerbosity );


int TestWhetherCarMatchesQueryValues(  QueryCommand_t * pQueryCommand,  CarRecord_t * pCarRecord );

int SetQueryCommand( QueryControl_t *pQueryControl, QueryCommand_t * pQueryCommand, char * sString );
int SetCategoryLabel( QueryControl_t *pQueryControl, CategoryRange_t * pCategoryRange, char * sString );
int ParseQueryCommandLine(  QueryControl_t *pQueryControl, QueryCommand_t * pQueryCommand, char sInputString[] );

int ProcessQueryCommand( QueryControl_t * pQueryControl, QueryCommand_t * pQueryCommand,  CarListNode_t * pCarsDatabase,  CarListNode_t ** ppCurrentWorkingList);


CarListNode_t * ReceiveAListOfCarRecords( CarRecord_t *pCarRecords, int nSpaceForCarRecords, int jSelfHandshakeBit, int jOtherHandshakeBit, int jVerbosity );
int Transmit_CarRecords( CarListNode_t *pCarList, int jSelfHandshakeBit, int jOtherHandshakeBit, int jVerbosity );

int CountCarsOnList( CarListNode_t * pListOfCars );
CarListNode_t * FindTailOfCarList( CarListNode_t * pListOfCars ) ;

void PrettyPrintACar(FILE *pFile, CarRecord_t * pCarRecord);
int  PrettyPrintListOfCars( FILE *pFile,  CarListNode_t * pListOfCars);
void PrettyPrintAQueryCommand(FILE *pFile, QueryCommand_t *pQueryCommand);



#endif /* INC_EE240_LABPART06_UTILITIES_H_ */
