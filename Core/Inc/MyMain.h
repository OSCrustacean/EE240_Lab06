/*
 * MyMain.h
 *
 *  Interfaces, for MyMain.c
 */

#ifndef _MYMAIN_H_
#define _MYMAIN_H_

// Indicates whether an HD44780 is attached
#undef  HD44780_Is_Present

// Function interface declarations
int MyMain(void);
int DemonstrateSerialIO(void);


#endif /* _MYMAIN_H_ */
