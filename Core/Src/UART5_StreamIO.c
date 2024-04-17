/***********************************
 *
 * Module for implementing stream IO over a UART
 * Main Concepts:
 *   C language, POSIX 2008,  stream I/O to a memory record (a vector of Char)
 *   Using stream I/O permits fscanf()  for formatted conversion.
 *
 *   Functions:
 *      SetLED_ForReception()     //  If 1, 2 or 3,  that LED flashes
 *      UnsetLED_ForReception()   //  Resets jLED_ForReception so no LED flash.
 *
 ***********************************/


// Include files
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "errno.h"
#include "main.h"

#include "EE240_Utilities.h"
#include "UART5_StreamIO.h"

//  External global variables
extern UART_HandleTypeDef huart5;

//  Persistent data of the UART5 Stream IO system
static char cBufferIn[nUART5_Buffer_Characters+1];
static char cBufferOut[nUART5_Buffer_Characters+1];
static uint8_t  UART5_IncomingByte  = 0;    // 0 is used to detect undesired initial interrupt
static int      nUART5_ReceiveCalls = 0;    // 0 is used to detect undesired initial interrupt
static int jcBufferIn  = 0;
static int jcBufferOut = 0;
static FILE *pFileIn = NULL;
static FILE *pFileOut = NULL;


static int	 jLED_ForReception = jLED_NotSet;


// Template
/*********************************
 *
 *
 * Inputs:              None
 * Outputs:             None
 * Side Effects:
********************************/




/*********************************
 *  SetLED_ForReception
 *     Sets jLED to flash during reception
 * Inputs:              jLED
 * Outputs:             0 for Success 
 * Side Effects: 	Sets jLED_ForReception
********************************/
int SetLED_ForReception( int jLED ) {
	int ReturnValue = SUCCESS;
	if (jLED >=  jNUCLEO_FirstLED  && jLED <= jNUCLEO_LastLED  ) {
	  jLED_ForReception = jLED;
	} else {
	  ReturnValue = FAILURE;
	}
  return ReturnValue;
}   // SetLED_ForReception


/*********************************
 *   SetLED_ForReception
 *     Unsets jLED to flash during reception
 *
 * Inputs:              None
 * Outputs:             None
 * Side Effects:        Unsets jLED_ForReception
********************************/
void UnsetLED_ForReception(void) {
	jLED_ForReception = jLED_NotSet;
}  //  UnsetLED_ForReception




/*********************************
 * HAL_UART_RxCpltCallback   This routine is called when there is a UART interrupt. 
 *                           It must test for which UART is interrupting.  
 *                           Here, the routine puts the received character into cBufferIn; 
 *
 * Inputs:              Pointer to uart control handle. 
 * Outputs:             None
 * Side Effects:        A character is transferred to cBufferIn.  
 *                      Interrupt is reactivated, for next character. 
 ********************************/
/* This callback is called by the HAL_UART_IRQHandler when the given number of bytes are received */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Tests for a 0 data on first call is because ficticious 0 data at head of a string has been seen.
  if (huart->Instance == UART5 && !( nUART5_ReceiveCalls==0 && UART5_IncomingByte == 0 ) )   {
	  nUART5_ReceiveCalls++;

	if ( jLED_ForReception !=  jLED_NotSet )
		WriteNUCLEO_LED(jLED_ForReception, nUART5_ReceiveCalls & 0x02 );

	if (jcBufferIn < nUART5_Buffer_Characters) {
		//ReturnValue = fputc((int)UART5_IncomingByte, pFileWriteToBufferIn);
		//if (ReturnValue == EOF) while (1) ;
	//	fflush( pFileWriteToBufferIn );
		//jcBufferIn++;
		cBufferIn[ jcBufferIn++ ] = UART5_IncomingByte ;
	//	fpurge(pFileIn);

	} else {
		//  Figure this out
		while (1) ;
	}

    /* Reenable interrupt mode reception */
    HAL_UART_Receive_IT(&huart5, &UART5_IncomingByte, 1);
  }  // if UART5
}    // HAL_UART_RxCpltCallback


/*********************************
 *  Start_UART5_Reception
 *
 * Inputs:          None
 * Outputs:         SUCCESS if launching returns HAL_OK, else FAILURE 
 * Side Effects:    Launch receiving characters from UART5
********************************/
int  Start_UART5_Reception(void) {
	HAL_StatusTypeDef  HAL_Status;
	int ReturnValue;


	UART5_IncomingByte  = 0;    // 0 is used to detect undesired initial interrupt
	nUART5_ReceiveCalls = 0;    // 0 is used to detect undesired initial interrupt

	HAL_Status = HAL_UART_Receive_IT(&huart5, &UART5_IncomingByte, 1);
	ReturnValue = ( HAL_Status == HAL_OK ) ? SUCCESS  :  FAILURE;
	return ReturnValue;
}  //  Start_UART5_Reception



/*********************************
 *  Stop_UART5_Reception
 *
 * Inputs:          None
 * Outputs:         SUCCESS if Abort returns HAL_OK, else FAILURE
 * Side Effects:    Stop receiving characters from UART5
********************************/
int  Stop_UART5_Reception(void) {
	HAL_StatusTypeDef  HAL_Status;
	int ReturnValue;

	HAL_Status = HAL_UART_AbortReceive(&huart5);
	ReturnValue = ( HAL_Status == HAL_OK ) ? SUCCESS  :  FAILURE;
	return ReturnValue;
}  //  Stop_UART5_Reception




/*********************************
 *  BlockingTransmit_cBufferOut  transmit the contents of cBufferOut
 *
 * Inputs:          None
 * Outputs:         Count of characters transmitted,   else FAILURE
 * Side Effects:    Trans
********************************/
int BlockingTransmit_cBufferOut( void ) {
  HAL_StatusTypeDef  HAL_Status;
  int ReturnValue = 0, jCharacter;
  for (jCharacter=0; jCharacter<jcBufferOut; jCharacter++ ) {
	  HAL_Status = HAL_UART_Transmit( &huart5, (uint8_t *)&cBufferOut[jCharacter], 1, 100 );
	  if ( ReturnValue != FAILURE && HAL_Status == HAL_OK ) {
		  ReturnValue++;
	  } else {
		  ReturnValue = FAILURE;
	  }
  }

  return ReturnValue;
}  //  BlockingTransmit_cBufferOut


/*********************************
 *  UART5_OpenInStream    Open a file to connect to the memory stream.   
 * Inputs:              None
 * Outputs:              *FILE, handle to the input stream.
 * Side Effects:        The cBufferIn is opened for reading.  The Stream is initialized to the
 *                      Characters in cBufferIn;
********************************/
FILE * UART5_OpenInStream(void) {

	if (jcBufferIn > 0) {
	// Use the memory stream open
		pFileIn   = fmemopen(cBufferIn, jcBufferIn, "r");
	} else {
		//  fmemopen errors on a zero length string
		//  Normally, the pFileIn = NULL would be passed back to caller
		//  But here, we want life to be simple for students, and so a single ' ' is inserted in cBufferIn,
		//    and a length 1 buffer is opened.
		// Set first character to blank
		cBufferIn[0] = ' ';
		jcBufferIn = 1;
		pFileIn   = fmemopen(cBufferIn, jcBufferIn, "r");
	}

		//  Detect a failure to open pFileIn, and halt
	if ( pFileIn == NULL )
		while(1) ;

 // setvbuf(pFileIn, NULL, _IONBF, 0);

  return pFileIn;
}  //  UART5_OpenInStream

/*********************************
 *  UART5_OpenOutStream
 * Inputs:              None
 * Outputs:             *FILE, handle to the output stream.
 * Side Effects:        The cBufferOut is opened for writing
********************************/
FILE * UART5_OpenOutStream(void) {

  pFileOut  = fmemopen(cBufferOut, nUART5_Buffer_Characters, "w");
  // setvbuf(pFileOut, NULL, _IONBF, 0);

	//  Detect a failure to open pFileOut, and halt
  if ( pFileOut == NULL )
	  while(1) ;

  return pFileOut;
}  //


/*********************************
 *  UART5_CloseOutStream
 * Inputs:              FILE *pFileOut,  output stream handle to close
 * Outputs:             None
 * Side Effects:        The cBufferOut is Closed for reading
********************************/
int UART5_CloseOutStream(FILE *pFileOut) {
  int      ReturnValue;
  fpos_t   jPosition;

  fflush( pFileOut );				// Transfer characters to cBufferOut;
  fgetpos( pFileOut, &jPosition );  // Find total number of characters transferred.
  jcBufferOut = jPosition;      	// Set jcBufferOut to indicate the number of characters in the out-bound buffer.

  // Validate that jPosition looks correct.
  if ( strlen(cBufferOut) != jcBufferOut )
	  while(1) ;  // Halt

  ReturnValue = fclose( pFileOut );
  return ReturnValue;
}  //  UART5_CloseOutStream



/*********************************
 *  LoadTestDataIntoUART5_Buffers
 * Inputs:              None
 * Outputs:             None
 * Side Effects:        Test data loaded into cBufferIn and cBufferOut
********************************/
void LoadTestDataIntoUART5_Buffers( void ) {
	for(int jj = 0; jj < nUART5_Buffer_Characters; jj++) {
		cBufferIn [jj]  = 'A' + (jj % 10);
		cBufferOut[jj]  = 'M' + (jj % 10);
	}
	cBufferIn [nUART5_Buffer_Characters]  = '\0';
	cBufferOut[nUART5_Buffer_Characters]  = '\0';
}  //  LoadTestDataIntoUART5_Buffers




/*********************************
 *  Initialize_cBufferIn   Initialize cBufferIn and jcBufferIn 
 *
 * Inputs:         A character used to fill the buffer 
 * Outputs:        None
 * Side Effects:   Global variables cBufferIn and jcBufferIn are initialized
********************************/
void Initialize_cBufferIn( char ACharacter) {
  memset( cBufferIn, ACharacter, nUART5_Buffer_Characters+1) ;
  jcBufferIn = 0; 
}



/*********************************
 *  Initialize_cBufferOut   Initialize cBufferOut and jcBufferOut 
 *
 * Inputs:         A character used to fill the buffer 
 * Outputs:        None
 * Side Effects:   Global variables cBufferOut and jcBufferOut are initialized
********************************/
void Initialize_cBufferOut( char ACharacter) {
  memset( cBufferOut, ACharacter, nUART5_Buffer_Characters+1) ;
  jcBufferOut = 0; 
}

