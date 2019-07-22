; Program for project1 (familiarization) 

;;;;;;; Program hierarchy ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Mainline
;   Initial
;   
;
;;;;;;; Assembler directives ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        list  P=PIC18F452, F=INHX32, C=160, N=0, ST=OFF, MM=OFF, R=DEC, X=ON
        #include P18F452.inc
	;HS oscillator, Reset
	CONFIG OSC = HS , OSCS = OFF , PWRT = ON , BOR = ON , BORV = 42
	;Watchdog timer disabled
	CONFIG WDT = OFF , WDTPS = 128 , CCP2MUX = ON
	;RB5 enabled for I/O
	CONFIG STVR = ON , LVP = OFF , DEBUG = OFF

;;;;;;; Variables ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
								; Variables are 8-bit long each
        cblock  0x000           ;Beginning of Access RAM
        IN_B
		endc

;;;;;;; Macro definitions ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

MOVLF   macro  literal,dest
        movlw  literal
        movwf  dest
        endm

;;;;;;; Vectors ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        org  0x0000             ;Reset vector
        nop
        goto  Mainline

        org  0x0008             ;High priority interrupt vector
        goto  $                 ;Trap

        org  0x0018             ;Low priority interrupt vector
        goto  $                 ;Trap

;;;;;;; Mainline program ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Mainline
        rcall  Initial          ;Initialize everything
Loop
; add your code here
        bra  Loop


;;;;;;; Initial subroutine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This subroutine performs all initializations of variables and registers.

Initial
        MOVLF  B'10001110',ADCON1  ;Enable PORTA & PORTE digital I/O pins
        MOVLF  B'11100001',TRISA  ;Set I/O for PORTA
        MOVLF  B'00000000',TRISB  ;Set I/O for PORTB
        MOVLF  B'00000000',TRISC  ;Set I/0 for PORTC
        MOVLF  B'00000000',TRISD  ;Set I/O for PORTD
        MOVLF  B'00000000',TRISE  ;Set I/O for PORTE
        MOVLF  B'10001000',T0CON  ;Set up Timer0 for a looptime of 10 ms
        MOVLF  B'00010100',PORTA  ;Turn off all four LEDs driven from PORTA
	MOVLF  B'11111111',PORTB  ;Send input of 1 to all parts of Ports B, C, D, and E.
	MOVLF  B'11111111',PORTC
	MOVLF  B'11111111',PORTD
	MOVLF  B'11111111',PORTE
        return
 end