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


uint8_t numero=0;

uint16_t milisegundos=0;

/*********************************************************************************************** 
//definimos los posibles estados de un pulsador 
************************************************************************************************/
const uint16_t pulsadorNoPresionado=1;
const uint16_t pulsadorTalvezPresionado=2;
const uint16_t pulsadorPresionado=3;
const uint16_t pulsadorTalvezNoPresionado=4;
//inicialmente el pulsador no esta presionado
uint16_t estadoPulsador= pulsadorNoPresionado;
//*********************************************************************

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
uint8_t cambiarEstadoPF1=0;
//**********************************************************************************************

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

/*Funcion para el envio de un número al UART*/
/*Entradas: número:numero que se mostrará en el UART
Salidas: Ninguna  
*/
void EnviarNumero( uint8_t numero )
{
	switch (numero){
		case 1:
			Enviar_caracter('1');		//Se envia el número 1
			break;
		case 2:
			Enviar_caracter('2');			//Se envia el número 2
			break;
		case 3:
			Enviar_caracter('3');			//Se envia el número 3
			break;
		case 4:
			Enviar_caracter('4');			//Se envia el número 4
			break;	
		case 5:
			Enviar_caracter('5');			//Se envia el número 5
			break;
		case 6:
			Enviar_caracter('6');			//Se envia el número 6
			break;
		case 7:
			Enviar_caracter('7');			//Se envia el número 7
			break;
		case 8:
			Enviar_caracter('8');			//Se envia el número 8
			break;
		case 9:
			Enviar_caracter('9');			//Se envia el número 9
			break;
		default:
			Enviar_caracter('0');			//Se envia el número 0
			break;
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
void SW1_presionado(){
 switch(estadoPulsador){
/********************************************************************************************** 
*Cuando el pulsador se encuentra en estado "no presionado",se pregunta si se ha presionado 
*Si la respuesta es afirmativa, entonces el estado cambia a "tal vez presionado" 
*si no se cumple la condición, se mantiene en estado "no presionado" 
**********************************************************************************************/
 case pulsadorNoPresionado:
 
	 if((GPIO_PORTF_DATA_R & 0x11) == 0x01){ 
		estadoPulsador=pulsadorTalvezPresionado;
	 } else {
		estadoPulsador=pulsadorNoPresionado;
	 }
	 break;

/********************************************************************************************** 
*Cuando el pulsador se encuentra en estado "Tal vez presionado",se pregunta si se ha presionado 
*Si se cumple la condición, entonces el estado cambia a "presionado"y se incrementa la variable 
*cambiarEstadoPF1, indicando que se ha presionado el pulsador. En ese momento valdría "1" 
*Si no se cumple la condición, pasa a estado "pulsadorNoPresionado" 
**********************************************************************************************/
 case pulsadorTalvezPresionado:
 
	 if((GPIO_PORTF_DATA_R & 0x11) == 0x01) {
		estadoPulsador=pulsadorPresionado; 
		cambiarEstadoPF1++;
	 } else {
		estadoPulsador=pulsadorNoPresionado;
	 }
	 break;
 
/********************************************************************************************** 
*Cuando el pulsador se encuentra en estado "presionado",se pregunta si se ha presionado 
*Si se cumple la condición, se mantiene en estado "pulsador presionado" 
*Si no se cumple la condición, pasa a estado "Tal vez no presionado" 
**********************************************************************************************/
 case pulsadorPresionado:
 
	 if((GPIO_PORTF_DATA_R & 0x11) == 0x01) {
		estadoPulsador=pulsadorPresionado;
	 }else {
		estadoPulsador=pulsadorTalvezNoPresionado;
	 }
	 break;
 
/********************************************************************************************** 
*Cuando el pulsador se encuentra en estado "Tal vez no presionado",se pregunta si se ha presionado 
*Si se cumple la condición, entonces el estado cambia a "presionado" y se incrementa la variable 
*cambiarEstadoPF1, indicando que se ha soltado luego de haber estado presionado. En ese momento 
*la variable valdría 2 
*Si no se cumple la condición, pasa al estado "no presionado" 
**********************************************************************************************/
 case pulsadorTalvezNoPresionado:
 
	 if((GPIO_PORTF_DATA_R & 0x11) == 0x01) {
		estadoPulsador=pulsadorPresionado;
	 }else {
		estadoPulsador=pulsadorNoPresionado;
		cambiarEstadoPF1++;
	 }
	 break;
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



// Función principal
int main (void){
	
	//Declaración variables a usar 
	//Variable de tiempo que genera retrasos
	uint8_t mInicio[] = "Presione SW1 o SW2 para reiniciar el cronometro.";
	uint8_t mEspacio[] = "\n\r";
	uint8_t mSW2[] = "\t //SW2 presionado \n\r";
	uint8_t mSW1[] = "\t //SW1 presionado \n\r";
	
	//Configurar Puertos
	ConfigPuertoF(); // configura el puerto F
	//Configurar UART0
	ConfigUART0(19200);
	
	ConfiguraTimer_1ms();
	
	
	while(1){ //Bucle infinito
		//Condicional que imprime el primer valor y cada que pasa 1 segundo
		if (milisegundos==0){ //¿Paso 1 segundo o se inicio el programa?
				Enviar_cadena(mInicio); //Mensaje de presionar algun SW
				if (cambiarEstadoPF2==2){  	//Si se presiono el SW2 
					cambiarEstadoPF2=0; // Se reinicia el valor que lee el SW2
					numero=5;  	//Se cambia el numero a 5
					Enviar_cadena(mSW2);	//Se imprime el mensaje de se presiono SW2
				} else if (cambiarEstadoPF1==2){ //Si se presiono el SW1
					cambiarEstadoPF1=0;		// Se reinicia el valor que lee el SW1
					numero=0;  	//Se cambia el numero a 0
					Enviar_cadena(mSW1);	//Se imprime el mensaje de se presiono SW1
				} else {  //No se presiono ningun SW
					Enviar_cadena(mEspacio);		//Se imprime un espacio 
				}
				EnviarNumero(numero);  //Se muestra el numero
				//Se imprimen los espacios
				Enviar_cadena(mEspacio);
				Enviar_cadena(mEspacio);
				//*******************
				numero++; //Se aumenta el valor del numero
				//Condicional que corrige el rango del numero
				if (numero>9){
					numero=0;
				}
				//*****************
		}
		//Actualiza el valor del tiempo en milisegundos
		actualizartTiempo();
		//se leen los valores de los SW
		SW1_presionado();
		SW2_presionado();
		//***********************
	}
}
