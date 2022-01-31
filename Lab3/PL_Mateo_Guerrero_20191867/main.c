/*****************************************************************************
* Nombre: Lab3_PL
* Simulador: Periferico Texas Port F en keil uVision4 Ver. 4.74
* Descripción: PL Juego de cartas
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

int numero_carta(uint8_t numero_ingresado){
	uint8_t numero;
	if (numero_ingresado>39){
		numero=numero_ingresado-39;
	}else if (numero_ingresado>26){
		numero=numero_ingresado-26;
	}else if (numero_ingresado>13){
		numero=numero_ingresado-13;
	}else if ((numero_ingresado==13) || (numero_ingresado==26) || (numero_ingresado==39) || (numero_ingresado==52)){
		numero=13;
	}
	return numero;
}

int main(void) {
	uint8_t num1=0, num2=0, num3=0,a, intento=0; // Variable para contar parpadeos 
	uint32_t i; // Variable para retardos
	configPuertoF(); // Se configuran los pulsadores y leds a utilizar
	GPIO_PORTF_DATA_R &= ~0x0E; // Se apagan los leds inicialmente 
	while (1){
		if ((GPIO_PORTF_DATA_R & 0x11) == 0x01){ // ¿Se presionó el SW1? PF0
			while((GPIO_PORTF_DATA_R & 0x11) == 0x01); // Se espera a que se suelte el SW1 
			for(i = 0; i < 1000000; i++); //Retardo
			if((GPIO_PORTF_DATA_R & 0x11) == 0x10){ //¿se presionó SW2? PF0
				// Se espera que se suelte el SW2 
				while((GPIO_PORTF_DATA_R & 0x11) == 0x10){
					num1=num1+1;
					GPIO_PORTF_DATA_R |= ledVerde; 
					for(i = 0; i < 800000; i++); 
					GPIO_PORTF_DATA_R &= ~leds; 
					for(i = 0; i < 800000; i++);
				}
			}
			for(i = 0; i < 1000000; i++); //Retardo
			if((GPIO_PORTF_DATA_R & 0x11) == 0x10){ //¿se presionó SW2? PF0
				// Se espera que se suelte el SW2 
				while((GPIO_PORTF_DATA_R & 0x11) == 0x10){
					num2=num2+1;
					GPIO_PORTF_DATA_R |= ledAzul; 
					for(i = 0; i < 800000; i++); 
					GPIO_PORTF_DATA_R &= ~leds; 
					for(i = 0; i < 800000; i++);
				}
			}
			for(i = 0; i < 1000000; i++); //Retardo
			if((GPIO_PORTF_DATA_R & 0x11) == 0x10){ //¿se presionó SW2? PF0
				// Se espera que se suelte el SW2 
				while((GPIO_PORTF_DATA_R & 0x11) == 0x10){
					num3=num3+1;
				}
			}
			num1=numero_carta(num1);
			num2=numero_carta(num2);
			num3=numero_carta(num3);
			if (num1==num2){
				// Parpadeos del led rojo
				for(a=0; a<num3; a++){
				GPIO_PORTF_DATA_R |= ledRojo; 
				for(i = 0; i < 800000; i++); 
				GPIO_PORTF_DATA_R &= ~leds; 
				for(i = 0; i < 800000; i++);
				}
				num1=0;
				num2=0;
				num3=0;
				intento=intento+1;
			}else if (num1>num2){
				if((num3>num2) && (num1>num3)){
					// Parpadeos del led blanco
					for(a=0; a<num3; a++){
					GPIO_PORTF_DATA_R |= ledRojo | ledVerde | ledAzul; 
					for(i = 0; i < 800000; i++); 
					GPIO_PORTF_DATA_R &= ~leds; 
					for(i = 0; i < 800000; i++);
					}
					num1=0;
					num2=0;
					num3=0;
				}else{
					// Parpadeos del led rojo
					for(a=0; a<num3; a++){
					GPIO_PORTF_DATA_R |= ledRojo; 
					for(i = 0; i < 800000; i++); 
					GPIO_PORTF_DATA_R &= ~leds; 
					for(i = 0; i < 800000; i++);
					}
					num1=0;
					num2=0;
					num3=0;
					intento=intento+1;
				}
			} else if (num2>num1){
				if((num3>num1) && (num2>num3)){
					// Parpadeos del led blanco
					for(a=0; a<num3; a++){
					GPIO_PORTF_DATA_R |= ledRojo | ledVerde | ledAzul; 
					for(i = 0; i < 800000; i++); 
					GPIO_PORTF_DATA_R &= ~leds; 
					for(i = 0; i < 800000; i++);
					}
					num1=0;
					num2=0;
					num3=0;
				}else{
					// Parpadeos del led rojo
					for(a=0; a<num3; a++){
					GPIO_PORTF_DATA_R |= ledRojo; 
					for(i = 0; i < 800000; i++); 
					GPIO_PORTF_DATA_R &= ~leds; 
					for(i = 0; i < 800000; i++);
					}
					num1=0;
					num2=0;
					num3=0;
					intento=intento+1;
				} 
			}
		}
	}// Fin del while
}// Fin del main
