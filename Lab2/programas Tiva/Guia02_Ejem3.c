/****************************************************************************************/
/****************************************************************************************/
/*****                  PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ                    *****/
/*****                     FACULTAD DE CIENCIAS E INGENIERÍA                        *****/
/*****                            SISTEMAS DIGITALES                                *****/
/****************************************************************************************/
/*****          Tema:               Programacion de puertos de E/S con TivaES       *****/
/*****          Proyecto:           Lab2_ejemplo3                                   *****/
/****************************************************************************************/
/*****          Microcontrolador:   TM4C123GH6PM                                    *****/
/*****          EvalBoard:          Tiva C Series TM4C123G LaunchPad                *****/
/*****          Autor:              Rodia Santivañez Santivañez                     *****/
/*****          Fecha:              Abril 2020                                      *****/
/****************************************************************************************/
/***** Enunciado:                                                                   *****/
/***** Cuando se presiona SW1 se callcula un numero aleatorio entre 0 y 7 inclusive *****/
/***** Cuando se presiona SW2 se muestra el numero en el led RGB                    *****/
/****************************************************************************************/
/****************************************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "TivaES.h"

int main(void) {
    uint8_t color, aleatorio = 0;
    TivaES_Inicializa();

    while(1){
			TivaES_EsperaPresionarPulsador(SW1);
			
			do
			{
				aleatorio++;
				if(aleatorio == 8) aleatorio = 0;
			}while(TivaES_LeePulsador(SW1) == LOW);
			
			TivaES_EsperaPulsador(SW2);
			
			color = aleatorio<<1;
			
			TivaES_LedColor(color);
    }
}

/****************************************************************************************/
/*****              F  I  N     D  E  L     P  R  O  G  R  A  M  A                  *****/
/****************************************************************************************/
