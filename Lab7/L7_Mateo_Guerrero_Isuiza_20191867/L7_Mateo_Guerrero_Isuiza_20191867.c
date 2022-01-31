/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ *****/ 
/***** FACULTAD DE CIENCIAS E INGENIERÍA *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Laboratorio 7 *****/ 
/***** Proyecto: LD_Ludo *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: 04 Noviembre 2020 *****/ 
/****************************************************************************************/ 
/***** Enunciado: Se realiza un juego de ludo*****/ 
/***** *****/ 
/****************************************************************************************/

//Incluir librerias
#include <stdint.h> 
#include "tm4c123gh6pm.h"
#include <stdlib.h>

//Definir los Leds
#define ledRojo 0x02
#define ledAzul 0x04
#define ledVerde 0x08
#define leds 0x0E

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
	UART0_LCRH_R &= ~UART_LCRH_STP2; //1 bit de parada
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

/*Función para la transmisión de un caracter*/
/*Entradas: car: caracter que se muestra en la pantalla
Salidas: Ninguna
*/
void Imprime_Letra(uint32_t car){ 
 while ((UART0_FR_R & UART_FR_TXFF)!=0); //Espera que esté disponible para tx
 UART0_DR_R = car; 	//Se envia el caracter car
} 

/*Función para la recepción de un carácter de 8 bits*/ 
/*Entradas: Ninguna
Salidas: temp:caracter recibido en el UART0
*/
uint8_t Leer_Letra( void ){
	uint8_t temp; //carácter leido
	while ((UART0_FR_R & UART_FR_RXFE)!=0); // Se espera que llegue un dato
	temp= UART0_DR_R&0xFF; // Se toman solo 8 bits
	return temp; //Se devuelve el dato
} 

/*Funcion para el envío de una cadena*/
/*Entradas: mens[]:arreglo con mensaje deseado para mostrar en el UART
Salidas: Ninguna  
*/
void Enviar_Cadena( uint8_t mens[] )
{
 uint8_t letra;
 uint8_t i = 0;
 letra = mens[ i++ ]; 
 while( letra != '\0' ){
	//Se envían todos los caracteres hasta el fin de cadena
	Imprime_Letra( letra ); 
	letra = mens[ i++ ]; 
 } 
}

/*Funcion para el envio de un número al UART*/
/*Entradas: número:numero que se mostrará en el UART
Salidas: Ninguna  
*/
void ImprimirNumero( uint8_t numero )
{
	switch (numero){
		case 1:
			Imprime_Letra('1');		//Se envia el número 1
			break;
		case 2:
			Imprime_Letra('2');			//Se envia el número 2
			break;
		case 3:
			Imprime_Letra('3');			//Se envia el número 3
			break;
		case 4:
			Imprime_Letra('4');			//Se envia el número 4
			break;	
		case 5:
			Imprime_Letra('5');			//Se envia el número 5
			break;
		default:
			Imprime_Letra('6');	    //Se envia el número 6
			break;
	}	  
}

/*Funcion para el envío del progreso del juego*/
/*Entradas: mens[]:arreglo con mensaje deseado para mostrar en el UART, numero: suma de los dados obtenida
Salidas: Ninguna  
*/
void Imprimir_Juego1( uint8_t mens[] , uint8_t numero )
{
 mens[numero-1]='X'; //Se cambia el "_" por X según corresponda por los dados
 uint8_t letra;
 uint8_t i = 0;
 letra = mens[ i++ ]; 
 while( letra != '\0' ){
	//Se envían todos los caracteres hasta el fin de cadena
	Imprime_Letra( letra ); 
	letra = mens[ i++ ]; 
 }
 mens[numero-1]='_';  //Se devuelve el elemento cambiado a X a su valor original '_'
}

/*Funcion para el envío del progreso del juego*/
/*Entradas: mens[]:arreglo con mensaje deseado para mostrar en el UART, numero: suma de los dados obtenida
Salidas: Ninguna  
*/
void Imprimir_Juego2( uint8_t mens[], uint8_t numero )
{
 mens[numero-1]='O';	//Se cambia el "_" por X según corresponda por los dados
 uint8_t letra;
 uint8_t i = 0;
 letra = mens[ i++ ]; 
 while( letra != '\0' ){
	//Se envían todos los caracteres hasta el fin de cadena
	Imprime_Letra( letra ); 
	letra = mens[ i++ ]; 
 } 
 mens[numero-1]='_'; 	//Se devuelve el elemento cambiado a X a su valor original '_'
}


// Función principal
int main (void){
	
	//Configurar UART0
	ConfigUART0(38400);
	
	//Declaración variables a usar 
	uint8_t ganador,noacepta,letra, numero1, numero2, numero3, numero4, suma1, suma2;
	//Declaración de arreglos con los mensajes predeterminador
	uint8_t oculta1[] = "_______________\n";
	uint8_t oculta2[] = "_______________\n";
	uint8_t mensaje1[] = "jugador 1 tire los dados:";
	uint8_t mensaje2[] = "jugador 2 tire los dados:";
	uint8_t medio[] = " -- ";
	uint8_t espacio[] = "\n";
	uint8_t ganador1[] = "El ganador fue el jugador 1!\n";
	uint8_t ganador2[] = "El ganador fue el jugador 2!\n";
	uint8_t empate[] = "FUE UN EMPATE!\n";
	
	while(1){ //Bucle infinito
		//Se Inicia el Juego
		ganador=1; 		//Variable de control del juego, no cambiará hasta que se logre un ganador
		//Valores inciales del avance de los jugadores
		suma1=0;  //Avance jugador 1
		suma2=0; //Avance jugador 2
		//*****************
		//Se envian el mapa de ludo
		Enviar_Cadena( oculta1 );  //Mapa ludo jugador 1
		Enviar_Cadena( oculta2 );	//Mapa ludo jugador 2
		//*******************************
		//Bucle que repite el proceso de pedir numeros hasta que se logre un ganador
		while(ganador){
			//Solicitud para jugador 1
			Enviar_Cadena( mensaje1 );
			noacepta=1;
			//Bucle que impide el avance del juego hasta que el jugador presione d
			while(noacepta){
				letra=Leer_Letra();
				//Condicional que valida que el jugador 1 hizo uso de su turno
				if(letra=='d'){
					numero1=rand()%6+1;  //Aleatorio numero1, rango de 1 a 6
					numero2=rand()%6+1;	//Aleatorio numero2, rango de 1 a 6
					noacepta=0;  //Se avanza en el juego, se sale del bucle
				}
				//**********************************
			}
			//****************
			//Imprime los numeros del jugador 1
			ImprimirNumero(numero1);
			Enviar_Cadena( medio );
			ImprimirNumero(numero2);
			Enviar_Cadena(espacio);
			//************************
			//*****************
			//Solicitud para jugador 2
			Enviar_Cadena( mensaje2 );
			noacepta=1;
			//Bucle que impide el avance del juego hasta que el jugador presione d
			while(noacepta){
				letra=Leer_Letra();
				//Condicional que valida que el jugador 2 hizo uso de su turno
				if(letra=='d'){
					numero3=rand()%6+1;	//Aleatorio numero3, rango de 1 a 6
					numero4=rand()%6+1;	//Aleatorio numero4, rango de 1 a 6
					noacepta=0;
				}
				//**********************************
			}
			//*************************
			//Imprime los numeros del jugador 2
			ImprimirNumero(numero3);
			Enviar_Cadena( medio );
			ImprimirNumero(numero4);
			Enviar_Cadena(espacio);
			//************************
			//**************************
			//Proceso para hallar los movimientos del jugador 1
			suma1+=numero1+numero2; //Avance del jugador 1
			//Condicional para el valor maximo que puede avanzar jugador 1
			if (suma1>15){
				suma1=15-(suma1-15); //Retrocede el número de casillas restantes
			}
			//*****************************
			//Proceso para hallar los movimientos del jugador 2
			suma2+=numero3+numero4;	//Avance del jugador 2
			//Condicional para el valor maximo que puede avanzar jugador 2
			if (suma2>15){
				suma2=15-(suma2-15);	//Retrocede el número de casillas restantes
			}
			//*****************************
			//Imprime el mapa de ludo con los avances correspondientes por jugador
			Imprimir_Juego1(oculta1,suma1); //Mapa avance jugador 1
			Imprimir_Juego2(oculta2,suma2);	//Mapa avance jugador 2
			//**********************
			//Condicional de salida y mensaje de ganador\es
			if (suma1==15){ //GANO JUGADOR 1
				Enviar_Cadena(ganador1);
				Enviar_Cadena(espacio);
				ganador=0; //Se reinicia el juego
			}else if (suma2==15){ //GANO JUGADOR 2
				Enviar_Cadena(ganador2);
				Enviar_Cadena(espacio);
				ganador=0;	//Se reinicia el juego
			} else if ((suma1==15) && (suma2==15)){	//EMPATE
				Enviar_Cadena(empate);
				Enviar_Cadena(espacio);
				ganador=0;	//Se reinicia el juego
			}
		}
		//******************************************************
	}
}
