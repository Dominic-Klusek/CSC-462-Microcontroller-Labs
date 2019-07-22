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
	numbers : 8
	sum
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
	rcall SumArray
	rcall DivideByEight
	rcall Delay
	rcall Display
	MOVLF 0x00, sum
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
	MOVLF  0x00, PORTB
	MOVLF  0x00, PORTC
	lfsr 0, numbers		   ;initialize numbers
	MOVLF D'12', POSTINC0
	MOVLF D'36', POSTINC0
	MOVLF D'104', POSTINC0
	MOVLF D'10', POSTINC0
	MOVLF D'39', POSTINC0
	MOVLF D'1', POSTINC0
	MOVLF D'9', POSTINC0
	MOVLF D'42', POSTINC0
	MOVLF D'0', POSTINC0
	MOVLF 0x00, sum		    ;initialize sum to 0
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
        MOVLF  1000,ALIVECNT     ;Reinitialize BLNKCNT
	btg  PORTC,RC2          ;Toggle pin, to support measuring loop time
DelayLoop
	rcall LoopTime
	decf  ALIVECNT,F        ;Decrement loop counter and return if not zero
	bnz DelayLoop
	return

;;;;;;; SumArray subroutine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
SumArray
	lfsr 0, numbers		;load address of numbers to LSFR0
AdditionLoop
	movf POSTINC0, W	;move current array value to WREG
	bz AdditionEnd		;check to see if we reached end of array if WREG equals 0
	addwf sum, F		;add WREG to sum
	bra AdditionLoop	;jump to AdditionLoop to continue addition
AdditionEnd
	return

;;;;;;; DivideByEightsubroutine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
DivideByEight
	rrncf sum, F		;rotate right 3 times to simulate dividing by 8
	bcf sum, 7		;clear MSB each time
	rrncf sum, F
	bcf sum, 7
	rrncf sum, F
	bcf sum, 7
	return

;;;;;;; Display subroutine ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	
Display
	movf sum, W		;move suim to WREG
	movwf PORTB		;display lower 6 bits
	andlw B'11000000'	;leave only the 2 MSB alone
	rrncf WREG, W		;rotate bits so that they display through PORTC0 and PORTC1
	rrncf WREG, W
	rrncf WREG, W
	rrncf WREG, W
	rrncf WREG, W
	rrncf WREG, W
	movwf PORTC		;move WREG to PORTC
	return
        end
