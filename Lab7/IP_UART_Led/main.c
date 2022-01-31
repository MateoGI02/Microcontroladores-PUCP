/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ *****/ 
/***** FACULTAD DE CIENCIAS E INGENIERÍA *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Laboratorio 7 *****/ 
/***** Proyecto: IP_UART_Led *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: 04 Noviembre 2020 *****/ 
/****************************************************************************************/ 
/***** Enunciado: Se genera un juego según las entradas en el UART y se muestra en led y la pantalla del UART los datos 
				necesarios del juego, se cuenta con una opción de reinicial el juego*****/ 
/***** *****/ 
/****************************************************************************************/

#include <stdint.h> 
#include "tm4c123gh6pm.h"

//Definir los Leds
#define ledRojo 0x02
#define ledAzul 0x04
#define ledVerde 0x08
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
 //SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; //Activa reloj del puerto F
 //while( (SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5) == 0 ) {} //¿Listo para operar?
 GPIO_PORTF_LOCK_R = 0x4C4F434B; //Desbloquear puerto F
 //GPIO_PORTF_CR_R |= (1<<0); //Se habilita PF0 (SW2)
 //GPIO_PORTF_DIR_R &= ~((1<<4)|(1<<0)); //PF4(SW1), PF0(SW2): entradas
 //GPIO_PORTF_AFSEL_R &= ~(1<<0); //Desactiva función alterna de PF0
 //GPIO_PORTF_PUR_R |= ((1<<4)|(1<<0)); //Activa pull-up
 //GPIO_PORTF_DEN_R |= ((1<<4)|(1<<0)); //Activa función digital
 GPIO_PORTF_DIR_R |= ((1<<3)|(1<<2)|(1<<1)); //PF3 (GREEN), PF2 (BLUE), PF1 (RED): salidas
 GPIO_PORTF_DR8R_R |= ((1<<3)|(1<<2)|(1<<1)); //Activa driver 8 mA
 GPIO_PORTF_DEN_R |= ((1<<3)|(1<<2)|(1<<1)); //Activa función digital
 GPIO_PORTF_DATA_R &= ~((1<<3)|(1<<2)|(1<<1)); //Leds apagados
}



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

/*Función para la transmisión de un caracter*/
/*Entradas: car: caracter que se muestra en la pantalla
Salidas: Ninguna
*/
void txcar_uart0(uint32_t car){ 
 while ((UART0_FR_R & UART_FR_TXFF)!=0); //Espera que esté disponible para tx
 UART0_DR_R = car; 	//Se envia el caracter car
} 

/*Función para la recepción de un carácter de 8 bits*/ 
/*Entradas: Ninguna
Salidas: temp:caracter recibido en el UART0
*/
uint8_t rxcar_uart0( void ){
	uint8_t temp; //carácter leido
	while ((UART0_FR_R & UART_FR_RXFE)!=0); // Se espera que llegue un dato
	temp= UART0_DR_R&0xFF; // Se toman solo 8 bits
	return temp; //Se devuelve el dato
} 

/*Funcion para el envío de una cadena*/
/*Entradas: mens[]:arreglo con mensaje deseado para mostrar en el UART
Salidas: Ninguna  
*/
void txmens_uart0( uint8_t mens[] )
{
 uint8_t letra;
 uint8_t i = 0;
 letra = mens[ i++ ]; 
 while( letra != '\0' ){
	//Se envían todos los caracteres hasta el fin de cadena
	txcar_uart0( letra ); 
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
			txcar_uart0('1');		//Se envia el número 1
			break;
		case 2:
			txcar_uart0('2');			//Se envia el número 2
			break;
		case 3:
			txcar_uart0('3');			//Se envia el número 3
			break;
		case 4:
			txcar_uart0('4');			//Se envia el número 4
			break;	
		case 5:
			txcar_uart0('5');			//Se envia el número 5
			break;
		case 6:
			txcar_uart0('6');			//Se envia el número 6
			break;
		case 7:
			txcar_uart0('7');			//Se envia el número 7
			break;
		case 8:
			txcar_uart0('8');			//Se envia el número 8
			break;
		case 9:
			txcar_uart0('9');			//Se envia el número 9
			break;
		case 10:
			txcar_uart0('1');			//Se envia el número 1
			txcar_uart0('0');			//Se envia el número 0
			break;
		case 11:
			txcar_uart0('1');			//Se envia el número 1
		  txcar_uart0('1');			//Se envia el número 1
			break;
		case 12:
			txcar_uart0('1');			//Se envia el número 1
		  txcar_uart0('2');			//Se envia el número 2
			break;
		default:
			txcar_uart0('1');	    //Se envia el número 1
			txcar_uart0('3');			//Se envia el número 3
			break;
	}	  
}

/*Funcion para el envio de un palo al UART*/
/*Entradas: número:numero del 0 al 3 que representa un palo
Salidas: Ninguna  
*/
void ImprimirPalo( uint8_t numero )
{
	switch (numero){
		case 0:
			txcar_uart0('p');	  	//Pica
			break;
		case 1:
			txcar_uart0('c');			//Corazón
			break;
		case 2:
			txcar_uart0('r');			//Rombo
			break;
		default:
			txcar_uart0('t');			//Trebol
			break;
	}	  
}

// Función principal
int main (void){
	
	//Declaración variables a usar 
	uint8_t contador, control,numero1,numero2,numero1p,numero2p,numero3,numero3p;
	//Variable de tiempo que genera retrasos
	uint8_t letra, menor, mayor, pmenor,pmayor;
	uint32_t i;
	uint8_t mInicio[] = "Presione ENTER 2 veces para empezar a jugar\n\r";
	uint8_t mLetra[] = "Presione s para continuar o n para obtener nuevos numeros\n\r";
	uint8_t mEspacio[] = "\n";
	uint8_t mMedio[] = " -- ";

	
	//Configurar Puertos
	ConfigPuertoF(); // configura el puerto F
	//Configurar UART0
	ConfigUART0(19200);
	
	
	while(1){ //Bucle infinito
		//Valores Iniciales variables
		control=1;
		contador=0;
		numero1=1;
		numero2=5;
		numero1p=2;
		numero2p=0;
		numero3=4;
		numero3p=1;
		//Se envia el mensaje para iniciar el juego
		txmens_uart0( mInicio );
		//Número aleatorio para el número 1 y el palo 1
		for(i=0;i<956;i++){
			numero1++;
			numero1p++;
			//Validación del rango del número 1 y su palo
			if(numero1>13){
				numero1=1;
			}
			if(numero1p>3){
				numero1p=0;
			}
			//*****************************
		}
		//******************
		//Bucle para iniciar el juego y hacer aleatorios los numeros
		while(contador<2){ //No acabará hasta presionar 2 enter, mientras tanto los número iran variando aleatoriamente
			//Incremento números y palos
			numero1++;
			numero2++;
			numero1p++;
			numero2p++;
			//**********************
			//Validación Rangos de los números y palos
			if(numero1>13){
				numero1=1;
			}
			if(numero2>13){
				numero2=1;
			}
			if(numero1p>3){
				numero1p=0;
			}
			if(numero2p>3){
				numero2p=0;
			}
			//**********************
			//Lectura del caracter recibido en el UART
			letra = UART0_DR_R&0xFF;
			//Condicional que valida el envio del caracter Enter
			if (letra==0x0D){ //Enter en ASCII es 0D
				contador++; 		//Incremento contador de Enter
			}
			//*******
		}
		//********************************
		//Clasificar los números por mayor y menor con sus respectivos palos
		if(numero1<numero2){
			menor=numero1;
			pmenor=numero1p;
			mayor=numero2;
			pmayor=numero2p;
		} else {
			menor=numero2;
			pmenor=numero2p;
			mayor=numero1;
			pmayor=numero1p;
		}
		//***************************************
		//Se envian los datos de los números y palos obtenidos
		ImprimirNumero(menor); //Se envia número menor
		ImprimirPalo(pmenor);		//Se envia palo menor
		txmens_uart0( mMedio );		//Se envia el separador
		ImprimirNumero(mayor);		//Se envia el número mayor
		ImprimirPalo(pmayor);			//Se envia el palo mayor
		txmens_uart0( mEspacio );		//Se salta de linea
		//***************************
		txmens_uart0( mLetra );  //Se imprime mensaje para participar del juego o reiniciar
		while(control){  /*Bucle que solicita se ingrese uno de los valores acpetados por el juego mientras 
												se hace aleatorio un número 3 y su palo*/
			//Se lee el caracter enviado
			letra = UART0_DR_R&0xFF;
			//Se aumenta número 3 y su palo
			numero3++;
			numero3p++;
			//*********************
			//Valida el rango del número 3 y su palo
			if(numero3>13){
				numero3=1;
			}
			if(numero3p>3){
				numero3p=1;
			}
			//*********************
			//Condicional 1, se ingreso n y se reinicia el juego
			if (letra==0x6E){
				//Led Blanco parpadea 
				GPIO_PORTF_DATA_R |= ledRojo|ledAzul|ledVerde; 
				for(i = 0; i < 800000; i++); 
				GPIO_PORTF_DATA_R &= ~leds; 						
				for(i = 0; i < 800000; i++);
				// ************************************
				txmens_uart0( mEspacio );  //Se envia un salto de linea
				control=0; //Se sale del bucle, terminando los procesos dentro del while(1) y reiniciando todo el proceso
			//*****************************
			// Condicional 2, se inserta s y se juega con el número 3 
			}else if (letra==0x73){
				//Imprime el valor del número 3 y su palo
				ImprimirNumero(numero3);
				ImprimirPalo(numero3p);
				txmens_uart0( mEspacio );
				txmens_uart0( mEspacio );
				//***************************
				//Condicional para ver si se gano el juego
				if((numero3<mayor)&&(numero3>menor)){
					//Led Verde parpadea 
					GPIO_PORTF_DATA_R |= ledVerde; 
					for(i = 0; i < 800000; i++); 
					GPIO_PORTF_DATA_R &= ~leds; 						
					for(i = 0; i < 800000; i++);
					// ************************************
				}else{ //Resultado de perder el juego
					//Led Rojo parpadea 
					GPIO_PORTF_DATA_R |= ledRojo; 
					for(i = 0; i < 800000; i++); 
					GPIO_PORTF_DATA_R &= ~leds; 						
					for(i = 0; i < 800000; i++);
					// ************************************
				}
				control=0; //Se termina el juego y se vuelve a ejecutar todo del inicio
			}
		}
	}
}
