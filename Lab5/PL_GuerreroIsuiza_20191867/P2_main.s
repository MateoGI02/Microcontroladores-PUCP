; ***********************************************************************************
; Nombre : Lab5 - Pregunta 1 GuerreroIsuiza
; Simulador : Periferico Texas Port F, en keil uVision4 Ver. 4.74 
; Descripción : Leer 2 interruptores y encender 1 led de acuerdo al enunciado
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
	
	
Start2
	MOV R2, #1 ; Inicializando el registro que se usara como escalador del retardo

Ciclo
	
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x10 ; Lectura del interruptor SW1
	CMP R1, #0X10
	BNE Contador ; Si se ha presionado el interruptor SW1 ejecutar Lazo1
	
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x01 ; Lectura del interruptor SW2
	CMP R1, #0X01
	BNE Led ; Si se ha presionado el interruptor SW2 ejecutar Lazo 2

	B Ciclo
	
; ****************************************************
Led ; ******************** CONTADOR **********************	
Led11
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x01 ; Lectura del interruptor SW1
	CMP R1, #0X01
	BNE Led11 ; Esperar que se suelte el interruptor SW1
	
	;PRENDE LED ROJO
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	ORR R1, #0x02 ; Activar el MAGENTA
	STR R1, [R0,#DATA]
	
	;RETARDO
	; Retardo (lo suficiente para apreciarse el parpadeo)
	mov R4, #100

delay_out11
	mov R3, #10000
delay_in11
	sub R3, #1
	cmp R3, #0
	bne delay_in11
	sub R4, #1
	cmp R4, #0
	bne delay_out11

;APAGA LED ROJO
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	BIC R1, #0x02 ; Desactivar el Rojo
	STR R1, [R0,#DATA]

	;RETARDO
	; Retardo (lo suficiente para apreciarse el parpadeo)
	mov R4, #100

delay_out12
	mov R3, #10000
delay_in12
	sub R3, #1
	cmp R3, #0
	bne delay_in12
	sub R4, #1
	cmp R4, #0
	bne delay_out12
	SUB R2, #1

	;¿PASO T veces?
	CMP R2, #0
	BNE Led11 ; EJECUTA PARPADEO DE NUEVO


	B Ciclo

; ****************************************************
Contador ; ******************** CONTADOR **********************
	ADD R2,#1
Lazo1b
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x10 ; Lectura del interruptor SW1
	CMP R1, #0X10
	BNE Lazo1b ; Esperar que se suelte el interruptor SW1
	CMP R2, #15
	BEQ Start2
	
	B Ciclo

; ****************************************************

RCGCGPIO DCD 0x400FE608
PRGPIO DCD 0x400FEA08
GPIO_PORTF_BASE DCD 0x40025000
KEYLOCK DCD 0x4C4F434B

	END
	