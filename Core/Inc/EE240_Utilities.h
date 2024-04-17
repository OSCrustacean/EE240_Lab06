/*
 * EE240_Utilities.h
 *
 *   Utiliites for EE240 programming environement
 *
 *  Created on: Mar 24, 2023
 *      Author: bsra
 */

#ifndef INC_EE240_UTILITIES_H_
#define INC_EE240_UTILITIES_H_

#include <stdint.h>

// General Defines
#define SUCCESS  0
#define FAILURE  -1

// Symbolic names for things
#define  jLED_NotSet  -1


// Parameters for the NUCLEO F429Zi
#define jNUCLEO_FirstLED   1
#define jNUCLEO_LastLED    3
#define jNUCLEO_GreenLED   1
#define jNUCLEO_BlueLED    2
#define jNUCLEO_RedLED     3


// Parameters related to Timer2
#define Tim2_CountsOneMilliSecond   (100L)
#define Tim2_CountsOneSecond        (1000 * Tim2_CountsOneMilliSecond)


// Setup to help visualize progress of state machine
#define MarkEvenState 	WriteNUCLEO_LED(jNUCLEO_RedLED, 0)
#define MarkOddState 	WriteNUCLEO_LED(jNUCLEO_RedLED, 1)

// Function declarations

int ProcessBackspaceCharacters( char sInputString[], int jVerbosity );
int GobbleWhiteSpace( FILE *pFile, bool bVerboseReporting );

int WriteNUCLEO_LED(int jLED, int jValue);
int ClearNUCLEO_LEDs( void );
int FlashNUCLEO_LED(int jLED);
void BlockingFlashAndSetNUCLEO_LED(int jLED, int nMilliSeconds);

int ReadUserButton( void );
void BlockUntilUserButtonRelease( int jLED );

void  InitializeConsoleCommunication( void);
int  PrintOnConsolePort( const char *pString, uint16_t nBytes);

void ConfigureTimer2ToStopWhenDebuggerStops( void );
uint32_t  ReadTimer2( void);

void InitializeHD44780_Display( void );

int WriteOutBit(int jOutBit, int jValue);
int ClearOutBits( void );
int ReadInBit   (int jInBit);


#endif /* INC_EE240_UTILITIES_H_ */
