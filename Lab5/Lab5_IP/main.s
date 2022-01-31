; ***********************************************************************************
; Nombre : Lab5 - Informe Previo GuerreroIsuiza
; Simulador : Periferico Texas Port F, en keil uVision4 Ver. 4.74 
; Descripción : Leer 1 interruptor y encender 1 led de acuerdo al enunciado del informe previo
; Autor : Mateo Guerrero Isuiza
; Fecha : Semestre 2021-2
; Conexiones : SW1 --> PF4
; : SW2 --> PF0
; : LedRojo --> PF1
; : LedAzul --> PF2
; : LedVerde --> PF3
; : Amarillo --> ROJO | VERDE
; : Blanco --> ROJO | VERDE | AZUL
; : Magenta --> ROJO | AZUL
;************************************************************************************
; DIRECCIONES DE DESPLAZAMIENTO (OFFSET)
; ----------------------------------------
DATA EQU 0x3FC
DIR EQU 0x400
AFSEL EQU 0x420
DR8R EQU 0x508
PUR EQU 0x510
DEN EQU 0x51C
LOCK EQU 0x520
CR EQU 0x524
; ----------------------------------------
	AREA main, READONLY, CODE
	THUMB ; Uso de instrucciones THUMB
	EXPORT Start

Start
	;********** ACTIVAR EL RELOJ DEL PUERTO F *************
	LDR R0, RCGCGPIO ; Registro para Habilitar los relojes de los puertos GPIO
	LDR R1, [R0]
	ORR R1, #0x20 ; Seleccionando el reloj del Puerto F
	STR R1, [R0] ; Cargando el registro con la configuración
	
	NOP ; Esperar que el reloj del puerto F se active
	NOP ; NOP = No Operation
	NOP
	NOP
	;*******************************************************
	;************ CONFIGURANDO EL PUERTO F *****************
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, KEYLOCK ; Offset de la llave
	STR R1, [R0,#LOCK] ; Cargando la llave en el registro correspondiente
	
	LDR R1, [R0,#CR]
	ORR R1, #0x01
	STR R1, [R0,#CR] ; Desbloqueando PF0
	
	LDR R1, [R0,#DIR] ; Registro de configuración de los puertos de Entrada y Salida
	ORR R1, #0x0E ; Definiendo puerto de Salida, led ROJO (PF1), led AZUL (PF2) y led Verde (PF3) 
	BIC R1, #0x11 ; Definiendo los puertos de Entrada, SW1(PF4) y SW2(PF0)
	STR R1, [R0,#DIR] ; Cargando el registro con la configuración
	
	LDR R1, [R0,#DEN] ; Registro de habilitación de puerto Digital
	ORR R1, #0x1F ; Habilitar los bits del puerto F que serán usados
	STR R1, [R0,#DEN] ; Cargando el registro con la configuración
	
	LDR R1, [R0,#DR8R] ; Registro de Habilitación del driver de 8mA
	ORR R1, #0x0E ; Puerto de Salida con el driver de 8mA
	STR R1, [R0,#DR8R] ; Cargando el registro con la configuración
	
	LDR R1, [R0,#PUR] ; Registro de Habilitación de las resistencias internas de "pull-up"
	ORR R1, #0x11 ; Puertos de Entrada con la resistencia de "pull-up"
	STR R1, [R0,#PUR] ; Cargando el registro con la configuración
	
	LDR R1, [R0,#AFSEL] ; Registro de Funciones Alternas
	BIC R1, #0x1F ; Deshabilitar funciones alternas en los puertos digítales
	STR R1, [R0,#AFSEL] ; Cargando el registro con la configuración
	
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	BIC R1, #0x0E ; Apagando todos los leds
	STR R1, [R0,#DATA] ; Cargando el registro con la configuración
	
	; *********** FIN DE LA CONFIGURACIÓN DEL PUERTO F **************

Retardo EQU 100  ; Contador que se usará para generar una base de tiempo
	MOV R3, #1 ; Opción para activar o desactivar el led
	MOV R5, #1 


CICLO
	CMP R5, #1
	BEQ AMARILLO
	
	CMP R5, #2
	BEQ MAGENTA
	
	CMP R5, #3
	BEQ BLANCO
	
	B CICLO

AMARILLO
	; ********** Lectura 1 de los interruptores ***********
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x10 ; Lectura del interruptor SW1
	CMP R1, #0X10
	BNE SW1_1 ; Si se ha presionado el interruptor SW1 ejecutar SW1_1


	; *****************************************************
	
MAGENTA
	; ********** Lectura 1 de los interruptores ***********
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x10 ; Lectura del interruptor SW1
	CMP R1, #0X10
	BNE SW1_2 ; Si se ha presionado el interruptor SW1 ejecutar SW1_1
	MOV R5, #3
	
	; *****************************************************
	
BLANCO
	; ********** Lectura 1 de los interruptores ***********
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x10 ; Lectura del interruptor SW1
	CMP R1, #0X10
	BNE SW1_1 ; Si se ha presionado el interruptor SW1 ejecutar SW1_1
	MOV R5, #1
	

	; *****************************************************

SW1_1
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	ORR R1, #0x0A ; Activar el ROJO
	STR R1, [R0,#DATA]

	MOV R5, #0 ; Iniciamos el contador del retardo
Ret1b
	ADD R5, #1 ; Incrementando el contador del retardo
	CMP R5, #Retardo ; Retardo seleccionado
	BNE Ret1b
	
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	BIC R1, #0x0A ; Desactivar el Rojo
	STR R1, [R0,#DATA]
	
	MOV R5, #2
	
	B CICLO
; ****************************************************

SW1_2
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	ORR R1, #0x06 ; Activar el ROJO
	STR R1, [R0,#DATA]

	MOV R5, #0 ; Iniciamos el contador del retardo
Ret1b2
	ADD R5, #1 ; Incrementando el contador del retardo
	CMP R5, #Retardo ; Retardo seleccionado
	BNE Ret1b
	
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	BIC R1, #0x06 ; Desactivar el Rojo
	STR R1, [R0,#DATA]
	
	MOV R5, #3
	
	B CICLO
; ****************************************************

SW1_3
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	ORR R1, #0x0E ; Activar el ROJO
	STR R1, [R0,#DATA]

	MOV R5, #0 ; Iniciamos el contador del retardo
Ret1b3
	ADD R5, #1 ; Incrementando el contador del retardo
	CMP R5, #Retardo ; Retardo seleccionado
	BNE Ret1b
	
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	BIC R1, #0x0E ; Desactivar el Rojo
	STR R1, [R0,#DATA]
	
	MOV R5, #1
	
	B CICLO
	

; ****************************************************
RCGCGPIO DCD 0x400FE608
PRGPIO DCD 0x400FEA08
GPIO_PORTF_BASE DCD 0x40025000
KEYLOCK DCD 0x4C4F434B

	END
	