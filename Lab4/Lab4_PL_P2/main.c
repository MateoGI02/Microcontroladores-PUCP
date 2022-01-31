/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ *****/ 
/***** FACULTAD DE CIENCIAS E INGENIERÍA *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Laboratorio 4 *****/ 
/***** Proyecto: P2 Aleatorio *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: Septiembre 2020 *****/ 
/****************************************************************************************/ 
/***** Enunciado: Se genera un número aleatorio *****/ 
/***** *****/ 
/****************************************************************************************/

#include <stdint.h> 
#include "tm4c123gh6pm.h"

#define SW1 0x10
#define SW2 0x01

/* Función que configura los pines PF0 y PF4 conectados a SW2 y SW1 como entradas.
Entradas: Ninguna
Salidas: Ninguna
*/
void config_portF (void) {
	 // activamos la señal de reloj del puerto F
	 SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	 // espera a que el puerto esté operativo
	 while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)==0){}
	 GPIO_PORTF_LOCK_R = 0x4C4F434B; // Desbloquear puerto F
	 GPIO_PORTF_CR_R |= 0x01; // Se habilita PF0
	 
	 GPIO_PORTF_DIR_R &= ~(0x11); // Configura PF0 y PF4 como entradas
	 GPIO_PORTF_AFSEL_R &= ~(0x11); // No utilizar funciones alternas
	 GPIO_PORTF_PUR_R |= 0x11; // Activa resistencias de pull-up de PF0 y PF4
	 GPIO_PORTF_DEN_R |= 0x11; // Habilita señales digitales en PF0 y PF4
}

// Función principal
int main (void){	
	
	uint8_t aleatorio=0;
	
	//Configurar Puertos
	config_portF(); // configura el puerto F

	//Ciclo
	while(1){
		if ((GPIO_PORTF_DATA_R & 0x11) == 0x01){ // ¿Se presionó el SW1? PF0
			// Se espera a que se suelte el SW1 
			while((GPIO_PORTF_DATA_R & 0x11) == 0x01){
				aleatorio+=1;
				if(aleatorio==7){
					aleatorio=2;
				}
			}
		}
	}//Fin While
}
