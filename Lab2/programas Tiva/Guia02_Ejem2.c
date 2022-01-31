/****************************************************************************************/
/****************************************************************************************/
/*****                  PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ                    *****/
/*****                     FACULTAD DE CIENCIAS E INGENIERÍA                        *****/
/*****                            SISTEMAS DIGITALES                                *****/
/****************************************************************************************/
/*****          Tema:               Introducción al entorno CodeComposer            *****/
/*****          Proyecto:           Lab01Proy03                                     *****/
/****************************************************************************************/
/*****          Microcontrolador:   TM4C123GH6PM                                    *****/
/*****          EvalBoard:          Tiva C Series TM4C123G LaunchPad                *****/
/*****          Autor:              Rolando Sánchez Ponce                           *****/
/*****          Fecha:              Marzo 2019                                      *****/
/****************************************************************************************/
/***** Enunciado:                                                                   *****/
/***** El led RGB toma los colores rojo, azul y verde alternadamente, para cambiar  *****/
/***** de color se debe presionar tres veces el pulsador SW2.                       *****/
/****************************************************************************************/
/****************************************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "TivaES.h"

int main(void) {
    uint8_t pulsos, color;
    TivaES_Inicializa();
    color = ROJO;

    while(1){

        TivaES_LedColor(color);             // Encendemos el led
        for(pulsos=0; pulsos<3; pulsos++){
            TivaES_EsperaPulsador(SW2);     // Estructura for que espera que se
        }                                   // presione tres veces el pulsador SW2

        if(color == ROJO)                   // Estrucutura if..else if..else
            color = AZUL;                   // que actualiza la variable color
        else if(color == AZUL)
            color = VERDE;
        else
            color = ROJO;
    }
}

/****************************************************************************************/
/*****              F  I  N     D  E  L     P  R  O  G  R  A  M  A                  *****/
/****************************************************************************************/
