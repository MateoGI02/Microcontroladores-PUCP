/*****************************************************************************
* Nombre: Informe Previo
* Simulador: Periferico Texas Port F en keil uVision4 Ver. 4.74
* Descripción: Se muestran dos secuencias de parpadeos, las cual se
* muestran al activar SW1 y SW2 respectivamente
* Fecha: Semestre 2020-2
* Autores: Mateo Guerrero Isuiza
******************************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define ledRojo 0x02
#define ledAzul 0x04
#define ledVerde 0x08
#define leds 0x0E

/******* Configuración del Puerto F **********/
void configPuertoF(void) {
	// Se habilita la señal de reloj del Puerto F 
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	// Espera a que se active
	while( (SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)==0) {} 
	GPIO_PORTF_LOCK_R = 0x4C4F434B; // Se desbloquea el Puerto F 
	GPIO_PORTF_CR_R |= 0x01; // Se habilita el PF0
	
	GPIO_PORTF_DIR_R &= ~0x11; // PF0 y PF4 como entradas
	GPIO_PORTF_PUR_R |= 0x11; // Resistencias pull-up de PF0 y PF4
	
	GPIO_PORTF_DIR_R |= 0x0E; // PF1, PF2 y PF3 como salidas
	GPIO_PORTF_AFSEL_R &= ~0x1F; // Se desactivan las funciones alternas
	
	GPIO_PORTF_DEN_R |= 0x1F;
}


int main(void) {
	uint8_t numero,palo,a, num_ingresado,pal_ingresado; // Variable para contar numero 
	uint32_t i; // Variable para retardos
	
	configPuertoF(); // Se configuran los pulsadores y leds a utilizar
		
	GPIO_PORTF_DATA_R &= ~0x0E; // Se apagan los leds inicialmente 
	
	while (1){
		numero=1;
		palo=0;
		num_ingresado=0;
		pal_ingresado=0;
		GPIO_PORTF_DATA_R |= ledRojo; 
						for(i = 0; i < 800000; i++); 
						GPIO_PORTF_DATA_R &= ~leds; 
						for(i = 0; i < 800000; i++);
		do{
			if (((GPIO_PORTF_DATA_R & 0x11) == 0x01) && (pal_ingresado==0)){ // ¿Se presionó el SW1? PF0
				while((GPIO_PORTF_DATA_R & 0x11) == 0x01){
					numero=numero+1;
					num_ingresado=1;
					GPIO_PORTF_DATA_R |= ledRojo; 
						for(i = 0; i < 800000; i++); 
						GPIO_PORTF_DATA_R &= ~leds; 
						for(i = 0; i < 800000; i++);
				}
			}	
			else if ((num_ingresado==1) && ((GPIO_PORTF_DATA_R&0x11)==0x00) ) {
					while(((GPIO_PORTF_DATA_R&0x10)==0x00) && ((GPIO_PORTF_DATA_R&0x01)==0x00)){
							palo=palo+1;
							pal_ingresado=1;
						GPIO_PORTF_DATA_R |= ledVerde; 
						for(i = 0; i < 800000; i++); 
						GPIO_PORTF_DATA_R &= ~leds; 
						for(i = 0; i < 800000; i++);
						}
			}
			else if((pal_ingresado==1) && (num_ingresado==1)){
				if (numero>13){
							if (numero%13==0){
								numero=13;
							}else{
								numero=numero%13;
							}
						}
				if (palo>3){
					if (palo%3==0){
							palo=3;
					}else{
						palo=palo%3;
					}
				}
				if ((palo==0) || (palo ==1)){
					// Parpadeos del led Magenta 
					for(a=0; a<numero; a++){
						GPIO_PORTF_DATA_R |= ledRojo | ledAzul; 
						for(i = 0; i < 800000; i++); 
						GPIO_PORTF_DATA_R &= ~leds; 
						for(i = 0; i < 800000; i++);
						}
				}else{
						// Parpadeos del led Blanco
					for(a=0; a<numero; a++){
						GPIO_PORTF_DATA_R |= ledRojo | ledVerde | ledAzul; 
						for(i = 0; i < 800000; i++); 
						GPIO_PORTF_DATA_R &= ~leds; 
						for(i = 0; i < 800000; i++);
					}
				}		
			}	
		}while ((num_ingresado==0) && (pal_ingresado==0));
	}// Fin del while
}// Fin del main
//{}
