            TTL Exercise 01
;***************************************************************
;This program loads four constants from ROM.
;and store them in variables in RAM
;Name:  <Your name here>
;Date:  <Date completed here>
;Class:  ELT-3240
;Section:  <Your lab section, day, and time here>
;---------------------------------------------------------------
;Template:  Hung K. Nguyen
;           September 19, 2019
;***************************************************************
;Assembler directives
            THUMB
; Memory
SSTACK_SIZE EQU    0x00000100  ;Used by MKL46Z4.s
; NVIC
ICER		EQU 	0xE000E180
;**********************************************************************
;Vector Table Mapped to Address 0 at Reset
;Linker requires __Vectors to be exported
            AREA    RESET, DATA, READONLY
            EXPORT  __Vectors
__Vectors 
            DCD    SP_INIT       ;00: stack pointer value when stack is empty
            DCD    Reset_Handler      ;01:reset vector
            ALIGN
;**********************************************************************
;Program
;Linker requires Reset_Handler
            AREA    MyCode,CODE,READONLY
            ENTRY
			EXPORT  Reset_Handler
Reset_Handler
Main
;---------------------------------------------------------------
;begin program code
;Disable interrupts
            LDR   R0, =ICER	; R0 = &ICER
			MOVS	R1,#0	;r1 = 0
			MVNS	R1,R1	;R1= 0xFFFFFFFF
			STR		R1, [R0,#0] ; ICER = 0xFFFFFFFF
;Initialize stack pointer
            LDR      R0, =SP_INIT
			MOV		SP,R0
;---------------------------------------------------------------
;>>>>> begin your main program code <<<<<
MainLoop						; do {
		NOP						; No operation
		LDR		R2, =ConstData	; CPtr = &ConstData
		MOVS	R3, #0x02		; Counter = 2
LOOP							; Repeat {
		LDR		R1, [R2, #0]	; R1 = CPtr[0]
		LDR		R0, =VarData	; VPtr = &VarData[0]
		STR		R1, [R0, #0]	; VPtr[0] = CPtr[0]
		LDR		R1, [R2,#4]		; R1 = CPtr[1]
		STR		R1, [R0, #4]	; VPtr[1] = CPtr[1]		
		ADDS	R2, #8			; CPTR = &(CPTR[2])
		SUBS	R3, #1			; Counter--
		BNE 	LOOP			; } While (Counter != 0)
		NOP						; No operation
;>>>>>   end your main program code <<<<<
;Stay here
        B       MainLoop		; } Forever
;---------------------------------------------------------------
;>>>>> begin subroutine code <<<<<
;>>>>>   end subroutine code <<<<<	
;end program code
        ALIGN
;***************************************************************
        AREA    MyConst, DATA, READONLY
; Place Constant data here
ConstData	DCD 0x0000000A, 0x0000000B, 0x00000010, 10
			ALIGN
;***************************************************************
        AREA    RAM, DATA, READWRITE
; Place Constant data here
SSTACK 	SPACE SSTACK_SIZE
SP_INIT
;Place variable data here
VarData		SPACE  8
			END		