/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ *****/ 
/***** FACULTAD DE CIENCIAS E INGENIERÍA *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Laboratorio 9 *****/ 
/***** Proyecto: PL_Señal_L9 *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: 18 Noviembre 2020 *****/ 
/****************************************************************************************/ 
/***** Enunciado: Se genera una señal que varía segun ingresos en el UART*****/ 
/***** *****/ 
/****************************************************************************************/




#include <stdint.h> 
#include "tm4c123gh6pm.h"


uint16_t frecuencia=100;

uint8_t porcentaje=50;
char letra;

uint16_t milisegundos=0;
uint8_t ingreso_dato=0;

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
*ConfiguraSysTick(): Función para configurar el timer a 1s. 
*Establecemos un tiempo base de 1ms para sondear el timer.Esto resulta conveniente cuando 
*queremos sondear varias tareas que se ejecutan a tiempos distintos, por lo que no 
*tendremos que modificar el timer 
************************************************************************************************/
void ConfiguraSysTick(void){ // Temporiza 1mseg, Fclksys=16MHz
	NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
	NVIC_ST_RELOAD_R = (NVIC_ST_RELOAD_R&0xFF000000)|0x00493DFF;
	NVIC_ST_CURRENT_R &= ~(0x00FFFFFF);
	NVIC_ST_CTRL_R |= (NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE);
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

//--------------------RxCar-------------------
// FunciÃ³n que espera hasta recibir 
// un caracter
// la pila de recepciÃ³n
// Input: none
// Output: Caracter recibido
uint8_t Leer_Car( void )
{ 
  while((UART0_FR_R & UART_FR_RXFE) != 0 ) ; // Espera mientras pila vacÃ­a
  return UART0_DR_R & 0xFF;
}

void SysTick_Handler(void){
	milisegundos++;
	if(milisegundos>((10*10*10/frecuencia))){
		milisegundos=0;
	}
	switch (letra){
		case 'F':
			frecuencia+=2;
			if(frecuencia>1000){
				frecuencia=1000;
			}
			break;
		case 'L':
			frecuencia-=2;
			if(frecuencia<1){
				frecuencia=1;
			}
			break;
		case 'D':
			porcentaje+=10;
			if(frecuencia>90){
				frecuencia=90;
			}
			break;
		case 'S':
			porcentaje-=10;
			if(frecuencia<10){
				frecuencia=10;
			}
			break;
		default:
			break;
	}
}

/* Función que genera una señal en el PF1.
Entradas: Ninguna
Salidas: Ninguna
*/
void genera_senal( void )
{ 
	uint8_t alto=(((10*10*10)/frecuencia)*(porcentaje/100));
	uint8_t bajo=(((10*10*10)/frecuencia)*((100-porcentaje)/100));
	while (milisegundos<alto){ 
		GPIO_PORTF_DATA_R |= 0x02;
		//Actualiza el valor del tiempo en milisegundos
	}
	milisegundos=0;
	while (milisegundos<bajo){ 
		GPIO_PORTF_DATA_R &= ~0x02;
		//Actualiza el valor del tiempo en milisegundos
	}	
	milisegundos=0;
}
//***************************************************************************

// Función principal
int main (void){
	
	//Declaración variables a usar 
	//Variable de tiempo que genera retrasos
	uint8_t mInicio[] = "Ingrese caracter ASCII conforme lo siguiente: \n F: Aumento de frecuencia \n L: Disminuir la frecuencia\n D: Aumenta Ciclo de trabajo\n S: Disminuye Ciclo de Trabajo\n";
	
	//Configurar Puertos
	ConfigPuertoF(); // configura el puerto F
	//Configurar UART0
	ConfigUART0(1920);
	
	ConfiguraSysTick();
	
	Enviar_cadena(mInicio);
	
	frecuencia=100;
	
	GPIO_PORTF_DATA_R |= (0x02);
	
	while(1){ //Bucle infinito
		genera_senal();
		letra=Leer_Car();
	}
}

