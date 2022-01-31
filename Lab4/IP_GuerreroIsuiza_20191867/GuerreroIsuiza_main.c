/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CAT�LICA DEL PER� *****/ 
/***** FACULTAD DE CIENCIAS E INGENIER�A *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Laboratorio 4 *****/ 
/***** Proyecto: IP Se�al Entradas *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: Septiembre 2020 *****/ 
/****************************************************************************************/ 
/***** Enunciado: Se genera una se�al mediante la activaci�n por el SW2. *****/ 
/***** *****/ 
/****************************************************************************************/

#include <stdint.h> 
#include "tm4c123gh6pm.h"

#define SW1 0x10
#define SW2 0x01
#define ledRojo 0x02
#define ledAzul 0x04
#define ledVerde 0x08
#define leds 0x0E
#define periodo_min 20000
#define periodo_max 2000000


/* Funci�n que configura los pines PF0 y PF4 conectados a SW2 y SW1 como entradas
y los pines PF1, PF2, PF3 conectados a los LEDs Rojo, Azul y Verde como salidas.
Entradas: Ninguna
Salidas: Ninguna
*/
void config_portF (void) {
	 // activamos la se�al de reloj del puerto F
	 SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	 // espera a que el puerto est� operativo
	 while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)==0){}
	 GPIO_PORTF_LOCK_R = 0x4C4F434B; // Desbloquear puerto F
	 GPIO_PORTF_CR_R |= 0x01; // Se habilita PF0
	 
	 GPIO_PORTF_DIR_R &= ~(0x11); // Configura PF0 y PF4 como entradas
	 GPIO_PORTF_AFSEL_R &= ~(0x11); // No utilizar funciones alternas
	 GPIO_PORTF_PUR_R |= 0x11; // Activa resistencias de pull-up de PF0 y PF4
	 GPIO_PORTF_DEN_R |= 0x11; // Habilita se�ales digitales en PF0 y PF4
	 GPIO_PORTF_DIR_R |= leds; // Configura PF1, PF2 y PF3 como salidas
	 GPIO_PORTF_DR8R_R |= leds; // Activa el driver de 8 mA en PF1, PF2 y PF3
	 GPIO_PORTF_DEN_R |= leds; // Habilita se�ales digitales en PF1, PF2 y PF3
	 GPIO_PORTF_DATA_R &=~(leds); // Apaga los LEDs conectados a PF1, PF2 y PF3
}

/* Funci�n que configura el pin PB2 como salida para generar la se�al cuadrada
Entradas: Ninguna
Salidas: Ninguna
*/
void config_portB (void) {
	 // activamos la se�al de reloj del puerto B
	 SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	 // espera a que el puerto est� operativo
	 while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1)==0){}
	 
	 GPIO_PORTB_DIR_R |= 0x04; // Configura PB2 como salida
	 GPIO_PORTB_AFSEL_R &= ~(0x04); // No utilizar funciones alternas
	 GPIO_PORTB_DR8R_R |= 0x04; // Activa el driver de 8 mA en PB2
	 GPIO_PORTB_DEN_R |= 0x04; // Habilita la se�al digital en PB2
	 GPIO_PORTB_DATA_R &=~(0x04); // Se�al en "0 l�gico"
}// fin config_portB


/* Funci�n que valida el valor del tiempo de trabajo
Entradas: t_alta - duraci�n en porcentaje del periodo de la se�al en alta ("1 l�gico") 
Salidas: valido - 0 si el valor de t_alta no es valido y 1 si el valor de t_alta es valido
*/
int valida_tiempo (uint8_t t_alta){
	uint32_t valido=0;	//Variable de control
	if ((t_alta%10==0) && (t_alta<100) && (t_alta>0)){ //Condicional que valida variable
		valido=1; //Verdadero - variable=1
	}
	return valido; //Devuelve el valor de valido
}

/* Funci�n que valida el valor del tiempo de trabajo
Entradas: ciclos - n�mero de periodos de la se�al 
Salidas: valido - 0 si el valor de ciclos no es valido y 1 si el valor de ciclos es valido
*/
int valida_pulsos (uint8_t ciclos){
	uint32_t valido=0; //Variable de control
	if ((ciclos<=15) && (ciclos>=5)){ //Condicional que valida variable
		valido=1; //Verdadero - variable=1
	}
	return valido; //Devuelve el valor de valido
}

/* Funci�n que valida el valor del periodo
Entradas: periodo - duraci�n de una �nica se�al 
Salidas: valido - 0 si el valor de t_alta no es valido y 1 si el valor de t_alta es valido
*/
int valida_periodo (uint32_t periodo){
	uint32_t valido=0; //Variable de control
	if ((periodo<=periodo_max) && (periodo>=periodo_min) && (periodo%periodo_min==0)){ //Condicional que valida variable
		valido=1; //Verdadero - variable=1
	}
	return valido; //Devuelve el valor de valido
}

/* Funci�n que genera el tren de se�ales cuadradas a un periodo determinado
Entradas: periodo - tiempo de la se�al en alta ("1 l�gico") y
 en baja ("0 l�gico")
ciclos - N�mero de se�ales que se generar�n
t_alta - tiempo en porcentaje del periodo que pondr� la se�al en "1 l�gico"
Salidas: Ninguna
*/
void genera_tren ( uint8_t t_alta, uint8_t ciclos, uint32_t periodo){
	uint8_t a; // Variable para contar parpadeos 
	uint32_t i; // Variable para retardos
	uint32_t cont; //Variable contar tiempo de se�al
	
	if ((valida_tiempo ( t_alta)) && (valida_pulsos (ciclos)) && (valida_periodo (periodo))){
		for(a=0; a<ciclos; a++){ //Ciclo for para repetir la se�al "ciclos" veces
			GPIO_PORTB_DATA_R |= 0x04;	//PB2 se pone en alta (1 logico")
			for(cont=0;cont<(periodo*t_alta/100);cont++){}	//Se mantiene en alta "t_alta" del "periodo"
  		GPIO_PORTB_DATA_R &=~(0x04); //PBS se pone en baja ("0 l�gico*")
			for(cont=0;cont<(periodo-periodo*t_alta/100);cont++){}  //Se mantiene en baja el tiempo del "periodo" restante
		}
	}else{
		// Parpadeos del led CIAN
		for(a=0; a<3; a++){ //Ciclo for para 3 parpadeos
			GPIO_PORTF_DATA_R |= ledAzul | ledVerde;  //Enciende led cian
			for(i = 0; i < 800000; i++); 	//Retraso
			GPIO_PORTF_DATA_R &= ~leds; 	//Apaga leds
			for(i = 0; i < 800000; i++);	//Retraso
		}
	}
}



// Funci�n principal
int main (void){
 
	// Definir variables
	uint8_t t_alta;
	uint32_t periodo;
	uint8_t ciclos;
	
	//Valores de las variables
	t_alta=50; 			//Valor multiplo de 10 entre 10 a 100
	ciclos=5; 		//Valor mayor a 5 y menor a 15
	periodo=periodo_min*2;	//Multiplo de 20000, menor a 2000000
	
	//Configurar Puertos
	config_portF(); // configura el puerto F
	config_portB(); // configura el puerto B

	//Ciclo
	while(1){
		if ((GPIO_PORTF_DATA_R & SW2) == 0){ //Se valida que el SW2 fue presionado
			while ((GPIO_PORTF_DATA_R & SW2) != SW2);		//Se espere se suelte el SW2
			genera_tren (t_alta, ciclos, periodo);   	//Funcion que genera el tren de se�ales
		}
	}//Fin While
}
