/****************************************************************************************/
/****************************************************************************************/
/*****                  PONTIFICIA UNIVERSIDAD CAT�LICA DEL PER�                    *****/
/*****                     FACULTAD DE CIENCIAS E INGENIER�A                        *****/
/*****                            SISTEMAS DIGITALES                                *****/
/****************************************************************************************/
/*****          Tema:               Introducci�n al entorno CodeComposer            *****/
/*****          Proyecto:           Lab02Proy01                                     *****/
/****************************************************************************************/
/*****          Microcontrolador:   TM4C123GH6PM                                    *****/
/*****          EvalBoard:          Tiva C Series TM4C123G LaunchPad                *****/
/*****          Autor:              Rolando S�nchez Ponce                           *****/
/*****          Fecha:              Abril 2019                                      *****/
/****************************************************************************************/
/***** Enunciado:                                                                   *****/
/***** El programa calcula el promedio de tres notas sin considerar la menor de     *****/
/***** ellas. El usuario ingresa las tres notas mediante los pulsadores y visualiza *****/
/***** el promedio en el led RGB.                                                   *****/
/****************************************************************************************/
/****************************************************************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "TivaES.h"

/****************************************************************************************/
/*****                              numMenor3()                                     *****/
/****************************************************************************************/
/***** Determina el menor de tres n�meros.                                          *****/
/****************************************************************************************/
/***** ENTRADAS:    a, b, c: Tres n�meros de 8 bits.                                *****/
/***** SALIDA:      El menor de los tres n�meros                                    *****/
/****************************************************************************************/
uint8_t numMenor3(uint8_t a, uint8_t b, uint8_t c){
	uint8_t menor;
	if((a<=b)&&(a<=c)) menor = a;
	else if((b<=a)&&(b<=c)) menor = b;
	else menor = c;
	return menor;
}

/****************************************************************************************/
/*****            P  R  O  G  R  A  M  A     P  R  I  N  C  I  P  A  L              *****/
/****************************************************************************************/
int main(void) {
	uint8_t nota1, nota2, nota3, promedio, notaMenor;
	TivaES_Inicializa();
	while(1){
		nota1 = TivaES_SolicitaNumero(2, TRUE);
		nota2 = TivaES_SolicitaNumero(2, TRUE);
		nota3 = TivaES_SolicitaNumero(2, TRUE);
		notaMenor = numMenor3(nota1, nota2, nota3);
		promedio = (nota1 + nota2 + nota3 - notaMenor)/2;
		TivaES_LedParpadeo(BLANCO, promedio);
	}
}

/****************************************************************************************/
/*****              F  I  N     D  E  L     P  R  O  G  R  A  M  A                  *****/
/****************************************************************************************/
