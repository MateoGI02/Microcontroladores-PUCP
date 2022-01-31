/***************************************************
	Librería creada para el almacenamiento e 
	invocación de instrucciones para el correcto 
	funcionamiento de la terminal serial UART.
	
	Características:
	38400 bps
	8 bits de dato
	1 bit  de parada
	paridad impar
***************************************************/

#ifndef UART_H
#define UART_H
#include <stdint.h>
#include "tm4c123gh6pm.h"


//------------ConfigUART0------------
// Initialization for UART serial.
// Input: none
// Output: none
void ConfigUART0(void);


//------------------HayCarRx------------------
// Función que verifica el estado de la pila 
// de recepción
// Input: none
// Output:  0 	pila vacía
// 			1	hay caracter recibido
uint8_t HayCarRx( void );


//--------------------RxCar-------------------
// Función que espera hasta recibir 
// un caracter
// la pila de recepción
// Input: none
// Output: Caracter recibido
uint8_t RxCar( void );


//-------------------TxCar--------------------
// Función que transmite un caracter
// Input: 	Car 	Caracter en ASCII
// Output:  none
void TxCar( uint8_t Car );


//------------------TxCadena------------------
// Funcion que transmite una cadena 
// ASCII-Z
// Input:  Cadena[] 	Cadena de caracteres
// Output: none
void TxCadena( uint8_t Cadena[] );

#endif

