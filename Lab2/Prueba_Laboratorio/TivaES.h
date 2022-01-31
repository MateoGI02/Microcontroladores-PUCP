/****************************************************************************************/
/****************************************************************************************/
/*****                  PONTIFICIA UNIVERSIDAD CATÓLICA DEL PERÚ                    *****/
/*****                     FACULTAD DE CIENCIAS E INGENIERÍA                        *****/
/*****                            SISTEMAS DIGITALES                                *****/
/****************************************************************************************/
/*****          Archivo:            TivaES.h                                        *****/
/*****          Microcontrolador:   TM4C123GH6PM                                    *****/
/*****          Módulo(s) uC:       GPIO PORTF                                      *****/
/*****          EvalBoard:          Tiva C Series TM4C123G LaunchPad                *****/
/*****          Autor:              Rolando Sánchez Ponce                           *****/
/*****          Fecha:              Abril 2018                                      *****/
/*****          Última revisión:    Marzo 2019                                      *****/
/****************************************************************************************/
/***** Funciones para el control del led RGB y de los pulsadores SW1 y SW2 de la    *****/
/***** tarjeta de evaluación EK-TM4C123GXL.                                         *****/
/****************************************************************************************/
/*****  Conexiones:                                                                 *****/
/*****          Led rojo        -       PF1                                         *****/
/*****          Led azul        -       PF2                                         *****/
/*****          Led verde       -       PF3                                         *****/
/*****          Pulsador SW1    -       PF4                                         *****/
/*****          Pulsador SW2    -       PF0                                         *****/
/****************************************************************************************/
/****************************************************************************************/
#ifndef _TIVAES_H_
#define _TIVAES_H_

/****************************************************************************************/
/*****                      C  O  N  S  T  A  N  T  E  S                            *****/
/****************************************************************************************/

/*****              CONSTANTES BOOLEANAS                                            *****/
#ifndef FALSE
  #define FALSE 0
#endif
#ifndef TRUE
  #define TRUE  1
#endif
#ifndef LOW
  #define LOW   0
#endif
#ifndef HIGH
  #define HIGH  1
#endif

/*****              CONSTANTES PARA MÁSCARAS DE LOS PULSADORES                      *****/
#define SW2         0x00000001      // Pulsador conectado a PF0
#define SW1         0x00000010      // Pulsador conectado a PF4

/*****              CONSTANTES PARA COLORES DEL LED RGB                             *****/
#define ROJO        0x00000002      // Led conectado a PF1
#define AZUL        0x00000004      // Led conectado a PF2
#define VERDE       0x00000008      // Led conectado a PF3
#define MAGENTA     (ROJO|AZUL)
#define AMARILLO    (ROJO|VERDE)
#define CIAN        (AZUL|VERDE)
#define BLANCO      (ROJO|VERDE|AZUL)
#define NEGRO       0

/*****              CONSTANTES PARA ESTADOS DE PULSADORES                           *****/
#define PRESIONADO      LOW
#define NO_PRESIONADO   HIGH

/*****              CONSTANTES PARA RETARDOS                                        *****/
                            // Número de iteraciones del bucle for para retardo:
#define retRebote       10000   // evita falsas lecturas por rebote mecánico.
#define retParpadeoON   500000  // permanece encendido el led cuando parpadea.
#define retParpadeoOFF  300000  // permanece apagado el led cuando parpadea.

/****************************************************************************************/
/*****          P R O T O T I P O S   D E    F U N C I O N E S                      *****/
/****************************************************************************************/

/****************************************************************************************/
/*****                      TivaES_Inicializa()                                     *****/
/****************************************************************************************/
/***** Configura los pines del microcontrolador de la tarjeta TIVA que controlan    *****/
/***** los tres leds (rojo, verde y azul) y los dos pulsadores (SW1 y SW2).         *****/
/****************************************************************************************/
/***** ENTRADAS:    Ninguna                                                         *****/
/***** SALIDA:      Ninguna                                                         *****/
/****************************************************************************************/
void TivaES_Inicializa(void);

/****************************************************************************************/
/*****                      TivaES_LedColor()                                       *****/
/****************************************************************************************/
/***** Enciende el led RGB de la tarjeta TIVA con el color indicado en la entrada.  *****/
/****************************************************************************************/
/***** ENTRADAS:                                                                    *****/
/*****    color:    Puede ser una de las ocho constantes para colores definidas     *****/
/*****              previamente: ROJO, AZUL, VERDE, MAGENTA, AMARILLO, CIAN, BLANCO *****/
/*****              o NEGRO.                                                        *****/
/***** SALIDA:      Ninguna                                                         *****/
/****************************************************************************************/
void TivaES_LedColor(uint8_t color);

/****************************************************************************************/
/*****                      TivaES_LedParpadeo()                                    *****/
/****************************************************************************************/
/***** Parpadea el led RGB de la tarjeta TIVA con el color y las veces indicados en *****/
/***** las entradas.                                                                *****/
/****************************************************************************************/
/***** ENTRADAS:                                                                    *****/
/*****    color:    Puede ser uno de las ocho constantes para colores definidas     *****/
/*****              previamente.                                                    *****/
/*****    veces:    Las veces que se emitirá un parpadeo por el led.                *****/
/***** SALIDA:      Ninguna                                                         *****/
/****************************************************************************************/
void TivaES_LedParpadeo(uint8_t color, uint8_t veces);

/****************************************************************************************/
/*****                      TivaES_LeePulsador()                                    *****/
/****************************************************************************************/
/***** Devuelve el nivel lógico en el pin solicitado en la entrada.                 *****/
/****************************************************************************************/
/***** ENTRADAS:                                                                    *****/
/*****    pulsadorMsk:  Máscara del pulsador a leer.                                *****/
/*****                  Puede ser SW1 (0x10) o SW2(0x01).                           *****/
/***** SALIDA:  PRESIONADO (o LOW):     Pulsador está presionado.                   *****/
/*****          NO_PRESIONADO (o HIGH): Pulsador no está presionado.                *****/
/****************************************************************************************/
uint8_t TivaES_LeePulsador(uint8_t pulsadorMsk);

/****************************************************************************************/
/*****                      TivaES_EsperaPresionarPulsador()                        *****/
/****************************************************************************************/
/***** Espera que se presione el pulsador conectado al pin especificado en la en-   *****/
/***** trada de la función. No se sale de la función hasta que el pulsador esté     *****/
/***** presionado.                                                                  *****/
/****************************************************************************************/
/***** ENTRADAS:                                                                    *****/
/*****    pulsadorMsk:  Máscara del pulsador a analizar.                            *****/
/*****                  Puede ser SW1 (0x10) o SW2(0x01).                           *****/
/***** SALIDA:  Ninguna.                                                            *****/
/****************************************************************************************/
void TivaES_EsperaPresionarPulsador(uint8_t pulsadorMsk);

/****************************************************************************************/
/*****                      TivaES_EsperaSoltarPulsador()                           *****/
/****************************************************************************************/
/***** Espera que se suelte el pulsador conectado al pin especificado en la entra-  *****/
/***** da de la función. No se sale de la función hasta que el pulsador no esté     *****/
/***** presionado.                                                                  *****/
/****************************************************************************************/
/***** ENTRADAS:                                                                    *****/
/*****    pulsadorMsk:  Máscara del pulsador a analizar.                            *****/
/*****                  Puede ser SW1 (0x10) o SW2(0x01).                           *****/
/***** SALIDA:  Ninguna.                                                            *****/
/****************************************************************************************/
void TivaES_EsperaSoltarPulsador(uint8_t pulsadorMsk);

/****************************************************************************************/
/*****                      TivaES_EsperaPulsador()                                 *****/
/****************************************************************************************/
/***** Espera que se presione y se suelte el pulsador conenctado al pin especifica- *****/
/***** do en la entrada de la función.                                              *****/
/****************************************************************************************/
/***** ENTRADAS:                                                                    *****/
/*****    pulsadorMsk:  Máscara del pulsador a analizar.                            *****/
/*****                  Puede ser SW1 (0x10) o SW2(0x01).                           *****/
/***** SALIDA:  Ninguna.                                                            *****/
/****************************************************************************************/
void TivaES_EsperaPulsador(uint8_t pulsadorMsk);

/****************************************************************************************/
/*****                      TivaES_SondeaPulsador()                                 *****/
/****************************************************************************************/
/***** Revisa si se ha producido el evento de presionar el pulsador conectado al    *****/
/***** pin especificado en la entrada de la función. Si se ha presionado devuelve   *****/
/***** verdadero (TRUE) caso contrario devuelve falso (FALSE). Solo devuelve TRUE   *****/
/***** si es que el pulsador está presionado y la vez anterior que se llamó a esta  *****/
/***** función el pulsador no estaba presionado. No hay lazo de espera.             *****/
/****************************************************************************************/
/***** ENTRADAS:                                                                    *****/
/*****    pulsadorMsk:  Máscara del pulsador a analizar.                            *****/
/*****                  Puede ser SW1 (0x10) o SW2(0x01).                           *****/
/*****    *antNivelPtr: Dirección de memoria que almacena el anterior nivel lógico  *****/
/*****                  del pin a analizar.                                         *****/
/***** SALIDA:  TRUE:   Se ha presionado el pulsador.                               *****/
/*****          FALSE:  No se ha presionado el pulsador.                            *****/
/****************************************************************************************/
uint8_t TivaES_SondeaPulsador(uint8_t pulsadorMsk, uint8_t *antNivelPtr);

/****************************************************************************************/
/*****                      TivaES_EsperaPulsadores()                               *****/
/****************************************************************************************/
/***** Espera que se presione y se suelte alguno o los dos pulsadores.              *****/
/***** Devuelve información sobre el(los) pulsador(es) presionado(s).               *****/
/****************************************************************************************/
/***** ENTRADAS:    Ninguna.                                                        *****/
/***** SALIDA:      1:  Pulsador SW1 presionado y soltado.                          *****/
/*****              2:  Pulsador SW2 presionado y soltado.                          *****/
/*****              3:  Pulsadores SW1 y SW2 presionados y soltados.                *****/
/****************************************************************************************/
uint8_t TivaES_EsperaPulsadores(void);

/****************************************************************************************/
/*****                      TivaES_SolicitaNumero()                                 *****/
/****************************************************************************************/
/***** Solicita el ingreso de un número de numDig dígitos por medio de los pulsado- *****/
/***** res SW1 y SW2. Se ingresa dígito por dígito empezando por el más significa-  *****/
/***** tivo. Para ingresar un dígito se presiona SW1 tantas veces como el dígito a  *****/
/***** a ingresar y luego SW2. Si la opción ledFlag está habilitada se produce un   *****/
/***** parpadeo azul cada vez que se presiona SW1, verde cada vez que se presiona   *****/
/***** SW2 y rojo cuando se termina de ingresar todo el número.                     *****/
/****************************************************************************************/
/***** ENTRADAS:                                                                    *****/
/*****    numDig:   Cantidad de dígitos del número solicitado. Rango [1, 5].        *****/
/*****    ledFlag:  TRUE:   Parpadeo del led RGB                                    *****/
/*****              FALSE:  No se escribe en el led RGB.                            *****/
/***** SALIDA:      Número ingresado por el usuario. Rango [0, 65535].              *****/
/****************************************************************************************/
uint16_t TivaES_SolicitaNumero(uint8_t numDig, uint8_t ledFlag);

#endif

/****************************************************************************************/
/*****              F  I  N     D  E  L     A  R  C  H  I  V  O                     *****/
/****************************************************************************************/
