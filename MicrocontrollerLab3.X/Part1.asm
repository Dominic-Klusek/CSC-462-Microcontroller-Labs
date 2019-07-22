; Dominic Klusek
; Samantha Rivera

;;;;;;; P1 for QwikFlash board ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Use 10 MHz crystal frequency.
; Use Timer0 for ten millisecond looptime.
; Blink "Alive" LED every two and a half seconds.
; Toggle C2 output every ten milliseconds for measuring looptime precisely.
;
;;;;;;; Program hierarchy ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Mainline
;   Initial
;   BlinkAlive
;   LoopTime
;
;;;;;;; Assembler directives ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        list  P=PIC18F452, F=INHX32, C=160, N=0, ST=OFF, MM=OFF, R=DEC, X=ON
        #include P18F452.inc
	;HS oscillator, Reset
	CONFIG OSC = HS , OSCS = OFF , PWRT = ON , BOR = ON , BORV = 42
	;Watchdog timer disabled
	CONFIG WDT = OFF , WDTPS = 128 , CCP2MUX = ON
	;RB5 enabled for I/O
	CONFIG STVR = ON , LVP = OFF , DEBUG = ON

;;;;;;; Variables ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        cblock  0x000           ;Beginning of Access RAM
        TMR0LCOPY               ;Copy of sixteen-bit Timer0 used by LoopTime
        TMR0HCOPY
        INTCONCOPY              ;Copy of INTCON for LoopTime subroutine
        ALIVECNT                ;Counter for blinking "Alive" LED
	BorC			;flag to determine if Port B or Port C are being used
	currentLED
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
	rcall Display
	rcall Delay
        bra  Loop

;;;;;;; Initial subroutine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This subroutine performs all initializations of variables and registers.

Initial
        MOVLF  B'10001110',ADCON1  ;Enable PORTA & PORTE digital I/O pins
        MOVLF  B'11100001',TRISA  ;Set I/O for PORTA
        MOVLF  B'00000000',TRISB  ;Set I/O for PORTB
        MOVLF  B'00000000',TRISC  ;Set I/0 for PORTC
        MOVLF  B'00001111',TRISD  ;Set I/O for PORTD
        MOVLF  B'00000000',TRISE  ;Set I/O for PORTE
        MOVLF  B'10001000',T0CON  ;Set up Timer0 for a looptime of 10 ms
        MOVLF  B'00010000',PORTA  ;Turn off all four LEDs driven from PORTA
	MOVLF  0x01, currentLED	  ;set current LED to be the leftmost one
	MOVLF  0x00, BorC	  ;using Port B first
	MOVLF  0x00, PORTB
	MOVLF  0x00, PORTC
        return

;;;;;;; LoopTime subroutine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This subroutine waits for Timer0 to complete its ten millisecond count
; sequence. It does so by waiting for sixteen-bit Timer0 to roll over. To obtain
; a period of precisely 10000/0.4 = 25000 clock periods, it needs to remove
; 65536-25000 or 40536 counts from the sixteen-bit count sequence.  The
; algorithm below first copies Timer0 to RAM, adds "Bignum" to the copy ,and
; then writes the result back to Timer0. It actually needs to add somewhat more
; counts to Timer0 than 40536.  The extra number of 12+2 counts added into
; "Bignum" makes the precise correction.

Bignum  equ     65536-25000+12+2

LoopTime
        btfss  INTCON,TMR0IF    ;Wait until ten milliseconds are up
        bra  LoopTime
        movff  INTCON,INTCONCOPY  ;Disable all interrupts to CPU
        bcf  INTCON,GIEH
        movff  TMR0L,TMR0LCOPY  ;Read 16-bit counter at this moment
        movff  TMR0H,TMR0HCOPY
        movlw  low  Bignum
        addwf  TMR0LCOPY,F
        movlw  high  Bignum
        addwfc  TMR0HCOPY,F
        movff  TMR0HCOPY,TMR0H
        movff  TMR0LCOPY,TMR0L  ;Write 16-bit counter at this moment
        movf  INTCONCOPY,W      ;Restore GIEH interrupt enable bit
        andlw  B'10000000'
        iorwf  INTCON,F
        bcf  INTCON,TMR0IF      ;Clear Timer0 flag
        return

;;;;;;; Delay subroutine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
Delay
        MOVLF  100,ALIVECNT     ;Reinitialize BLNKCNT
	btg  PORTC,RC2          ;Toggle pin, to support measuring loop time
DelayLoop
	rcall LoopTime
	decf  ALIVECNT,F        ;Decrement loop counter and return if not zero
	bnz DelayLoop
	return
	
	
;;;;;;; Display subroutine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; 

Display
	movf BorC, F		;check which PORT we are using
	bnz CSect   
BSect
	movff currentLED, PORTB ;move current LED to PORTB
	
	movf currentLED, W	;set flag which port we are using
	andlw B'00111111'	;test to see if  need to switch PORTS
	bnz BChange		;if we don't need to change PORTS yet, just rotate
	rcall PortChange	;call subroutine to change PORT
CSect
	movff currentLED, PORTC ;move current LED to PORTC
	
	movf currentLED, W	;move current LED to WREG for testing
	andlw B'00000011'	;test to see if we need to switch PORTS
	
	bnz BChange		;if we don't jump to the end
	rcall PortChange	;call subroutine to change Port
	bra BSect
BChange
	rlncf currentLED, F	;rotate the bit
BAend
        return
;;;;;;; PortChange subroutine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; 
PortChange
	movf BorC, F		;check which PORT we are using
	bnz ChangeSect  
	MOVLF 0x00, PORTB	;change to using PORT C
	MOVLF 0x01, BorC	;set the variable  to see if use port B or port C
	MOVLF 0x01, currentLED	;reset to 1 so that when value is passed to Port C it works
	bra ChangeEnd
ChangeSect
	MOVLF 0x00, PORTC	;clear PORT C
	MOVLF 0x00, BorC	;change to using PORT B
	MOVLF 0x01, currentLED	;reset to 1 so that when value is passed to Port C it works
ChangeEnd
	return
        end
