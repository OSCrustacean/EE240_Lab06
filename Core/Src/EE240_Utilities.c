/*
 * EE240_Utilities.c
 * Basic Utilities, to do common EE240 things.
 *   To Abstract away HAL calls, in ways useful to EE 240.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include "main.h"
#include "EE240_Utilities.h"
#include "EE240_LiquidCrystal_I2C.h"

// Using UART3 for console communications
extern UART_HandleTypeDef huart3;
// Timer 2 for ReadTimer2
extern TIM_HandleTypeDef htim2;


// Template
/*********************************
 *
 * Inputs:              None
 * Outputs:             None
 * Side Effects:
********************************/


/*********************************
 *  __io_putchar  Called from write,  hooks printf to huart3 (put a character on stdout)
 *
 * Inputs:    The Character to transmit (converted to an int)
 * Outputs:   Returns the character sent, or EOF on failure (EOF is -1 in STM32CubeIDE)
 * Side Effects:
********************************/
int __io_putchar(int ACharacter) {
 HAL_StatusTypeDef  HAL_Status;
 int                ReturnValue;

	// printf() outputs characters through the routine __io_putchar()
  HAL_Status = HAL_UART_Transmit(&huart3, (uint8_t *)&ACharacter, 1, 100);

  	  // See man  putchar()
  ReturnValue = ( HAL_Status == HAL_OK ) ? ACharacter  :  EOF;

  return ReturnValue;
} //__io_putchar


/*********************************
 *  __io_getchar  Called from write,  hooks scanf to huart3 (get a character from stdin)
 *                Converts CR to NL, for fgets to work.
 * Inputs:    None
 * Outputs:   Returns the character received, or EOF on failure (EOF is -1 in STM32CubeIDE)
 * Side Effects:
********************************/
int __io_getchar(void) {
  uint8_t ACharacter = 0;
  HAL_StatusTypeDef  HAL_Status;
  int                ReturnValue;

  // scanf() inputs characters through the routine __io_getchar()

  /* Clear the Overrun flag just before receiving the first character */
  __HAL_UART_CLEAR_OREFLAG(&huart3);  // It should be checked when this is necessary

  /* Wait for reception of a character on the USART RX line and echo this character on console */
  HAL_Status = HAL_UART_Receive(&huart3, (uint8_t *)&ACharacter, 1, HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart3, (uint8_t *)&ACharacter, 1, 100);

  //  fgets() needs the new line character.   No way at this point, or one up, to add a character for \r\n
  if (ACharacter == '\r') 	  ACharacter = '\n';

  ReturnValue = ( HAL_Status == HAL_OK ) ? ACharacter  :  EOF;

  return ReturnValue;
}


/*********************************
 *  GobbleWhiteSpace  White space characters are removed from a stream, up to the first non-white space character, which could be EOF.
 *
 * Inputs:              pFile, bVerboseReporting
 * Outputs:             AChar, the last character read
 * Side Effects:		White space characters are removed from a stream
 *                      If bVerboseReporting is true, messages are sent to stdout
********************************/
int GobbleWhiteSpace( FILE *pFile, bool bVerboseReporting ) {
	int AChar;

	do {
		AChar = getc( pFile );
		if ( bVerboseReporting ) {
			fprintf(stdout, "    GobbleWhiteSpace:  getc AChar got: x%02x", AChar);
			if ( isprint(AChar) )  //  If character is printable, show it
				fprintf(stdout, " >%c< \n", AChar);
			else
				fprintf(stdout, "\n");
		}
	} while ( isspace(AChar) );  //  == '\n' || AChar == '\r' || AChar == ' ' || AChar == '\t' );

	// If we did not get an EOF, then we have a non-white space character, and do unget.
	if ( AChar != EOF ) {
		if ( bVerboseReporting ) fprintf(stdout, "    GobbleWhiteSpace:  ungetc: x%02x \n", AChar);
		ungetc( AChar, pFile );
	}

	return AChar;
}


int PrintStringWithSpecialCharacters( FILE * pFile, char sString[] ) {
	char AChar;
	int  jCharacterToPrint = 0, nCharactersPrinted = 0;

	while( sString[ jCharacterToPrint ] != '\0' ) {
		AChar = sString[ jCharacterToPrint ];

		if ( isprint(AChar) ) {
			putc( AChar, pFile) ;
			nCharactersPrinted++;
		} else {
			switch ( AChar ) {
			case '\n' :
				fprintf(pFile, "\\n");
				nCharactersPrinted += 2;
				break;
			case '\t' :
				fprintf(pFile, "\\t");
				nCharactersPrinted += 2;
				break;
			default:
				fprintf(pFile, "\\%3o", AChar);
				nCharactersPrinted += 4;
				break;
			}
		}
		jCharacterToPrint++;
	}  // while
	return nCharactersPrinted;
}  // PrintStringWithSpecialCharacters



/*********************************
 *  ProcessBackspaceCharacters   Go through a string, and process the backspace characters
 *
 * Inputs:  			sInputString,  jVerbosity
 * Outputs:				0 for success
 * Side Effects:		Operates on the string, removes the character prior to each DEL character
********************************/
int ProcessBackspaceCharacters( char sInputString[], int jVerbosity ) {
	int jCharacterToRead = 0, jCharacterToWrite = 0;

	if (jVerbosity >= 11)  {
		fprintf(stdout, "ProcessBackspaceCharacters, Processing: >");
		PrintStringWithSpecialCharacters(stdout, sInputString );
		fprintf(stdout, "< \n");
	}

	while ( sInputString[ jCharacterToRead ] != 0) {

		if ( sInputString[ jCharacterToRead ] == 127  ) {
			if (jCharacterToWrite != 0)
				jCharacterToWrite--;
			else
				;
		} else {
			sInputString[ jCharacterToWrite++ ] = sInputString[ jCharacterToRead ] ;
		}

		jCharacterToRead++;

	}  // while
	sInputString[ jCharacterToWrite ] = '\0';

	if (jVerbosity >= 11) {
		fprintf(stdout, "ProcessBackspaceCharacters, Returning: >");
		PrintStringWithSpecialCharacters(stdout, sInputString );
		fprintf(stdout, "< \n");
	}

  return 0;
}  //  ProcessBackspaceCharacters


/*********************************
 *  InitializeConsoleCommunication
 *
 * Inputs:              None
 * Outputs:             None
 * Side Effects:   	 Buffering is suppressed in stdout and stdin, needed for Console communication
********************************/
void  InitializeConsoleCommunication( void) {
  // This must come after initialization of huart3
  setvbuf(stdin,  NULL, _IONBF, 0); 	// Needed for serial port connection to work with scanf
  setvbuf(stdout, NULL, _IONBF, 0);	// Needed, so that output is unbuffered, otherwise fflush(stdout) or \n required for serial output to be emitted.
  printf("\f");						// Clear anything currently on the Putty screen (\f is "form feed," and clears the Putty screen)
}


/*********************************
 *  PrintOnConsolePort   Print a string on the console port
 *
 * Inputs:              String and length
 * Outputs:             None
 * Side Effects:   	String goes out on the console port
********************************/
int  PrintOnConsolePort( const char *pString, uint16_t nBytes) {
	int  ReturnValue;
	ReturnValue = (int) HAL_UART_Transmit(&huart3, (uint8_t *)pString, nBytes, 100);
	return ReturnValue;
}  // PrintOnConsolePort



void ConfigureTimer2ToStopWhenDebuggerStops( void ) {
// From  https://community.st.com/s/question/0D50X00009XkYS5SAN/i-cant-make-timer-stop-while-debuging
//RCC->APB2ENR   |= RCC_APB2ENR_DBGMCUEN;           //enable MCU debug module clock
HAL_DBGMCU_EnableDBGStandbyMode();
HAL_DBGMCU_EnableDBGStopMode();
DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM2_STOP;   // Enable timer 2 stop
}


/*********************************
 *  ReadTimer2  Read Timer 2
 *
 * Inputs:              None
 * Outputs:             Current Timer2 value
 * Side Effects:		None
********************************/
uint32_t  ReadTimer2( void) {
  return __HAL_TIM_GET_COUNTER(&htim2);
}  // ReadTimer2


void InitializeHD44780_Display( void ) {
//  Initialize the HD44780  display
  HD44780_Init(nHD44780_Rows);
  HD44780_Clear();
}  //  InitializeHD44780_Display




/*********************************
 *  WriteNUCLEO_LED   Put a 1 or 0 on a Nucleo board LED
 *
 * Inputs:              jLED (1 .. 3), and jValue  (value to put on LED)
 * Outputs:             0 for Success, else -1
 * Side Effects:		LED port is changed
********************************/
int WriteNUCLEO_LED(int jLED, int jValue) {
	int ReturnValue = 0;
	switch( jLED) {
	case 1:
	   HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, jValue);
	   break;
	case 2:
	    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, jValue);
	    break;
	case 3:
	    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, jValue);
	    break;
	default:
		ReturnValue = -1;
	}
	return ReturnValue;
}  // WriteNUCLEO_LED

/*********************************
 *  ClearNUCLEO_LEDs   Put a 0 on all Nucleo board LEDs
 *
 * Inputs:              None
 * Outputs:             0 for Success, else -1
 * Side Effects:		LEDs set to 0
********************************/
int ClearNUCLEO_LEDs( void ) {
	int jLED;
	for (jLED = jNUCLEO_FirstLED; jLED <= jNUCLEO_LastLED; jLED++)
		WriteNUCLEO_LED(jLED, 0);
	return SUCCESS;
}


/*********************************
 *  FlashNUCLEO_LED		Non-blocking LED flash routine, based on Timer2
 *
 * Inputs:          jLED, if jLED >= 1 && jLED <= 3,  will flash the LED indicated, based on Timer2
 *                  Set jLED to 0 to stop flashing (no write to LED, in this case)
 * Outputs:         None.
 * Side Effects:	Flash LED.
********************************/
int FlashNUCLEO_LED(int jLED) {
	if ( jLED >= jNUCLEO_FirstLED && jLED <= jNUCLEO_LastLED )
		WriteNUCLEO_LED(jLED,  ReadTimer2() >> 16 & 0x01);
	return SUCCESS;
}  // FlashNUCLEO_LED




/*********************************
 *  BlockingFlashAndSetNUCLEO_LED		LED flash routine, based on Timer2
 *  									Flashes for nMilliSeconds, then leaves the LED ON. *
 * Inputs:          jLED, if jLED >= 1 && jLED <= 3,  will flash the LED indicated, based on Timer2
 *                  Set jLED to 0 to stop flashing (no write to LED, in this case)
 * Outputs:         None.
 * Side Effects:	Flash LED.
********************************/
void BlockingFlashAndSetNUCLEO_LED(int jLED, int nMilliSeconds) {
	uint32_t  jStartTime = ReadTimer2();

	while ( ReadTimer2() <  jStartTime + nMilliSeconds * Tim2_CountsOneMilliSecond)
		FlashNUCLEO_LED(jLED) ;

	WriteNUCLEO_LED(jLED,  1);
}  // FlashNUCLEO_LED


/*********************************
 *  ReadUserButton
 *
 * Inputs:          None
 * Outputs:         Current State Of The User Button.
 * Side Effects:	None.
********************************/
int ReadUserButton( void ) {
 return	 HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin)  != 0;
}


/*********************************
 *  BlockUntilUserButtonRelease
 *
 * Inputs:          jLED, if jLED >= 1 && jLED <= 3,  will flash the LED indicated while waiting
 *                  Set jLED to 0 to stop flashing (no write to LED, in this case)
 * Outputs:         None.
 * Side Effects:	Return when User Button is Released, optinally flash LED.
********************************/
void BlockUntilUserButtonRelease( int jLED ) {
  int CurrentUserButtonValue, PreviousUserButtonValue;

  CurrentUserButtonValue  = ReadUserButton();  PreviousUserButtonValue = CurrentUserButtonValue;

  while ( ! ((CurrentUserButtonValue == 0) && (PreviousUserButtonValue == 1)) ) {
		PreviousUserButtonValue = CurrentUserButtonValue;
		CurrentUserButtonValue  = ReadUserButton();
		FlashNUCLEO_LED( jLED );
	}
  WriteNUCLEO_LED(jLED, 0);  //  Leave LED in off condition
}  //  BlockUntilUserButtonRelease



/*********************************
 *  WriteOutBit   Put a 1 or 0 on an EE240 OutBit0, 1, 2 or 3
 *
 * Inputs:              jOutBit (0 .. 3), and jValue  (value to put on OutBit)
 * Outputs:             0 for Success, else -1
 * Side Effects:		OutBit(jOutBit)  bit is changed
********************************/
int WriteOutBit(int jOutBit, int jValue) {
	int ReturnValue = SUCCESS;
	switch( jOutBit) {
	case 0:
		HAL_GPIO_WritePin(OutBit0_GPIO_Port, OutBit0_Pin, jValue);
		break;
	case 1:
		HAL_GPIO_WritePin(OutBit1_GPIO_Port, OutBit1_Pin, jValue);
		break;
	case 2:
		HAL_GPIO_WritePin(OutBit2_GPIO_Port, OutBit2_Pin, jValue);
		break;
	case 3:
		HAL_GPIO_WritePin(OutBit3_GPIO_Port, OutBit3_Pin, jValue);
		break;
	default:
		ReturnValue = -1;
	}
	return ReturnValue;
}  // WriteOutBit


/*********************************
 *  ClearOutBits   Put a  0 on an EE240 OutBit0, 1, 2 and 3
 *
 * Inputs:              None
 * Outputs:             0 for Success, else -1
 * Side Effects:		All jOutBits set to zero
********************************/
int ClearOutBits( void ) {
  int jOutBit;
  int ReturnValue = SUCCESS;
  for (jOutBit = 0; jOutBit < 4; jOutBit++)
	ReturnValue |= WriteOutBit(jOutBit, 0);
  return ReturnValue;
} //  ClearOutBits




/*********************************
 *  ReadInBit   Put a 1 or 0 on an EE240 InBit0, 1, 2 or 3
 *
 * Inputs:              jInBit (0 .. 3)
 * Outputs:             0 for Success, else -1
 * Side Effects:		InBItx bit is changed
********************************/
int ReadInBit(int jInBit) {
	int ReturnValue;
	switch( jInBit) {
	case 0:
		ReturnValue=HAL_GPIO_ReadPin(InBit0_GPIO_Port, InBit0_Pin);
		break;
	case 1:
		ReturnValue=HAL_GPIO_ReadPin(InBit1_GPIO_Port, InBit1_Pin);
		break;
	case 2:
		ReturnValue=HAL_GPIO_ReadPin(InBit2_GPIO_Port, InBit2_Pin);
		break;
	case 3:
		ReturnValue=HAL_GPIO_ReadPin(InBit3_GPIO_Port, InBit3_Pin);
		break;
	default:
		ReturnValue = -1;
	}
	return ReturnValue;
}  // ReadInBit







