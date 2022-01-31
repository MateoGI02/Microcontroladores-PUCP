/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ *****/ 
/***** FACULTAD DE CIENCIAS E INGENIERÍA *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Laboratorio 8 *****/ 
/***** Proyecto: IP_Cronometro *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: 11 Noviembre 2020 *****/ 
/****************************************************************************************/ 
/***** Enunciado: Se genera un cronometro controlado por SW1 y SW2*****/ 
/***** *****/ 
/****************************************************************************************/


#include <stdint.h> 
#include "tm4c123gh6pm.h"


uint8_t numero;

uint16_t milisegundos=0;
uint8_t ingreso_dato=0;

//definimos los posibles estados de un pulsador 
//************************************************************************************************/
const uint16_t pulsador2NoPresionado=1;
const uint16_t pulsador2TalvezPresionado=2;
const uint16_t pulsador2Presionado=3;
const uint16_t pulsador2TalvezNoPresionado=4;
//inicialmente el pulsador no esta presionado
uint16_t estadoPulsador2= pulsador2NoPresionado;
//*********************************************************************



//Definimos una bandera que nos indica que el pulsador fue presionado y soltado 
//Cuando su valor es "2" indica que se presionó y soltó 
//************************************************************************************************
uint8_t cambiarEstadoPF2=0;
//**********************************************************************************************

#define leds 0x0E


/* Función que configura los pines PF0 y PF4 conectados a SW2 y SW1 como entradas
y los pines PF1, PF2, PF3 conectados a los LEDs Rojo, Azul y Verde como salidas.
Entradas: Ninguna
Salidas: Ninguna
*/
void ConfigPuertoF( void )
{ 
	 SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOF;
	 SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOF;
	 SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOF;
	 SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOF;
	
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
//***************************************************************************

/* Función que configura el pin PB5 como salida.
Entradas: Ninguna
Salidas: Ninguna
*/
void ConfigPuertoB( void )
{ 
	 SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOB;
	 SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOB;
	 SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOB;
	 SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOB;
	
	 GPIO_PORTB_LOCK_R = 0x4C4F434B; // Desbloquear puerto B
	 GPIO_PORTB_CR_R |= 0x01; // Se habilita PB0
	 
	 GPIO_PORTB_DIR_R |= 0x20; // Configura PB5 como salida
	 GPIO_PORTB_DR8R_R |= 0x20; // Activa el driver de 8 mA en PB5
	 GPIO_PORTB_DEN_R |= 0x20; // Habilita señal digital en PB5
	 GPIO_PORTB_DATA_R &=~(0x20); // Pone a 0 a PB5
}
//***************************************************************************

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Funcion de configuración del UART0
/*Entradas: bps: parámetro
Salidas: Ninguna
*/
void ConfigUART0( uint32_t bps )
{ 
	SYSCTL_RCGC1_R |= SYSCTL_SRCR1_UART0; //Habilitamos reloj para el UART2
	SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOA; //Habilitamos reloj para GPIOD
	UART0_CTL_R &= ~UART_CTL_UARTEN; //Se desactiva el UART
	UART0_IBRD_R &= ~0xFFFF; //
	UART0_IBRD_R |= 1000000UL/bps; //Parte entera
	UART0_FBRD_R &= ~0x3F; //
	UART0_FBRD_R |= ((1000000UL%bps)*128 + bps)/2/bps; //Parte decimal
	//UART0_LCRH_R &= ~UART_LCRH_SPS; //stick parity
	UART0_LCRH_R &= ~UART_LCRH_WLEN_M;
	UART0_LCRH_R |= UART_LCRH_WLEN_8; //8 bits de dato
	UART0_LCRH_R |= UART_LCRH_FEN; //activa uso de pila FIFO
	UART0_LCRH_R &= UART_LCRH_STP2; //2 bits de parada
	UART0_LCRH_R |= UART_LCRH_EPS; //paridad impar
	UART0_LCRH_R |= UART_LCRH_PEN; //con paridad
	UART0_CTL_R |= UART_CTL_RXE; //Habilita Rx
	UART0_CTL_R |= UART_CTL_TXE; //Habilita Tx
	UART0_CTL_R &= ~UART_CTL_LBE; //Deshabilita loopback
	UART0_CTL_R &= ~UART_CTL_HSE; //Deshabilita high speed
	UART0_CTL_R |= UART_CTL_UARTEN; //Habilita UART
	GPIO_PORTA_DIR_R |= (1<<1); //PA1 como salida
	GPIO_PORTA_DIR_R &= ~(1<<0); //PA0 como entrada
	GPIO_PORTA_AMSEL_R &= ~((1<<1)|(1<<0)); //Deshabilita funciones analógicas
	GPIO_PORTA_AFSEL_R |= ((1<<1)|(1<<0)); //Activa función alternativa
	GPIO_PORTA_PCTL_R &= ~((0xF<<(1*4)) | (0xF<<(0*4)));
	GPIO_PORTA_PCTL_R |= ((0x1<<(1*4)) | (0x1<<(0*4))); //U0Rx, U0Tx
	GPIO_PORTA_DEN_R |= ((1<<1)|(1<<0)); //Activa funciones digitales
} 
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/********************************************************************************************** 
*ConfiguraTimer_1ms(): Función para configurar el timer a 1ms. 
*Establecemos un tiempo base de 1ms para sondear el timer.Esto resulta conveniente cuando 
*queremos sondear varias tareas que se ejecutan a tiempos distintos, por lo que no 
*tendremos que modificar el timer 
************************************************************************************************/
void ConfiguraTimer_1ms(void)
 {
 /*Inhabilitamos el módulo SysTick*/
 NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
 /* TiempoDeseado=1ms; FreqCPU=16MHZ 
 * valor que debe ir en el registro RELOAD=X 
 * X=(TiempoDeseado)*(FreqCPU) -1 
 * X= [[100*10^(-3)] * [16*10^(6)]] -1 =15999
 *159999 en hexadecimal es 3E7F*/
 NVIC_ST_RELOAD_R = (NVIC_ST_RELOAD_R&0xFF000000)|0x00003E7F;
 
 // Iniciamos el contador con cero (escribiendo cualquier valor)
 NVIC_ST_CURRENT_R &= ~(0x00FFFFFF);
 // Habilitamos el módulo SysTick
 NVIC_ST_CTRL_R |= (NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE);
}

/*Función para la transmisión de un caracter*/
/*Entradas: car: caracter que se muestra en la pantalla
Salidas: Ninguna
*/
void Enviar_caracter(uint32_t car){ 
 while ((UART0_FR_R & UART_FR_TXFF)!=0); //Espera que esté disponible para tx
 UART0_DR_R = car; 	//Se envia el caracter car
}

/*Funcion para el envío de una cadena*/
/*Entradas: mens[]:arreglo con mensaje deseado para mostrar en el UART
Salidas: Ninguna  
*/
void Enviar_cadena( uint8_t mens[] )
{
 uint8_t letra;
 uint8_t i = 0;
 letra = mens[ i++ ]; 
 while( letra != '\0' ){
	//Se envían todos los caracteres hasta el fin de cadena
	Enviar_caracter( letra ); 
	letra = mens[ i++ ]; 
 } 
}

/*********************************************************************************************** 
*actualizartTiempo(): Función que actualiza el contador para cada tarea 
************************************************************************************************/
void actualizartTiempo(){
 //preguntamos si paso 1mss
 if((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT)){ 
		milisegundos++;
 }
 if (milisegundos==1000){
	 milisegundos=0;
 }
}

/*Funcion lectura del SW2*/
/*Entradas: Ninguna
Salidas: Ninguna  
*/
void SW2_presionado(){
 switch(estadoPulsador2){
/********************************************************************************************** 
*Cuando el pulsador se encuentra en estado "no presionado",se pregunta si se ha presionado 
*Si la respuesta es afirmativa, entonces el estado cambia a "tal vez presionado" 
*si no se cumple la condición, se mantiene en estado "no presionado" 
**********************************************************************************************/
 case pulsador2NoPresionado:
 
	 if((GPIO_PORTF_DATA_R & 0x11) == 0x10){ 
		estadoPulsador2=pulsador2TalvezPresionado;
	 } else {
		estadoPulsador2=pulsador2NoPresionado;
	 }
	 break;

/********************************************************************************************** 
*Cuando el pulsador se encuentra en estado "Tal vez presionado",se pregunta si se ha presionado 
*Si se cumple la condición, entonces el estado cambia a "presionado"y se incrementa la variable 
*cambiarEstadoPF1, indicando que se ha presionado el pulsador. En ese momento valdría "1" 
*Si no se cumple la condición, pasa a estado "pulsadorNoPresionado" 
**********************************************************************************************/
 case pulsador2TalvezPresionado:
 
	 if((GPIO_PORTF_DATA_R & 0x11) == 0x10) {
		estadoPulsador2=pulsador2Presionado; 
		cambiarEstadoPF2++;
	 } else {
		estadoPulsador2=pulsador2NoPresionado;
	 }
	 break;
 
/********************************************************************************************** 
*Cuando el pulsador se encuentra en estado "presionado",se pregunta si se ha presionado 
*Si se cumple la condición, se mantiene en estado "pulsador presionado" 
*Si no se cumple la condición, pasa a estado "Tal vez no presionado" 
**********************************************************************************************/
 case pulsador2Presionado:
 
	 if((GPIO_PORTF_DATA_R & 0x11) == 0x10) {
		estadoPulsador2=pulsador2Presionado;
	 }else {
		estadoPulsador2=pulsador2TalvezNoPresionado;
	 }
	 break;
 
/********************************************************************************************** 
*Cuando el pulsador se encuentra en estado "Tal vez no presionado",se pregunta si se ha presionado 
*Si se cumple la condición, entonces el estado cambia a "presionado" y se incrementa la variable 
*cambiarEstadoPF1, indicando que se ha soltado luego de haber estado presionado. En ese momento 
*la variable valdría 2 
*Si no se cumple la condición, pasa al estado "no presionado" 
**********************************************************************************************/
 case pulsador2TalvezNoPresionado:
 
	 if((GPIO_PORTF_DATA_R & 0x11) == 0x10) {
		estadoPulsador2=pulsador2Presionado;
	 }else {
		estadoPulsador2=pulsador2NoPresionado;
		cambiarEstadoPF2++;
	 }
	 break;
 }
}

/* Función que configura los pines PF0 y PF4 conectados a SW2 y SW1 como entradas
y los pines PF1, PF2, PF3 conectados a los LEDs Rojo, Azul y Verde como salidas.
Entradas: Ninguna
Salidas: Ninguna
*/
int validar_numero( uint8_t letra ){ 
	 if (letra=='2'){
		 numero=2;
		 return 1;
	 } else if (letra=='4'){
		 numero=4;
		 return 1;
	 } else if (letra=='5'){
		 numero=5;
		 return 1;
	 }else if (letra=='6'){
		 numero=6;
		 return 1;
	 } else if (letra=='8'){
		 numero=8;
		 return 1;
	 } else {
		return 0;
	 }
}
//***************************************************************************

/* Función que genera una señal en el PB5.
Entradas: Ninguna
Salidas: Ninguna
*/
void genera_senal( void )
{ 
	if (cambiarEstadoPF2==2){
		milisegundos=0;
		while ((milisegundos<((1000/3)*0.8))){ 
			GPIO_PORTB_DATA_R |= 0x20;
			//Actualiza el valor del tiempo en milisegundos
			actualizartTiempo();
		}
		milisegundos=0;
		while (milisegundos<(((1000/3)*0.2))){ 
			GPIO_PORTB_DATA_R &= ~0x20;
			//Actualiza el valor del tiempo en milisegundos
			actualizartTiempo();
		}
		milisegundos=0;
	} else{
		milisegundos=0;
		while ((milisegundos<(((1000/numero)*0.2)))){ 
			GPIO_PORTB_DATA_R |= 0x20;
			//Actualiza el valor del tiempo en milisegundos
			actualizartTiempo();
		}
		milisegundos=0;
		while ((milisegundos<(((1000/numero)*0.8)))){ 
			GPIO_PORTB_DATA_R &= ~0x20;
			//Actualiza el valor del tiempo en milisegundos
			actualizartTiempo();
		}
		milisegundos=0;
	}
	
}
//***************************************************************************

// Función principal
int main (void){
	
	//Declaración variables a usar 
	//Variable de tiempo que genera retrasos
	uint8_t mInicio[] = " Ingrese la cantidad de ciclos\n\r";
	uint8_t mEspacio[] = "\n\r";
	uint8_t i, ingreso_dato, letra;
	
	//Configurar Puertos
	ConfigPuertoF(); // configura el puerto F
	ConfigPuertoB(); // configura el puerto B
	//Configurar UART0
	ConfigUART0(19200);
	
	ConfiguraTimer_1ms();
	
	uint8_t inicio=1;
	
	while(1){ //Bucle infinito
		if (inicio){
			Enviar_cadena(mInicio);
			inicio=0;
		} else if (ingreso_dato){ //¿se ingreso un número valido?
			Enviar_caracter(letra);
			Enviar_cadena(mEspacio);
			Enviar_cadena(mEspacio);
			for(i=0;i<numero;i++){
				genera_senal();
			}
			ingreso_dato=0;
			inicio=1;
		} else if (cambiarEstadoPF2==2){  	//Si se presiono el SW2 
					for (i=0;i<3;i++){
						genera_senal();
					}
					cambiarEstadoPF2=0;
		}
		//se leen los valores del SW2
		SW2_presionado();
		//***********************
		//Lectura del caracter recibido en el UART
		letra = UART0_DR_R&0xFF;
		ingreso_dato=validar_numero(letra);
		//**************
	}
}
