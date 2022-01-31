/****************************************************************************************/ 
/****************************************************************************************/ 
/***** PONTIFICIA UNIVERSIDAD CAT�LICA DEL PER� *****/ 
/***** FACULTAD DE CIENCIAS E INGENIER�A *****/ 
/***** SISTEMAS DIGITALES *****/ 
/****************************************************************************************/ 
/***** Tema: Introducci�n al entorno Keil uVision4 *****/ 
/***** Proyecto: IP_Sum_Res_Leds *****/ 
/****************************************************************************************/ 
/***** Microcontrolador: TM4C123GH6PM *****/ 
/***** EvalBoard: Tiva C Series TM4C123G LaunchPad *****/ 
/***** Autor: Guerrero Isuiza / Mateo *****/ 
/***** Fecha: Septiembre 2020 *****/ 
/****************************************************************************************/ 
/***** Enunciado: Se genera un parpadeo en los leds RGB seg�n los Switch. *****/ 
/***** *****/ 
/****************************************************************************************/


#include <stdint.h> 
#include "tm4c123gh6pm.h"
#include "TivaES.h"

int main(void) 
{ 
	uint8_t num1, num2, valor;
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // Activamos el Reloj de Puerto F 
	while( (SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R5)==0){} // Se espera a que se active el reloj
	TivaES_Inicializa();  //Iniciamos Puerto F
	while (1){  //Abrimos un bucle
 		num1 = TivaES_SolicitaNumero(2, TRUE);  //Se solicita el n�mero 1 de 2 d�gitos
		TivaES_LedParpadeo (ROJO, num1-1);			// El led ROJO parpadea n�mero1 -1 veces (se le resta 1 porque el led se prende 1 vez de m�s del valor de num1)
		num2 = TivaES_SolicitaNumero(2, TRUE); 	//Se solicita el n�mero 2 de 2 d�gitos
		TivaES_LedParpadeo (ROJO,num2-1);				// El led ROJO se parpadea n�mero2 -1 veces (se le resta 1 porque el led se prende 1 vez de m�s del valor de num2)
		switch (TivaES_EsperaPulsadores())			// Elecci�n seg�n el valor de switch que se presiona
		{
			case 1:
				TivaES_LedParpadeo (AMARILLO,num1+num2); // El led AMARILLO parmadea n�mero1 + n�mero2 veces
				break;   		//Se sale del proceso Switch
			case 2:
				if (num1>num2) 	 //Condicional num1>num2
					valor=num1-num2; 	//Valor es igual a num1 - num2 si num1 es mayor a num2
				else 
					valor=num2-num1;  //Valor es igual a num2 - num1 si num2 es mayor a num1
				TivaES_LedParpadeo (MAGENTA,valor);		// El led AMARILLO parmadea valor veces
				break; 	//Se sale del proceso Switch
		}
	}	
}
