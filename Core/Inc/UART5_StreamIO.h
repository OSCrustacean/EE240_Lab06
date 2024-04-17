/*
 * UART5_StreamIO.h
 *  Interface for UART5 Stream IO between Assessment and Student computers
 *  Created on: Mar 24, 2023
 *      Author: bsra
 */

#ifndef INC_UART5_STREAMIO_H_
#define INC_UART5_STREAMIO_H_

#define nUART5_Buffer_Characters  1024


// Function declarations
FILE * UART5_OpenInStream(void);
FILE * UART5_OpenOutStream(void);
int UART5_CloseOutStream(FILE *pFileOut);


int  Start_UART5_Reception(void);
int  Stop_UART5_Reception(void);

int BlockingTransmit_cBufferOut( void );

int SetLED_ForReception( int jLED );
void UnsetLED_ForReception(void);

void LoadTestDataIntoUART5_Buffers();
void Initialize_cBufferIn( char ACharacter);
void Initialize_cBufferOut( char ACharacter);


#endif /* INC_UART5_STREAMIO_H_ */
