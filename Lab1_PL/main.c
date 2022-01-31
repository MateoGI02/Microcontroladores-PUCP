/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ *****/ 
/***** FACULTAD DE CIENCIAS E INGENIERÍA *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Introducción al entorno Keil uVision4 *****/ 
/***** Proyecto: Parpadeo 3 Leds *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: Septiembre 2021 *****/ 
/****************************************************************************************/ 
/***** Enunciado: Se genera un parpadeo en los leds RGB de color rojo, azul y verde. *****/ 
/***** *****/ 
/****************************************************************************************/

#include <stdint.h> 
#include "tm4c123gh6pm.h"
#define T 1000000

int main(void)
{
	uint32_t i; 
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // Activamos el Reloj de Puerto F 
	while( (SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)==0) { } // Se espera a que se active el reloj 
	GPIO_PORTF_DIR_R |= 0x0E; // Configurar los pines PF1, PF2 y PF3 como salidas 
	GPIO_PORTF_DR8R_R |=0x0E; // Se activa el driver de 8 mA en los pines PF1, PF2 y PF3. 
	GPIO_PORTF_DEN_R |=0x0E; // Se activa los pines PF1, PF2 y PF3 como salidas digitales. 
	while(1) 
		{ 
			GPIO_PORTF_DATA_R |=0x02; // Enciende led rojo PF1 
			for(i=0;i<=T;i++){}	//Retraso 1T 
			GPIO_PORTF_DATA_R ^=0x02; // Apaga led rojo PF1 
			GPIO_PORTF_DATA_R |=0x04; // Enciende led azul PF2 
			for(i=0;i<=2*T;i++){}  //Retraso 2T
			GPIO_PORTF_DATA_R ^=0x04; // Apaga led azul PF2
			GPIO_PORTF_DATA_R |=0x08; // Enciende led verde PF3 
			for(i=0;i<=3*T;i++){}  //Retraso 3T
			GPIO_PORTF_DATA_R ^=0x08; // Apaga led verde PF3
		}
}