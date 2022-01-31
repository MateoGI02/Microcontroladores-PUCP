/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ *****/ 
/***** FACULTAD DE CIENCIAS E INGENIERÍA *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Examen Final *****/ 
/***** Proyecto: EX2_Ejer *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: 7 Diciembre 2020 *****/ 
/****************************************************************************************/ 
/***** Enunciado: *****/ 
/***** *****/ 
/****************************************************************************************/


#include <stdint.h> 
#include "tm4c123gh6pm.h"
#define leds 0x0E
#define ledRojo 0x02
#define ledAzul 0x04
#define ledVerde 0x08


//Declaración variables globales
volatile uint8_t estado, tiempoRojo, tiempoAmbar, tiempoVerde, letra, rojo;
uint8_t mEspacio[] = "\n\r";
uint16_t tiempo_v=600;
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//Declaración de funcione codificadas en el archivo starup.s
void DisableInterrupts(void);
void EnableInterrupts(void);
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Funcion de configuración del UART0
/*Entradas: Ninguna
Salidas: Ninguna
*/
void ConfigUART0( void )
{ 
	SYSCTL_RCGC1_R |= SYSCTL_SRCR1_UART0; //Habilitamos reloj para el UART2
	SYSCTL_RCGC2_R |= SYSCTL_SRCR2_GPIOA; //Habilitamos reloj para GPIOD
	UART0_CTL_R &= ~UART_CTL_UARTEN; //Se desactiva el UART
	UART0_IBRD_R &= ~0xFFFF; //
	UART0_IBRD_R |= 1000000UL/115200; //Parte entera
	UART0_FBRD_R &= ~0x3F; //
	UART0_FBRD_R |= ((1000000UL%115200)*128 + 115200)/2/115200; //Parte decimal					 115200
	//UART0_LCRH_R &= ~UART_LCRH_SPS; //stick parity
	UART0_LCRH_R &= ~UART_LCRH_WLEN_M;
	UART0_LCRH_R |= UART_LCRH_WLEN_8; //8 bits de dato                                    8
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

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
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
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX 

/********************************************************************************************** 
*ConfiguraTimer_50ms(): Función para configurar el timer a 1ms. 
*Establecemos un tiempo base de 1ms para sondear el timer.Esto resulta conveniente cuando 
*queremos sondear varias tareas que se ejecutan a tiempos distintos, por lo que no 
*tendremos que modificar el timer 
************************************************************************************************/
void ConfiguraSysTick(void){ // Temporiza 50mseg, Fclksys=16MHz
	/*Inhabilitamos el módulo SysTick*/
	NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
	/* TiempoDeseado=1ms; FreqCPU=16MHZ 
	* valor que debe ir en el registro RELOAD=X 
	* X=(TiempoDeseado)*(FreqCPU) -1 
	* X= [[50*10^(-3)] * [16*10^(6)]] -1 =799999
	*799999 en hexadecimal es 000C34FF*/
	NVIC_ST_RELOAD_R = (NVIC_ST_RELOAD_R&0xFF000000)|0x000C34FF;
	// Iniciamos el contador con cero (escribiendo cualquier valor)
	NVIC_ST_CURRENT_R &= ~(0x00FFFFFF);
	// Habilitamos el módulo SysTick y las interrupciones
	NVIC_ST_CTRL_R |= (NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE);
}
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX 

// Función que transmite un caracter
void TxCar( uint8_t Car )
{
 while((UART0_FR_R & UART_FR_TXFF) != 0 ) ; //Espera mientras pila llena
 UART0_DR_R = Car; 
}
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

// Función que espera hasta recibir un caracter
uint8_t RxCar( void )
{ 
 while((UART0_FR_R & UART_FR_RXFE) != 0 ) ; //Espera mientra pila vacía
 return UART0_DR_R & 0xFF;
}
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

// Funcion que transmite una cadena ASCII-Z
void Enviar_cadena( uint8_t Cadena[] )
{
uint8_t i;
 for( i = 0; Cadena[i] != '\0'; i++ )
 TxCar( Cadena[i] );
}
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

// Funcion Timer en puerto B
void Configura_GPTM1A_PWM(void){
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
	while(!(SYSCTL_PRTIMER_R & SYSCTL_PRTIMER_R1));
	TIMER1_CTL_R &= ~TIMER_CTL_TAEN;
	TIMER1_CFG_R &= ~TIMER_CFG_M;
	TIMER1_CFG_R |= TIMER_CFG_16_BIT;
	TIMER1_TAMR_R = (TIMER1_TAMR_R & 0xFFFFFFF0) | 0x0000000A;
	TIMER1_CTL_R |= TIMER_CTL_TAPWML;
	TIMER1_TAILR_R = (TIMER1_TAILR_R & 0xFFFF0000) | 0x000070FF;
	TIMER1_TAPR_R = (TIMER1_TAPR_R & 0xFFFFFF00) | 0x00000002;
	
	//Cnfiguración tiempo en alta
  TIMER1_TAMATCHR_R = (TIMER1_TAMATCHR_R & 0xFFFF0000) | (40*1600 & 0x0000FFFF);
  TIMER1_TAPMR_R = (TIMER1_TAPMR_R & 0xFFFFFF00) | ((40*1600 & 0x000F0000)>>16);
	//
	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	while(!(SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1));
	GPIO_PORTB_DIR_R |= 0x01;
	GPIO_PORTB_AFSEL_R |= 0x01;
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFF) | 0x00070000;
	GPIO_PORTB_DR8R_R |= 0x01;
	GPIO_PORTB_DEN_R |= 0x01;
	TIMER1_CTL_R |= TIMER_CTL_TAEN;
}
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


//Función incio del programa
void Inicio(){
	uint8_t mInicio[] = "Ingrese 'I' para iniciar\n";
	uint8_t mValores[] = "Ingrese 'S' o 'F' para alterar tiempo en Verde\n";
	uint8_t inicio=0;  //No se inicio el programa
	Enviar_cadena(mInicio); //Se envia el mensaje de ingresar I para iniciar
	while (!inicio){  //No se ingreso I y no se inicio el programa
		if (RxCar()=='I'){ //Se lee el valor en el uart y se compara con I
			TxCar('I'); //Se imprime el valor
			TxCar('\n');		//Se imprime el espacio
			inicio=1; //Se sale del bucle pues se inicio el programa
		}
	}
	Enviar_cadena(mValores); //Envia mensaje para leer la duración del verde
}
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

void SysTick_Handler(void){
	//incrementamos el contador de tiempo para el rojo y se prende el led rojo
	if ((tiempoRojo<8) && (estado=='R')) {tiempoRojo++;GPIO_PORTF_DATA_R |= ledRojo; rojo=1;}
	//incrementamos el contador de tiempo para el ambar y se prende el led ambar
	if ((tiempoAmbar<3) && (estado=='A')) {tiempoAmbar++;}
	//incrementamos el contador de tiempo para el verde y se prende el led verde
	if ((tiempoVerde<(tiempo_v/50))&& (estado=='V')) {tiempoVerde++; GPIO_PORTF_DATA_R |= ledVerde; rojo=0;}
	
  if(tiempoRojo==8) { GPIO_PORTF_DATA_R &=~(leds); tiempoRojo=0; estado='A';}
	if(tiempoAmbar==3) {
		GPIO_PORTF_DATA_R &=~(leds); 
		tiempoAmbar=0;
		if (rojo==1){
			estado='V';
		} else {
			estado='R';
		}
	}
	if(tiempoVerde==(tiempo_v/50)) { GPIO_PORTF_DATA_R &=~(leds); tiempoVerde=0;estado='A';}

	
	letra=UART0_DR_R & 0xFF;
	if (letra=='S'){
		tiempo_v=600;
		TxCar('S'); //Se imprime el valor
		TxCar('\n');		//Se imprime el espacio
	} else if (letra=='F'){
		tiempo_v=600;
		TxCar('F'); //Se imprime el valor
		TxCar('\n');		//Se imprime el espacio
	}
}


// Función principal
int main (void){
	
	DisableInterrupts(); //Desabilita Interrupciones
	
	estado='R';  // Inicia estado Rojo
	tiempoRojo=0;  //La cuenta inicia en 0
	tiempoVerde=0;  //La cuenta inicia en 0
	tiempoAmbar=0;  //La cuenta inicia en 0
	
	ConfigUART0();    //Configura UART
	ConfigPuertoF();		//Configura Puerto F
	ConfiguraSysTick();	//Configura interrupciones
	
	
  
	//Iniciar el programa
	Inicio();
	//****************************************

	EnableInterrupts();  //Habilitan interrupciones
	while (1){
		
	}
}
