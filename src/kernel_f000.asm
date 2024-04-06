; *****************************************************************************
; *    kernel_f000.asm                                                        *
; *                                                                           *
; *          a.k.a. The Basic Input and Output System (BIOS)                  *
; *****************************************************************************
		INCLUDE "memory_map.asm"

; *****************************************************************************
; * SOFTWARE VECTORS                                                          *
; *****************************************************************************
		org	$0000	

VECT_EXEC	fdb	EXEC_start	; User defined EXEC vector
VECT_SWI3 	fdb	SWI3_start	; SWI3 Software Interrupt Vector
VECT_SWI2 	fdb	SWI2_start	; SWI2 Software Interrupt Vector
VECT_FIRQ 	fdb	FIRQ_start	; FIRQ Software Interrupt Vector
VECT_IRQ  	fdb	IRQ_start	; IRQ Software Interrupt Vector
VECT_SWI  	fdb	SWI_start	; SWI / SYS Software Interrupt Vector
VECT_NMI  	fdb	NMI_start	; NMI Software Interrupt Vector	
VECT_RESET	fdb	KRNL_START	; RESET Software Interrupt Vector	

; *****************************************************************************
; * KERNAL ROUTINE SOFTWARE VECTORS                                           *
; *****************************************************************************
; ... add these

; *****************************************************************************
; * RESERVED ZERO PAGE KERNAL VARIABLES                                       *
; *****************************************************************************
KRNL_CSR_COL	fcb	0		; (Byte) current cursor horizontal position
KRNL_CSR_ROW	fcb	0		; (Byte) current cursor vertical position
KRNL_ATTRIB	fcb	0		; (Byte) current character display attribute
KRNL_LE_ANCHOR	fdb	0		; (Word) line edit anchor screen address

; *****************************************************************************
; * KERNEL ROM                                                                *
; *****************************************************************************
		org 	KERNEL_ROM
; Notes: 
;	fcc 	stores raw character string with no default termination
;	fcs	character string with its terminators high bit set
;	fcn	character string with null termination

KRNL_PROMPT0	fcn	"Retro 6809 Kernel ROM V0.1\n"
KRNL_PROMPT1	fcn	"Emulator compiled on \n"
KRNL_PROMPT2	fcn	"Under GPL V3 Liscense\n"
KRNL_PROMPT3	fcn	"Copyright 2024 By Jay Faries\n\n"
READY_PROMPT	fcn	"Ready\n"

; *****************************************************************************
; * KERNEL JUMP VECTORS                                                       *
; *****************************************************************************
KRNL_EXEC	jmp	[VECT_EXEC]	; The user EXEC vector
KRNL_SWI3 	jmp	[VECT_SWI3 ]	; SWI3 Software Interrupt Vector	
KRNL_SWI2 	jmp	[VECT_SWI2 ]	; SWI2 Software Interrupt Vector
KRNL_FIRQ 	jmp	[VECT_FIRQ ]	; FIRQ Software Interrupt Vector
KRNL_IRQ  	jmp	[VECT_IRQ  ]	; IRQ Software Interrupt Vector
KRNL_SWI  	jmp	[VECT_SWI  ]	; SWI / SYS Software Interrupt Vector
KRNL_NMI  	jmp	[VECT_NMI  ]	; NMI Software Interrupt Vector
KRNL_RESET	jmp	[VECT_RESET]	; RESET Software Interrupt Vector	

; *****************************************************************************
; * DEFAULT VECTOR SUBROUTINES                                                *
; *****************************************************************************
EXEC_start	bra	EXEC_start	; EXEC program
SWI3_start	bra	SWI3_start	; SWI3 Implementation
SWI2_start	bra	SWI2_start	; SWI2 Implementation
FIRQ_start	bra	FIRQ_start	; FIRQ Implementation
IRQ_start	bra	IRQ_start	; IRQ Implementation
SWI_start	bra	SWI_start	; SWI / SYS Implementation
NMI_start	bra	NMI_start	; NMI Implementation
RESET_start	bra	RESET_start	; RESET Implementation

; *****************************************************************************
; * KERNEL INITIALIZATION                                                     *
; *****************************************************************************
KRNL_START	; initialize the system stack (and the zero page)
		ldx	#ZERO_PAGE	; point to the start of zero-page
k_start_0	clr	,x+		; clear the next byte
		cmpx	#SSTACK_TOP	; at the end of the stack space?
		bne	k_start_0	; loop if not there yet	
		lds	#SSTACK_TOP	; set the S to the top of the stack
		; CPU clock speed
		lda	#$0C		; set the default CPU clock speed
		sta	SYS_STATE	;	to 2.0 mhz.
		; default graphics mode
		lda	#$0E		; default: 0x03 = 40x25 text
					;          0x0E = 32x15 text (16:9)
		sta	GFX_MODE	; set the default graphics
		; default text color attribute
		lda	#$B4		; lt green on dk green
		sta	KRNL_ATTRIB
		; clear the default text screen buffer
		ldd	#$20B4		; lt-green on dk-green SPACE character
		jsr	KRNL_CLS	; clear the screen

	; TESTING

		ldx	#KRNL_PROMPT0
		jsr	KRNL_LINEOUT
		ldx	#KRNL_PROMPT1
		jsr	KRNL_LINEOUT
		ldx	#KRNL_PROMPT2
		jsr	KRNL_LINEOUT
		ldx	#KRNL_PROMPT3
		jsr	KRNL_LINEOUT
		ldx	#READY_PROMPT
		jsr	KRNL_LINEOUT

	; infinate loop
inf_loop	bra 	inf_loop


; *****************************************************************************
; * KERNEL SUBROUTINES (Prototypes)                                           *
; *****************************************************************************
; KRNL_CLS		; Clears the current screen buffer
; KRNL_CHROUT		; Output a character to the console
; KRNL_NEWLINE		; Perfoms a CR/LF on the console
; KRNL_LINEOUT		; Outputs a string to the console
; KRNL_CHARPOS		; Loads into X the cursor position
; KRNLL_SCROLL		; Scroll the text screen up one line

; KRNLL_CMPSTR		; Compare two strings of arbitrary lengths
; KRNLL_CMPSTREQ	; Compare two strings of equal length
; KRNLL_TBLSEARCH	; General Table Search
; KRNLL_LINEEDIT	; Line edit characters in the EDT_BUFFER
; KRNLL_INCHR		; Input a character from the console
; KRNLL_SCROLL		; Scroll the text screen up one line
; KRNLL_INHEX		; Input a hex digit from the console

; *****************************************************************************
; * KRNL_CLS                                                                  *
; * 	Clears the currently displayed screen buffer                          *
; *                                                                           *
; * ENTRY REQUIREMENTS: A = Character Color Attribute                         *
; *                         MSN = Foreground Color                            *
; *                         LSN = Background Color                            *
; *                     (or in bitmap graphics modes)                         *
; *                     A = byte to fill the buffer with                      *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved                               *
; *****************************************************************************
KRNL_CLS	pshs	X		; save the used registers onto the stack
		ldx	#VIDEO_START	; point X to the start of the video buffer
K_CLS_0		std	,x++		; store the attrib/character or pixel data
		cmpx	GFX_VID_END	; check for the end of the current buffer
		blt	K_CLS_0		; loop of not yet reached the end
		puls	x, pc		; restore the registers and return

; *****************************************************************************
; * KRNL_CHAROUT                                                              *
; * 	Outputs a character to the console at the current                     *
; *     cursor position. This routine should the cursors                      *
; *     position and handles text scrolling as needed.                        *
; *                                                                           *
; * ENTRY REQUIREMENTS: A = Character to be displayed                         *
; *                     B = (if non-zero) Color Attribute                     *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved                               *
; *****************************************************************************
KRNL_CHAROUT	
		pshs	d, x		; save the used registers onto the stack
		tstb			; is B a null?
		bne	K_CHAROUT_1	; nope, continue
		ldb	KRNL_ATTRIB	; load the current color attribute
K_CHAROUT_1	tsta			; is A a null?
		beq	K_CHAROUT_DONE	;    A is null, just return and do nothing		
		cmpa	#$0A		; is it a newline character?
		bne	K_CHAROUT_0	; nope, don't do a newline
		jsr	KRNL_NEWLINE	; advance the cursor 
		bra	K_CHAROUT_DONE	; clean up and return
K_CHAROUT_0	jsr	KRNL_CHARPOS	; position X at the cursor position
		std	,x		; display the character/attribute combo
		inc	KRNL_CSR_COL	; increment current cursor column position
		lda	KRNL_CSR_COL	; load current cursor column position					
		cmpa	GFX_HRES+1	; compare with the current screen columns
		blt	K_CHAROUT_DONE	; cleanup and return if the csr column is okay
		jsr	KRNL_NEWLINE	; perform a new line
K_CHAROUT_DONE	puls	d, x, pc	; cleanup and return

; *****************************************************************************
; * KRNL_NEWLINE                                                              *
; * 	Perfoms a CR/LF on the console. Advances the current                  *
; *     cursor position and scrolls the console if needed.                    *
; *                                                                           *
; * ENTRY REQUIREMENTS: NONE                                                  *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved.                              *
; *****************************************************************************
KRNL_NEWLINE	pshs	D, X		; save the used registers onto the stack
		clr	KRNL_CSR_COL	; carrage return (move to left edge)
		inc	KRNL_CSR_ROW	; increment the cursors row
		lda	KRNL_CSR_ROW	; load the current row
		cmpa	GFX_VRES+1	; compared to the current screen rows
		blt	K_NEWLINE_DONE	; clean up and return if less than
		dec	KRNL_CSR_ROW	; move the cursor the the bottom row
		jsr	KRNL_SCROLL	; scroll the text screen up one line
K_NEWLINE_DONE	puls	D, X, pc	; restore the saved registers and return


; *****************************************************************************
; * KRNL_LINEOUT                                                              *
; * 	Outputs a string to the console                                       *
; *                                                                           *
; * ENTRY REQUIREMENTS: X = String starting address                           *
; *                         (null or neg terminated)                          *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved.                              *
; *****************************************************************************
KRNL_LINEOUT	
		pshs	D, U, X		; save the used registers onto the stack
		tfr	x,u		; move X to U
		jsr	KRNL_CHARPOS	; set X to the cursor position 
K_LINEOUT_0	lda	,u+		; fetch the next character
		beq	K_LINEOUT_DONE	; cleanup and return if null-terminator
		bmi	K_LINEOUT_DONE	; cleanup and return if neg-terminator
		jsr	KRNL_CHAROUT	; send the character to the console
		leax	1, x		; point to the next character
		bra	K_LINEOUT_0	; continue looping until done
K_LINEOUT_DONE	puls	D, U, X, pc	; restore the saved registers and return		

; *****************************************************************************
; * KRNL_CHARPOS                                                              *
; * 	Loads into X the cursor position                                      *
; *                                                                           *
; * ENTRY REQUIREMENTS: NONE                                                  *
; *                                                                           *
; * EXIT CONDITIONS:	X = The address within the text                       *
; *                         where the cursor is positioned.                   *   
; *                     All other registers preserved.                        *
; *****************************************************************************
KRNL_CHARPOS	pshs	d		; save the used registers onto the stack
		lda	KRNL_CSR_ROW	; current cursor row
		ldb	GFX_HRES+1	; current text columns
		lslb			; times two (account for the attribute)
		mul			; row * columns
		ldx	#VIDEO_START	; the buffer starting address
		leax	d, x		; add the video base address
		ldb	KRNL_CSR_COL	; load the current cursor column
		lslb			; times two (account for the attribute)
		leax	b, x		; add the column to the return address
		puls	d, pc		; restore the saved registers and return

; *****************************************************************************
; * KRNL_SCROLL                                                               *
; * 	Scroll the text screen up one line and blank the bottom line.         *
; *                                                                           *
; * ENTRY REQUIREMENTS: NONE                                                  *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved.                              *
; *****************************************************************************
KRNL_SCROLL	pshs	d, x, u		; save the used registers onto the stack
		ldx	#VIDEO_START	; set X to the start of the video buffer
		tfr	x, u		; copy X into U
		ldb	GFX_HRES+1	; B = Screen Columns
		lslb			; account for the attribute byte
		leau	b, u		; U is now one line below X
K_SCROLL_0	ldd	,u++		; load a character from where U points
		std	,x++		; store it to where X points
		cmpu	GFX_VID_END	; has U exceeded the screen buffer
		blt	K_SCROLL_0	; continue looping of not
		lda	#' '		; set SPACE as the current character
K_SCROLL_1	sta	,x++		; and store it to where X points
		cmpx	GFX_VID_END	; continue looping until the bottom ...
		blt	K_SCROLL_1	; ... line has been cleared
		puls	d, x, u, pc	; restore the registers and return

; ************************************************************
; * ROM BASED HARDWARE VECTORS                               *
; ************************************************************
		org	$FFF0

		fdb	KRNL_EXEC	; HARD_RSRVD       EXEC Interrupt Vector
		fdb	KRNL_SWI3  	; HARD_SWI3        SWI3 Hardware Interrupt Vector
		fdb	KRNL_SWI2  	; HARD_SWI2        SWI2 Hardware Interrupt Vector
		fdb	KRNL_FIRQ  	; HARD_FIRQ        FIRQ Hardware Interrupt Vector
		fdb	KRNL_IRQ    	; HARD_IRQ         IRQ Hardware Interrupt Vector
		fdb	KRNL_SWI    	; HARD_SWI         SWI / SYS Hardware Interrupt Vector
		fdb	KRNL_NMI    	; HARD_NMI         NMI Hardware Interrupt Vector
		fdb	KRNL_RESET 	; HARD_RESET       RESET Hardware Interrupt Vector
