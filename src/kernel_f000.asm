; ***********************************************************************
; * 	kernel_f000.asm 						*
; *									*
; *	a.k.a. The Basic Input and Output System (BIOS)			*
; ***********************************************************************
; *									*
; *  	LINUX: 		cd asm						*
; *			sh build.sh kernel_f000				*
; *		or							*	
; *	WINDOWS:	asm kernel_f000					*
; *									*
; *	build.sh:							*
; *		#!/bin/bash						*
; *		#lwasm -9 test.asm --format=ihex -otest.hex -ltest.lst	*
; *		echo "lwasm -9 $1.asm --format=ihex -o$1.hex -l$1.lst"	*
; *		lwasm -9 $1.asm --format=ihex -o$1.hex -l$1.lst		*
; *									*
; *	asm.bat:							*
; *     	@set arg1=%1						*
; *     	asm6809 -H -9 %arg1%.asm -o %arg1%.hex -l %arg1%.lst	*
; ***********************************************************************
		INCLUDE "memory_map.asm"

; Software Vectors
	org	$0000	
VECT_EXEC	fdb	$0000		; This will likely be used as the EXEC vector
VECT_SWI3 	fdb	SWI3_start	; SWI3 Software Interrupt Vector
VECT_SWI2 	fdb	SWI2_start	; SWI2 Software Interrupt Vector
VECT_FIRQ 	fdb	FIRQ_start	; FIRQ Software Interrupt Vector
VECT_IRQ  	fdb	IRQ_start	; IRQ Software Interrupt Vector
VECT_SWI  	fdb	SWI_start	; SWI / SYS Software Interrupt Vector
VECT_NMI  	fdb	NMI_start	; NMI Software Interrupt Vector	
VECT_RESET	fdb	kernel_start	; RESET Software Interrupt Vecto	





; Kernel Jump Vector Calls	
	org 	KERNEL_ROM		
KRNL_EXEC	jmp	[VECT_EXEC]	; This will likely be used as t	he EXEC vector
KRNL_SWI3 	jmp	[VECT_SWI3 ]	; SWI3 Software Interrupt Vector	
KRNL_SWI2 	jmp	[VECT_SWI2 ]	; SWI2 Software Interrupt Vector
KRNL_FIRQ 	jmp	[VECT_FIRQ ]	; FIRQ Software Interrupt Vector
KRNL_IRQ  	jmp	[VECT_IRQ  ]	; IRQ Software Interrupt Vector
KRNL_SWI  	jmp	[VECT_SWI  ]	; SWI / SYS Software Interrupt Vector
KRNL_NMI  	jmp	[VECT_NMI  ]	; NMI Software Interrupt Vector
KRNL_RESET	jmp	[VECT_RESET]	; RESET Software Interrupt Vector	

; a null vectors (i.e. an infinate loop traps for vector testing)
KRNL_UNDEF	bra	KRNL_UNDEF

; default subs
;EXEC_start	bra	EXEC_start	; EXEC program
SWI3_start	bra	SWI3_start	; SWI3 Implementation
SWI2_start	bra	SWI2_start	; SWI2 Implementation
FIRQ_start	bra	FIRQ_start	; FIRQ Implementation
IRQ_start	bra	IRQ_start	; IRQ Implementation
SWI_start	bra	SWI_start	; SWI / SYS Implementation
NMI_start	bra	NMI_start	; NMI Implementation
RESET_start	bra	RESET_start	; RESET Implementation



kernel_start
	; initialize both stack pointers
		lds	#SSTACK_TOP
		ldu	#0
		stu	VECT_EXEC
		ldu	#SSTACK_TOP

	; increase the cpu clock rate to unmetered for now
		lda	#$0F
		sta	SYS_STATE


	; testing: ... fill the screen buffer
		ldd	#$000f
		ldx	#VIDEO_START
lp0		std	,x++
		inca
		incb
		cmpx	#VIDEO_END
		blt	lp0

	; testing: ... increment the screen buffer
lp1		
		ldx	#VIDEO_START
lp2		inc	,x+
		cmpx	GFX_VID_END
		bls	lp2
		bra	lp1



inf_loop	bra 	inf_loop




; ROM Based Hardware Vectors
		org	$FFF0
		fdb	KRNL_EXEC	; HARD_RSRVD       EXEC Interrupt Vector
		fdb	KRNL_SWI3  	; HARD_SWI3        SWI3 Hardware Interrupt Vector
		fdb	KRNL_SWI2  	; HARD_SWI2        SWI2 Hardware Interrupt Vector
		fdb	KRNL_FIRQ  	; HARD_FIRQ        FIRQ Hardware Interrupt Vector
		fdb	KRNL_IRQ    	; HARD_IRQ         IRQ Hardware Interrupt Vector
		fdb	KRNL_SWI    	; HARD_SWI         SWI / SYS Hardware Interrupt Vector
		fdb	KRNL_NMI    	; HARD_NMI         NMI Hardware Interrupt Vector
		fdb	KRNL_RESET 	; HARD_RESET       RESET Hardware Interrupt Vector
