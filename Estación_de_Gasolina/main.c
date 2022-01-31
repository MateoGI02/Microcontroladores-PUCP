

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "UART.h"

#define _XOPEN_SOURCE_EXTENDED 1
#include <stdlib.h>

uint8_t verificar_eleccion ( void );


	char Elegido_90[]  = "90";
	char Elegido_93[]  = "93";
	char Elegido_95[]  = "95";
	char Elegido_97[]  = "97";
	char Elegido_GNV[] = "GLP";
	char Elegido_GLP[] = "GNV";
	char Elegido_84[]  = "84";
	
	uint8_t Enter[] = "\r\n";
	char Enter1[] = "\n";
	uint8_t importe_esc[] = "##.## ";
	
	char Importe_ST[] = " 00.00       SOLES \n"; // Costo total de la gasolina pedido
	char Precio_ST[]  = " 00.00       SOLES \n"; // Precio de la gasolina pedida
	char Volumen_ST[] = " 00.00      LITROS \n"; // El volumen que está siendo despachado
	char precio_90[]  = " 12.67       SOLES \n";
	char precio_93[]  = " 14.31       SOLES \n";
	char precio_95[]  = " 15.85       SOLES \n";
	char precio_97[]  = " 17.63       SOLES \n";
	char precio_GNV[] = "  2.33       SOLES \n";
	char precio_GLP[] = "  1.38       SOLES \n";
	char precio_84[]  = " 11.56       SOLES \n";
	char importe_num[6];

	uint8_t calidad = 0;
	double precio_escog, importe;
	uint8_t centesima_gas=0, decima_gas=0, unidad_gas=0, decena_gas=0;
	uint8_t centesima_importe=0, decima_importe=0, unidad_importe=0, decena_importe=0;
	
	uint8_t contador_gas      = 0;
	uint8_t contador_importe   = 0;
	
	const uint16_t tiempo_gas     = 10;
	const uint16_t tiempo_importe  = 5;
	
	uint8_t inicio=1;
	uint8_t ingreso_dato=0;
	uint8_t completo=1;
	uint8_t pantalla;
	char elegido[]="00";
	
/************************************************************************************************/
void config_portF (void) {
 // activamos la señal de reloj del puerto F
 SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	
 // espera a que el puerto esté operativo
 while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)==0){}
 GPIO_PORTF_LOCK_R = 0x4C4F434B; // Desbloquear puerto F
 GPIO_PORTF_CR_R |= 0x01;        // Se habilita PF0 (SW2)
 // Entradas PF0 y PF4 (SW1 y SW2)
 GPIO_PORTF_DIR_R &= ~(0x11);   // Configura PF0 y PF4 como entradas
 GPIO_PORTF_AFSEL_R &= ~(0x11); // No utilizar funciones alternas
 GPIO_PORTF_PUR_R |= 0x11;      // Activa resistencias de pull-up de PF0 y PF4
 GPIO_PORTF_DEN_R |= 0x11;      // Habilita señales digitales en PF0 y PF4
 
 // Salidas PF1, PF2, PF3 (leds)	 
 GPIO_PORTF_DIR_R |= 0x0E;    // Configura PF1, PF2 y PF3 como salidas
 GPIO_PORTF_DR8R_R |= 0x0E;   // Activa el driver de 8 mA en PF1, PF2 y PF3
 GPIO_PORTF_DEN_R |= 0x0E;    // Habilita señales digitales en PF1, PF2 y PF3
 GPIO_PORTF_DATA_R &=~(0x0E); // Apaga los LEDs conectados a PF1, PF2 y PF3
	 
}// fin config_portF()
/************************************************************************************************/

/************************************************************************************************/
// ConfiguraTimer_1s(): Función para configurar el timer a 1s.
// Establecemos un tiempo base de 1 s para sondear el timer. Esto resulta conveniente cuando 
// queremos sondear varias tareas que se ejecutan a tiempos distintos, por lo que no 
// tendremos que modificar el timer .
/************************************************************************************************/
void ConfiguraTimer_1ms(void)
    {
     /*Inhabilitamos el módulo SysTick*/
     NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
     /* TiempoDeseado=1ms; FreqCPU=16MHZ
      * valor que debe ir en el registro RELOAD=X
      * X=(TiempoDeseado)*(FreqCPU) -1
      * X= [[1*10^-3] * [80*10^(6)]] -1 =79 999 
      * 15 999 en hexadecimal es 0x0001387F*/
     NVIC_ST_RELOAD_R = (NVIC_ST_RELOAD_R&0xFF000000)|0x0001387F;
		// Se usara un contador de 1ms para que la cuenta sea de 0 a 9999 ms,
     // Iniciamos el contador con cero (escribiendo cualquier valor)
     NVIC_ST_CURRENT_R &= ~(0x00FFFFFF);
     // Habilitamos el módulo SysTick
     NVIC_ST_CTRL_R |= (NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE);
}
/************************************************************************************************/



void SysTick_Handler(void){
	
	if (RxCar()=='x'){
		inicio=0;
		completo=0;
	}
	//incrementamos el contador de tiempo para la tarea1
	if (contador_gas<tiempo_gas) {contador_gas++;}
		//incrementamos el contador de tiempo para la tarea2
	if (contador_importe<tiempo_importe) {contador_importe++;}
	
	if(contador_gas==tiempo_gas) { contador_gas = 0; centesima_gas++;}
	if(contador_importe==tiempo_importe) { contador_importe = 0; centesima_importe++;}
	
	if(centesima_gas==10){ centesima_gas=0; decima_gas++;}
	if(centesima_importe==10){ centesima_importe=0; decima_importe++;}
		
	if(decima_gas==10){ decima_gas=0; unidad_gas++;}
	if(decima_importe==10){ decima_importe=0; unidad_importe++;}

	if(unidad_gas==10){ unidad_gas=0; decena_gas++;}
	if(unidad_importe==10){ unidad_importe=0; decena_importe++;}
	
}


// Función para obtener el codigo ASCII de un numero
uint8_t Num_to_ASCII(uint8_t num){
	return '0' + num;
}
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


// Funcion para obtener el codigo ASCII de un numero con cierta condicion
uint8_t Dec_to_ASCII(uint8_t num){
	if (num == 0) return ' ';
	else return '0' + num;
}
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

void Mostrar_volumen( void ){
	
	double volumen_total, volumen_actual=0;
	
	volumen_total = importe_esc[0]*10 + importe_esc[1] + importe_esc[3]/10 + importe_esc[4]/100;
	volumen_total = volumen_total/precio_escog; 
	
	while(volumen_actual<volumen_total){
		
		
		if(NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT){ // Preguntamos si paso 1 ms
			SysTick_Handler(); // Actualizamos los valores para mostrarlo en el LCD	
			
			Volumen_ST[1] = Dec_to_ASCII(decena_gas);
			Volumen_ST[2] = Num_to_ASCII(unidad_gas);
			Volumen_ST[4] = Num_to_ASCII(decima_gas);
			Volumen_ST[5] = Num_to_ASCII(centesima_gas);
			
			Importe_ST[1] = Dec_to_ASCII(decena_gas);
			Importe_ST[2] = Num_to_ASCII(unidad_gas);
			Importe_ST[4] = Num_to_ASCII(decima_gas);
			Importe_ST[5] = Num_to_ASCII(centesima_gas);
			
			volumen_actual = decena_gas*10 + unidad_gas + decima_gas/10 + centesima_gas/100;
			
			ST7735_DrawString(0, 10, Importe_ST, ST7735_YELLOW);
			ST7735_DrawString(0, 13, Volumen_ST, ST7735_YELLOW);
			
		}
		
	}
	
}



void asignar_valores( char arreglo[] ){
	uint8_t i=0;
	while (i < 5){
		Precio_ST[i] = arreglo[i];
	}
}


void calidad_elegida ( void ){
	
	switch (calidad){
		case 0:
			asignar_valores(precio_90);
			precio_escog = 12.67;
			break;
		case 1:
			asignar_valores(precio_93);
			precio_escog = 14.31;
			break;
		case 2:
			asignar_valores(precio_95);
			precio_escog = 15.85;
			break;
		case 3:
			asignar_valores(precio_97);
			precio_escog = 17.63;
			break;
		case 4:
			asignar_valores(precio_GLP);
			precio_escog = 2.33;
			break;
		case 5:
			asignar_valores(precio_GNV);
			precio_escog = 1.38;
			break;
		default:
			asignar_valores(precio_84);
			precio_escog = 11.56;
			break;
		
	}
}

void Mostrar_Pant_Ini( void ){
	
  ST7735_OutString("    BIENVENIDOS A:  \n");//0
  ST7735_OutString(" GASOLINERIA -OCTAN-\n");   //1
	ST7735_OutString("     SELECCION DE   \n");			//2
	ST7735_OutString("      PRODUCTOS:    \n");		//3
	ST7735_OutString("--------------------\n");		//4
	ST7735_OutString("(0)90: DISP. (12.67)\n");			//5
	ST7735_OutString("(1)93: DISP. (14.31)\n");			//6	
	ST7735_OutString("(2)95: DISP. (15.85)\n");			//7
	ST7735_OutString("(3)97: DISP. (17.63)\n");			//8
	ST7735_OutString("--------------------\n");			//9
	ST7735_OutString("(4)GLP: DISP. (2.33)\n");			//10
	ST7735_OutString("(5)GNV: DISP. (1.38)\n");			//11
	ST7735_OutString("(6)84: DISP. (11.56)\n");			//12
	ST7735_OutString("--------------------\n");			//13
	if(ingreso_dato){
		ST7735_OutString("Producto elegido:");		//14
		ST7735_OutString(elegido);
		ST7735_OutString(Enter1);
		ST7735_OutString(" Pantallas operando \n");		//15
	}else{
		ST7735_OutString("Seleccione producto\n");		//14
	}	
}


void Mostrar_Pant_2( void ){
	
	calidad_elegida();	
	
	ST7735_OutString("-------------------\n");
  ST7735_OutString("   BIENVENIDOS A:  \n");
  ST7735_OutString("GASOLINERIA -OCTAN-\n");
	ST7735_OutString("    DESPACHO DEL   \n");
	ST7735_OutString("     PRODUCTO:     \n");
	ST7735_OutString("-------------------\n");
	ST7735_OutString("PRECIO:            \n");
	ST7735_OutString(Precio_ST);
	ST7735_OutString("-------------------\n");
	ST7735_OutString("IMPORTE:           \n");
	Mostrar_volumen();
	ST7735_OutString(Importe_ST);
	ST7735_OutString("-------------------\n");
	ST7735_OutString("VOLUMEN:           \n");
	ST7735_OutString(Volumen_ST);
	ST7735_OutString("-------------------\n");
	
	Mostrar_volumen();
	
}

void Mostrar_Pant_3( void ){
  ST7735_OutString(" GASOLINERIA -OCTAN-\n");
}

void Mostrar_Pant_4( void ){
	
  ST7735_OutString(" GASOLINERIA -OCTAN-\n");
}

uint8_t verificar_eleccion ( void ){
	
	uint8_t i;
	if (HayCarRx()){
		switch (RxCar()){
				case '0':
					for (i=0; i<2; i++){
						elegido[i]=Elegido_90[i];
					}
					calidad = 0;
					return 1;
				case '1':
					for (i=0; i<2; i++){
						elegido[i]=Elegido_93[i];
					}
					calidad = 1;
					return 1;
				case '2':
					for (i=0; i<2; i++){
						elegido[i]=Elegido_95[i];
					}
					calidad = 2;
					return 1;
				case '3':
					for (i=0; i<2; i++){
						elegido[i]=Elegido_97[i];
					}
					calidad = 3;
					return 1;
				case '4':
					for (i=0; i<2; i++){
						elegido[i]=Elegido_GLP[i];
					}
					calidad = 4;
					return 1;
				case '5':
					for (i=0; i<2; i++){
						elegido[i]=Elegido_GNV[i];
					}
					calidad = 5;
					return 1;
				case '6':
					for (i=0; i<2; i++){
						elegido[i]=Elegido_84[i];
					}
					calidad = 6;
					return 1;
				default:
					return 0;
		}
	}
	return 0;
}


void verificar_pulsadores( void ){
	
	if(((GPIO_PORTF_DATA_R & 0x11) == 0x00)&&(pantalla!=4)){ //¿se presionó SW2 y SW1?
		Output_Clear();
		Output_On();
		Mostrar_Pant_4();
		pantalla=4;
	} else if(((GPIO_PORTF_DATA_R & 0x11) == 0x10)&&(pantalla!=3)){ //¿se presionó SW2?
		Output_Clear();
		Output_On();
		Mostrar_Pant_3();
		pantalla=3;
	} else if(((GPIO_PORTF_DATA_R & 0x11) == 0x01)&&(pantalla!=2)){ //¿se presionó SW1?
		Output_Clear();
		Output_On();
		Mostrar_Pant_2();		// Mostrar pantalla 2
		pantalla=2;
	} else if(((GPIO_PORTF_DATA_R & 0x11) == 0x11)&&(pantalla!=1)){ //¿se presionó NINGUNO?
		Output_Clear();
		Output_On();
		Mostrar_Pant_Ini(); // Mostrar pantalla 1
		pantalla=1;
	}
	
}




//PROGRAMA PRINCIPAL
int main ( void )
{
  
	
	ConfigUART0();											 // Inicializa el terminal serial
	ConfiguraTimer_1ms();								 // Inicializa el terminal serial
	config_portF();										   // Inicializa el puerto F
	PLL_Init(Bus80MHz);                  // Inicializa el reloj del sistema a 80 MHz
  ST7735_InitR(INITR_REDTAB);					 // Inicializa la pantalla LCD Sitronix ST7735
	

	
	
  while(1){
		
		inicio=1;
		completo=1;
		Output_Clear();
		Output_On();
		Mostrar_Pant_Ini();
		while (inicio){
			if (verificar_eleccion()){
				ST7735_SetCursor(0,14);
				ST7735_OutString(" Producto elegido:");
				ST7735_OutString(elegido);
				ST7735_OutString(Enter1);
				ST7735_OutString(" Pantallas operando \n");
				pantalla=1;
				inicio=0;
			}
		}
		while(completo){
			verificar_pulsadores();
		}
		
  }
}

