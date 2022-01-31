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
	
T EQU 1000
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
	
	
Ciclo
	
	; ********** Lectura de los interruptores ***********
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x11 ; Lectura del interruptor SW1
	CMP R1, #0X01
	BEQ Led1 ; Si se ha presionado el interruptor SW1 ejecutar Led1
	
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x11 ; Lectura del interruptor SW2 y SW1
	CMP R1, #0X00
	BEQ Led3 ; Si se ha presionado el interruptor SW1 y SW2 ejecutar Led3
	
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	AND R1, #0x11 ; Lectura del interruptor SW2
	CMP R1, #0X10
	BEQ Led2 ; Si se ha presionado el interruptor SW2 ejecutar Led 2
	
	
	B Ciclo
	

; ****************************************************
Led1 ; ******************** CONTADOR **********************
	MOV R5, #T	
Led11
	;PRENDE LED ROJO
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	ORR R1, #0x06 ; Activar el MAGENTA
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
	SUB R5, #1
	sub R4, #1
	cmp R4, #0
	bne delay_out11

;APAGA LED ROJO
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	BIC R1, #0x06 ; Desactivar el Rojo
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
	SUB R5, #1
	sub R4, #1
	cmp R4, #0
	bne delay_out12


	;¿PASO TIEMPO T?
	CMP R5, #0
	BNE Led11 ; EJECUTA PARPADEO DE NUEVO
	
	B Ciclo

; ****************************************************
Led2 ; ******************** CONTADOR **********************
	MOV R5, #T	; TIEMPO T
	ADD R5, R5	;TIEMPO 2T
Led21
	;ENCIENDE LED AMARILLO
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	ORR R1, #0x0A ; Activar el AMARILLO
	STR R1, [R0,#DATA]
	
	;RETARDO
	; Retardo (lo suficiente para apreciarse el parpadeo)
	mov R4, #100

delay_out21
	mov R3, #10000
delay_in21
	sub R3, #1
	cmp R3, #0
	bne delay_in21
	SUB R5, #1
	sub R4, #1
	cmp R4, #0
	bne delay_out21

	;APAGAR LED AMARILLO
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	BIC R1, #0x0A ; Desactivar el Rojo
	STR R1, [R0,#DATA]

	
	; Retardo (lo suficiente para apreciarse el parpadeo)
	mov R4, #100

delay_out22
	mov R3, #10000
delay_in22
	sub R3, #1
	cmp R3, #0
	bne delay_in22
	SUB R5, #1
	sub R4, #1
	cmp R4, #0
	bne delay_out22

	;¿ACABO TIEMPO 2T?
	CMP R5, #0
	BNE Led21 ; EJECUTA PARPADEO DE NUEVO
	
	B Ciclo



; ****************************************************
Led3 ; ******************** CONTADOR **********************
	MOV R5, #T	
	ADD R5, R5
	ADD R5,R5

Led31
	;PRENDE LED CIAN
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	ORR R1, #0x0C ; Activar el CIAN
	STR R1, [R0,#DATA]
	
	;RETARDO
	; Retardo (lo suficiente para apreciarse el parpadeo)
	mov R4, #100

delay_out31
	mov R3, #10000
delay_in31
	sub R3, #1
	cmp R3, #0
	bne delay_in31
	SUB R5, #1
	sub R4, #1
	cmp R4, #0
	bne delay_out31

	;APAGA LED CIAN
	LDR R0, GPIO_PORTF_BASE ; Dirección Base del puerto F
	LDR R1, [R0,#DATA] ; Registro de Datos del puerto F
	BIC R1, #0x0C ; Desactivar el CIAN
	STR R1, [R0,#DATA]

	;RETARDO
	; Retardo (lo suficiente para apreciarse el parpadeo)
	mov R4, #100

delay_out32
	mov R3, #10000
delay_in32
	sub R3, #1
	cmp R3, #0
	bne delay_in32
	SUB R5, #1
	sub R4, #1
	cmp R4, #0
	bne delay_out32

	;¿SE CUMPLE EL TIEMPO 3T?
	CMP R5, #0
	BNE Led31 ; PARPADEA LED CIAN DE NUEVO
	
	B Ciclo

; ****************************************************

RCGCGPIO DCD 0x400FE608
PRGPIO DCD 0x400FEA08
GPIO_PORTF_BASE DCD 0x40025000
KEYLOCK DCD 0x4C4F434B

	END
	