/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ *****/ 
/***** FACULTAD DE CIENCIAS E INGENIERÍA *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Laboratorio 6 *****/ 
/***** Proyecto: Arreglo_Aletorio_Led *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: Octubre 2020 *****/ 
/****************************************************************************************/ 
/***** Enunciado: Se genera un arreglo con 4 elementos que varian aleatoriamente con el SW1 y el SW2 reconce por leds si la suma de
			 los elementos es par o impar*****/ 
/***** *****/ 
/****************************************************************************************/

#include <stdint.h> 
#include "tm4c123gh6pm.h"

#define ledRojo 0x02
#define ledAzul 0x04
#define ledVerde 0x08
#define leds 0x0E


/* Función que configura los pines PF0 y PF4 conectados a SW2 y SW1 como entradas
y los pines PF1, PF2, PF3 conectados a los LEDs Rojo, Azul y Verde como salidas.
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
	 GPIO_PORTF_DIR_R |= leds; // Configura PF1, PF2 y PF3 como salidas
	 GPIO_PORTF_DR8R_R |= leds; // Activa el driver de 8 mA en PF1, PF2 y PF3
	 GPIO_PORTF_DEN_R |= leds; // Habilita señales digitales en PF1, PF2 y PF3
	 GPIO_PORTF_DATA_R &=~(leds); // Apaga los LEDs conectados a PF1, PF2 y PF3
}

// ************** KEYS Inicialización *******************
/* Función que configura los pines PD0 al PD3 como entradas
Entradas: Ninguna
Salidas: Ninguna
*/
void KEYS_Init (void){
	 // activamos la señal de reloj del puerto D
	 SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
	 // espera a que el puerto esté operativo
	 while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R3)==0){}
	GPIO_PORTD_AFSEL_R &= ~0x0F;				// No utilizar funciones alternas
	GPIO_PORTD_AMSEL_R &= ~0x0F;				//No se requieren funciones analógicas
	GPIO_PORTD_PCTL_R  &= ~0x00FFFFFF;	//No se selecciona ninguna función alternativas
	GPIO_PORTD_DIR_R   &= ~0x0F;				//Configura como entradas
	GPIO_PORTD_DEN_R   |= 0x0F;					//Habilita la función digital sobre PD0 hasta PD3
}

//************ DAC Inicialización********
/* Función que configura los pines PB0 al PB5 como entradas
Entradas: Ninguna
Salidas: Ninguna
*/
void DAC_Init(void){
	 // activamos la señal de reloj del puerto B
	 SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	 // espera a que el puerto esté operativo
	 while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)==0){}
	GPIO_PORTB_AFSEL_R &= ~0x3F;    		//Desactia funciones alternativas PB0 hasta PB5
	GPIO_PORTB_AMSEL_R &= ~0x3F;				//No se requieren funciones analógicas
	GPIO_PORTB_PCTL_R  &= ~0x00FFFFFF;	//No se selecciona ninguna función alternativas
	GPIO_PORTB_DIR_R   |= 0x3F;					//Configura como salidas
	GPIO_PORTB_DR8R_R  |= 0x3F;					//Habilita el driver de 8mA sobre PB0 hasta PB5
	GPIO_PORTB_DEN_R   |= 0x3F;					//Habilita la función digital sobre PB0 hasta PB5
	GPIO_PORTB_DATA_R  &= ~0x3F;				//Apaga todo por defecto
}



// Función principal
int main (void){
	
	//Declaración variables a usar 
	/* Suma: valor de la suma de los elementos del arreglo
	key: valor del key seleccionado que ayuda a escoger el elemento del arreglo
	valido: valor que representa que todos los elementos del arreglo han sido variados aleatoriamente
	activo: valor que representa que una key ha sido seleccionada y activa el proceso aleatorio
	*/
	uint8_t suma, key, valido, activo; 
	
	//Variable de tiempo que genera retrasos
	uint32_t i;
	
	//Arrelo de 4 elementos, todos con un valor inicial de 1
	uint8_t arreglo[4]={1,1,1,1};
	
	//Configurar Puertos
	config_portF(); // configura el puerto F
	KEYS_Init();
	DAC_Init();
	
	//Valor inicial de ciertas variables
	//Restricción al SW2, primero se deben variar todos los elementos del arreglo
	valido=0;
	//Valor inicial de la suma
	suma=0;
	//Restricción al SW1, primero se debe seleccionar una key
	activo=0;
	
	while(1){ //Bucle infinito
		
		//************** Lectura de las KEYS ***************
		
		//Condicional KEYS
		if ((GPIO_PORTD_DATA_R & 0x0F) == 0x01){ // ¿Se presionó el Key0
			while((GPIO_PORTD_DATA_R & 0x0F) == 0x01){} // Se espera a que se suelte el Key0
			key=0; //Valor 0 de la key que selecciona el elemento 1 del arreglo
			activo=1;		//Se activa el SW1 Para la variación aleatoria
			//Led Azul parpadea validando que la KEY 0 fue seleccionada
			GPIO_PORTF_DATA_R |= ledAzul; 
			for(i = 0; i < 800000; i++); 
			GPIO_PORTF_DATA_R &= ~leds; 						
			for(i = 0; i < 800000; i++);
			//************************************
		} else if ((GPIO_PORTD_DATA_R & 0x0F) == 0x02){ // ¿Se presionó el Key1
			while((GPIO_PORTD_DATA_R & 0x0F) == 0x02){} // Se espera a que se suelte el Key1
			key=1;  //Valor 1 de la key que selecciona el elemento 2 del arreglo
			activo=1;		//Se activa el SW1 Para la variación aleatoria
			//Led Verde parpadea validando que la KEY 1 fue seleccionada
			GPIO_PORTF_DATA_R |= ledVerde; 
			for(i = 0; i < 800000; i++); 
			GPIO_PORTF_DATA_R &= ~leds;
			for(i = 0; i < 800000; i++);
			//****************************************
		} else if ((GPIO_PORTD_DATA_R & 0x0F) == 0x04){ // ¿Se presionó el Key2
			while((GPIO_PORTD_DATA_R & 0x0F) == 0x04){} // Se espera a que se suelte el Key2
			key=2; //Valor 2 de la key que selecciona el elemento 3 del arreglo
			activo=1;		//Se activa el SW1 Para la variación aleatoria
			//Led Rojo parpadea validando que la KEY 2 fue seleccionada
			GPIO_PORTF_DATA_R |= ledRojo; 
			for(i = 0; i < 800000; i++); 
			GPIO_PORTF_DATA_R &= ~leds;
			for(i = 0; i < 800000; i++);
			//********************************************
		} else if ((GPIO_PORTD_DATA_R & 0x0F) == 0x08){ // ¿Se presionó el Key3
			while((GPIO_PORTD_DATA_R & 0x0F) == 0x08){} // Se espera a que se suelte el Key3
			key=3; //Valor 3 de la key que selecciona el elemento 4 del arreglo
			activo=1;		//Se activa el SW1 Para la variación aleatoria
			//Led Blanco parpadea validando que la KEY 3 fue seleccionada
			GPIO_PORTF_DATA_R |= ledAzul | ledVerde |  ledRojo; 
			for(i = 0; i < 800000; i++); 
			GPIO_PORTF_DATA_R &= ~leds;
			for(i = 0; i < 800000; i++);
			//**************************************************
		}
		
		if (((GPIO_PORTF_DATA_R & 0x11) == 0x01) && (activo==1)){ // ¿Se presionó el SW1, PF0 y se presiono antes un KEY?
				while((GPIO_PORTF_DATA_R & 0x11) == 0x01){ // Mientras no se suelte el SW1 se realiza el NUMERO ALEATORIO
					//************* NUMERO ALEATORIO DEL ARREGLO[KEY] ********
					arreglo[key]+=1;
					if (arreglo[key]==10){  //Restricción para que el valor máximo sea 9
						arreglo[key]=1;
					}
				}
				GPIO_PORTB_DATA_R = arreglo[key];  //El valor del numero aleatorio resultante se muestra en el Puerto B (binario) DACOUT
				activo=0;  //Se inabilita el acceso al SW1 hasta presionar la siguiente KEY
				valido+=1;	//Se confirma que se ha realizado 1 variación en los elementos del arreglo
				suma+=arreglo[key];		//Se suma el valor aleatorio a la suma
				//Led de Confirmación que se activo el SW1 y se vario aleatoriamente el valor de un elemento del arreglo
				GPIO_PORTF_DATA_R |= ledAzul | ledRojo; 
				for(i = 0; i < 800000; i++); 
				GPIO_PORTF_DATA_R &= ~leds;
				for(i = 0; i < 800000; i++);
				//*******************************************************************
		}
		 
		if(((GPIO_PORTF_DATA_R & 0x11) == 0x10) && (valido==4) && (suma%2==0)){ //¿se presionó SW2 PF4 y ya se activaron 4 elementos del arreglo y la suma es par?
			// Se espera que se suelte el SW2 
			while((GPIO_PORTF_DATA_R & 0x11) == 0x10){ //Ciclo mientras se tiene presionado el SW2
				//*********** Parpadeo Led Verde por suma par
				GPIO_PORTF_DATA_R |= ledVerde; 
				for(i = 0; i < 800000; i++); 
				GPIO_PORTF_DATA_R &= ~leds; 						
				for(i = 0; i < 800000; i++);
				//****************************************************
			}
			//Restricción del ingreso al SW2 y se reinicia el valor de la suma
			valido=0;
			suma=0;
		}else if (((GPIO_PORTF_DATA_R & 0x11) == 0x10) && (valido==4) && (suma%2!=0)){ //¿se presionó SW2 PF4 y ya se activaron 4 elementos del arreglo y la suma es impar?
			// Se espera que se suelte el SW2 
			while((GPIO_PORTF_DATA_R & 0x11) == 0x10){ //Ciclo mientras se tiene presionado el SW2
				//*********** Parpadeo Led Rojo por suma par
				GPIO_PORTF_DATA_R |= ledRojo; 
				for(i = 0; i < 800000; i++); 
				GPIO_PORTF_DATA_R &= ~leds; 						
				for(i = 0; i < 800000; i++);
				//****************************************
			}
			//Restricción del ingreso al SW2 y se reinicia el valor de la suma
			valido=0;
			suma=0;
		}
	}
}
