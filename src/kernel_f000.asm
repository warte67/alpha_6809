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
VECT_CLS	fdb	STUB_CLS	; CLS function Software Vector
VECT_CHROUT	fdb	STUB_CHROUT	; Character Out Software Vector
VECT_NEWLINE	fdb	STUB_NEWLINE	; Kernel Newline Software Vector
VECT_LINEOUT	fdb	STUB_LINEOUT	; String to Console Software Vector
VECT_CSRPOS	fdb	STUB_CSRPOS	; Cursor Position Software Vector
VECT_SCROLL	fdb	STUB_SCROLL	; Scroll Text Screen Software Vector
VECT_LINEEDIT	fdb	STUB_LINEEDIT	; Console Line Editor Software Vector
VECT_GETKEY	fdb	STUB_GETKEY	; Wait for Key Press Software Vector
VECT_GETHEX	fdb	STUB_GETHEX	; Wait for Hex Character Software Vector
VECT_GETNUM	fdb	STUB_GETNUM	; Wait for Numeric Character Vector

; *****************************************************************************
; * RESERVED ZERO PAGE KERNAL VARIABLES                                       *
; *****************************************************************************
KRNL_CURSOR_COL	fcb	0		; (Byte) current cursor horizontal position
KRNL_CURSOR_ROW	fcb	0		; (Byte) current cursor vertical position
KRNL_ATTRIB	fcb	0		; (Byte) current character display attribute
KRNL_ANCHOR_COL	fcb	0		; (Byte) line edit anchor column
KRNL_ANCHOR_ROW	fcb	0		; (Byte) line edit anchor row
KRNL_LOCAL_0	fcb	0		; (Byte) used locally for some KRNL calls
KRNL_LOCAL_1	fcb	0		; (Byte) used locally for some KRNL calls
KRNL_LOCAL_2	fcb	0		; (Byte) used locally for some KRNL calls
KRNL_LOCAL_3	fcb	0		; (Byte) used locally for some KRNL calls

; *****************************************************************************
; * KERNEL ROM                                                                *
; *****************************************************************************
		org 	KERNEL_ROM
; Notes: 
;	fcc 	stores raw character string with no default termination
;	fcs	character string with its terminators high bit set
;	fcn	character string with null termination

KRNL_PROMPT0	fcn	"Retro 6809 Kernel ROM V0.3\n"
KRNL_PROMPT1	fcn	"Emulator compiled "
KRNL_PROMPT2	fcn	"Under GPL V3 Liscense\n"
KRNL_PROMPT3	fcn	"Copyright 2024 By Jay Faries\n\n"  
READY_PROMPT	fcn	"Ready\n"

KRNL_CMD_TABLE	fcn	"cls"		; #0
		fcn	"color"		; #1
		fcn	"load"		; #2
		fcn	"exec"		; #3
		fcn	"reset"		; #4
		fcn	"dir"		; #5
		fcn	"cd"		; #6
		fcn	"chdir"		; #7
		fcn	"exit"		; #8
		fcn	"quit"		; #9
		fcn	"mode"		; #10
		fcb	$FF		; $FF = end of list
		; ...

KRNL_CMD_VECTS  fdb	do_cls		; #0
		fdb	do_color	; #1
		fdb	do_load		; #2
		fdb	do_exec		; #3
		fdb	do_reset	; #4
		fdb	do_dir		; #5
		fdb	do_cd		; #6
		fdb	do_chdir	; #7
		fdb	do_exit		; #8
		fdb	do_quit		; #9
		fdb	do_mode		; #10
		; ...
KRNL_ERR_NFND 	fcn	"ERROR: Command Not Found\n"

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
EXEC_start	rts			; EXEC program
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
KRNL_START	; initialize the system
		ldx	#SYSTEM_STACK	; point to the start of stack space
k_init_0	clr	,x+		; clear the next byte
		cmpx	#SSTACK_TOP	; at the end of the stack space?
		bne	k_init_0	; loop if not there yet	
		lds	#SSTACK_TOP	; set the S to the top of the stack
		; CPU clock speed
		lda	#$0C		; set the default CPU clock speed
		sta	SYS_STATE	;	to 2.0 mhz.
		; default graphics mode
		lda	#$03		; default: 0x03 = 40x25 text
					;          0x0E = 32x15 text (16:9)
		sta	GFX_MODE	; set the default graphics
		; default text color attribute
		lda	#$B4		; lt green on dk green
		sta	KRNL_ATTRIB
		; clear the default text screen buffer
		ldd	#$20B4		; lt-green on dk-green SPACE character
		jsr	KRNL_CLS	; clear the screen

		; Initialize the line editor
		clr	EDT_BFR_CSR	; set the buffer cursor to the start
		ldx	#EDT_BUFFER	; point to the edit buffer
k_init_1	clr	,x+		; clear an entry and advance to next
		cmpx	#KEY_END	; are we at the end of the buffer?
		blt	k_init_1	;   not yet, continue looping
		; output the startup prompts
		ldx	#KRNL_PROMPT0	; point to the first prompt line
		jsr	KRNL_LINEOUT	; output it to the console
		ldx	#KRNL_PROMPT1	; point to the second prompt line
		jsr	KRNL_LINEOUT	; output it to the console
		; fetch compilation date
		lda	#FC_COMPDATE	; command to fetch the compilation date
		sta	FIO_COMMAND	; issue the command to the FileIO device
k_init_5	lda	FIO_PATH_DATA	; load a character from the response data
		beq	k_init_4	; if we've received a NULL, stop looping
		jsr	KRNL_CHROUT	; output the retrieved character to concole
		bra	k_init_5	; continue looping while theres still data
k_init_4	lda	#$0a		; line feed character
		jsr	KRNL_CHROUT	; send the line feed to the console
		ldx	#KRNL_PROMPT2	; point to the third prompt line
		jsr	KRNL_LINEOUT	; output it to the console
		ldx	#KRNL_PROMPT3	; point to the fourth prompt line
		jsr	KRNL_LINEOUT	; output it to the console

; *****************************************************************************
; * THE MAIN COMMAND LOOP                                                     *
; *****************************************************************************
; *                                                                           *
; * 	1) Displays the "Ready" prompt                                        *
; *     2) Runs the Command Input Line Editor                                 *
; *     3) Dispatches the Operating System Commands                           *
; *                                                                           *
; *****************************************************************************
MAIN_LOOP	ldb	KRNL_ATTRIB	; fetch the current color attribute
		ldx	#READY_PROMPT	; the ready prompt
		jsr	KRNL_LINEOUT	; output to the console
		lda	#$ff		; Initialize the line editor
		sta	EDT_BFR_LEN	; allow for the full sized buffer
		clr	EDT_BFR_CSR	; set the buffer cursor to the start
		clr	EDT_BUFFER		
		ldx	#EDT_BUFFER	; point to the edit buffer
k_main_clr	clr	,x+		; clear an entry and advance to next
		cmpx	#KEY_END	; are we at the end of the buffer?
		blt	k_main_clr	;   not yet, continue looping
k_main_0	jsr	KRNL_LINEEDIT	; run the command line editor
		jsr	KRNL_CMD_PROC	; decode the command; A = Table Index
		tst	FIO_BUFFER	; test the buffer for a null
		beq	k_main_cont	; skip, nothing was entered
		cmpa	#$FF		; ERROR: command not found 
		beq	k_main_error	;    display the error
		lsla			; index two byte addresses
		leax	1,x
		ldy	#KRNL_CMD_VECTS	; the start of the command vector table
		jsr	[a,y]		; call the command subroutine
k_main_cont	tst	EDT_BUFFER	; nothing entered in the command line?
		beq	k_main_0	;   nope, skip the ready prompt
		bra	MAIN_LOOP	; back to the top of the main loop
k_main_error	ldx	#KRNL_ERR_NFND	; ERROR: Command Not Found
		jsr	KRNL_LINEOUT	; send it to the console
		bra	k_main_cont	; continue within the main loop

; *****************************************************************************
; * MAIN KERNEL COMMAND SUBROUTINES (Prototypes)                              *
; *****************************************************************************
;	do_cls		; #0
;	do_color	; #1
;	do_load		; #2
;	do_exec		; #3
;	do_reset	; #4
;	do_dir		; #5
;	do_cd		; #6
;	do_chdir	; #7
;	do_exit		; #8
;	do_quit		; #9
;	do_mode		; #10

str_cls		fcn	"CLS\n"
str_color	fcn	"COLOR\n"
str_load	fcn	"LOAD\n"
str_exec	fcn	"EXEC\n"
str_reset	fcn	"RESET\n"
str_dir		fcn	"DIR\n"
str_cd		fcn	"CD\n"
str_chdir	fcn	"CHDIR\n"
str_exit	fcn	"EXIT\n"
str_quit	fcn	"QUIT\n"
str_mode	fcn	"MODE\n"


do_cls		; CLEAR SCREEN			ARG1 = Color Attribute

		tst	,x
		beq	do_cls_0
		jsr 	KRNL_ARG_TO_A
		tsta
		beq	do_cls_0
		sta	KRNL_ATTRIB
do_cls_0	lda	#' '
		ldb	KRNL_ATTRIB
		jsr	KRNL_CLS		
		rts

do_color	; COLOR				ARG1 = Color Attribute
		tst	,x
		beq	do_color_0
		jsr 	KRNL_ARG_TO_A
		tsta
		beq	do_color_0
		cmpa	#$ff
		beq	do_color_0
		sta	KRNL_ATTRIB
do_color_0	rts		

do_load		; LOAD 				ARG1 = {filepath}/filename
		ldx	#str_load
		bra	str_output

do_exec		; EXEC				ARG1 = none
		ldx	#str_exec
		bra	str_output

do_reset	; RESET				ARG1 = none
		lda	#FC_RESET
		sta	FIO_COMMAND
		rts

do_dir		; DIR				ARG1 = {filepath}
		ldx	#str_dir
		bra	str_output

do_cd		; CHDIR				ARG1 = {filepath}
		ldx	#str_cd
		bra	str_output

do_chdir	; CHDIR				ARG1 = {filepath}
		ldx	#str_chdir
		bra	str_output

do_exit		; EXIT and QUIT			ARG1 = none
do_quit		; EXIT and QUIT			ARG1 = none
		lda	#FC_SHUTDOWN
		sta	FIO_COMMAND
		rts

do_mode		; MODE				ARG1 = Graphics Mode
		tst	,x
		beq	do_mode_0
		jsr 	KRNL_ARG_TO_A
		sta	GFX_MODE
		lda	#' '
		jsr	KRNL_CLS
do_mode_0	rts

		* ldx	#str_mode
		* bra	str_output		

str_output	jsr	KRNL_LINEOUT
		rts




; *****************************************************************************
; * KERNEL SUBROUTINES (Prototypes)                                           *
; *****************************************************************************
; 	KRNL_CLS	; Clears the current screen buffer                    
; 	KRNL_CHROUT	; Output a character to the console                   
; 	KRNL_NEWLINE	; Perfoms a CR/LF on the console                      
; 	KRNL_LINEOUT	; Outputs a string to the console                     
; 	KRNL_CSRPOS	; Loads into X the cursor position                    
; 	KRNL_SCROLL	; Scroll the text screen up one line
; 	KRNL_LINEEDIT	; Engage the text line editor
; 	KRNL_GETKEY	; Input a character from the console
; 	KRNL_GETHEX	; Input a hex digit from the console
; 	KRNL_GETNUM	; Input a numeric digit from the console
; 	KRNL_CMPSTR	; Compare two strings of arbitrary lengths
; 	KRNL_TBLSEARCH	; Table Search (find the string and return its index)
;
;	KRNL_CPY_DWORD	; Copy 32-bits from where X points to where Y points
; 	KRNL_D_TO_RAWA	; Write the D register to the ACA raw float register
; 	KRNL_D_TO_RAWB	; Write the D register to the ACB raw float register
; 	KRNL_D_TO_RAWR	; Write the D register to the ACR raw float register
; 	KRNL_D_TO_INTA	; Write the D register to the ACA integer register
; 	KRNL_D_TO_INTB	; Write the D register to the ACB integer register
; 	KRNL_D_TO_INTR	; Write the D register to the ACR integer register
;
; 	KRNL_RAWA_TO_D	; Read the ACA raw float register into the D register
; 	KRNL_RAWB_TO_D	; Read the ACB raw float register into the D register
; 	KRNL_RAWR_TO_D	; Read the ACR raw float register into the D register
; 	KRNL_INTA_TO_D	; Read the ACA integer register into the D register
; 	KRNL_INTB_TO_D	; Read the ACB integer register into the D register
; 	KRNL_INTR_TO_D	; Read the ACR integer register into the D register
;
;	KRNL_8BIT_MATH	; 8-bit integer math: D = A (U:mop) B
;	KRNL_DSP_ACA	; Displays the floating point number in the ACA register. 
;	KRNL_DSP_ACB	; Displays the floating point number in the ACB register. 
;	KRNL_DSP_ACR	; Displays the floating point number in the ACR register. 
;	KRNL_WRITE_ACA	; Convert a string (pointed to by X) and write to ACA
;	KRNL_WRITE_ACB	; Convert a string (pointed to by X) and write to ACB
;	KRNL_WRITE_ACR	; Convert a string (pointed to by X) and write to ACR
;
;	KRNL_ARG_TO_A	; convert a numeric string (at X) and return it in A

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
KRNL_CLS	jmp	[VECT_CLS]	; proceed through the software vector
STUB_CLS	pshs	X		; save the used registers onto the stack
		ldx	#VIDEO_START	; point X to the start of the video buffer
K_CLS_0		std	,x++		; store the attrib/character or pixel data
		cmpx	GFX_VID_END	; check for the end of the current buffer
		blt	K_CLS_0		; loop of not yet reached the end
		clr	KRNL_CURSOR_COL
		clr	KRNL_CURSOR_ROW
		puls	x,pc		; restore the registers and return

; *****************************************************************************
; * KRNL_CHROUT                                                              *
; * 	Outputs a character to the console at the current                     *
; *     cursor position. This routine should the cursors                      *
; *     position and handles text scrolling as needed.                        *
; *                                                                           *
; * ENTRY REQUIREMENTS: A = Character to be displayed                         *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved                               *
; *****************************************************************************
KRNL_CHROUT	jmp	[VECT_CHROUT]	; proceed through the software vector
STUB_CHROUT	pshs	d,x,cc		; save the used registers onto the stack
		ldb	KRNL_ATTRIB	; load the current color attribute
K_CHROUT_1	tsta			; is A a null?
		beq	K_CHROUT_DONE	;    A is null, just return and do nothing		
		cmpa	#$0A		; is it a newline character?
		bne	K_CHROUT_0	; nope, don't do a newline
		jsr	KRNL_NEWLINE	; advance the cursor 
		bra	K_CHROUT_DONE	; clean up and return
K_CHROUT_0	jsr	KRNL_CSRPOS	; position X at the cursor position
		std	,x		; display the character/attribute combo
		inc	KRNL_CURSOR_COL	; increment current cursor column position
		lda	KRNL_CURSOR_COL	; load current cursor column position					
		cmpa	GFX_HRES+1	; compare with the current screen columns
		blt	K_CHROUT_DONE	; cleanup and return if the csr column is okay
		jsr	KRNL_NEWLINE	; perform a new line
K_CHROUT_DONE	puls	d,x,cc,pc		; cleanup and return

; *****************************************************************************
; * KRNL_NEWLINE                                                              *
; * 	Perfoms a CR/LF on the console. Advances the current                  *
; *     cursor position and scrolls the console if needed.                    *
; *                                                                           *
; * ENTRY REQUIREMENTS: NONE                                                  *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved.                              *
; *****************************************************************************
KRNL_NEWLINE	jmp	[VECT_NEWLINE]	; proceed through the software vector
STUB_NEWLINE	pshs	D,X		; save the used registers onto the stack
		clr	KRNL_CURSOR_COL	; carrage return (move to left edge)
		inc	KRNL_CURSOR_ROW	; increment the cursors row
		lda	KRNL_CURSOR_ROW	; load the current row
		cmpa	GFX_VRES+1	; compared to the current screen rows
		blt	K_NEWLINE_DONE	; clean up and return if less than
		dec	KRNL_CURSOR_ROW	; move the cursor the the bottom row
		jsr	KRNL_SCROLL	; scroll the text screen up one line
K_NEWLINE_DONE	puls	D,X,pc		; restore the saved registers and return


; *****************************************************************************
; * KRNL_LINEOUT                                                              *
; * 	Outputs a string to the console                                       *
; *                                                                           *
; * ENTRY REQUIREMENTS: X = String starting address                           *
; *                         (null or neg terminated)                          *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved.                              *
; *****************************************************************************
KRNL_LINEOUT	jmp	[VECT_LINEOUT]	; proceed through the software vector
STUB_LINEOUT	pshs	D,U,X		; save the used registers onto the stack
		tfr	x,u		; move X to U
		jsr	KRNL_CSRPOS	; set X to the cursor position 
K_LINEOUT_0	lda	,u+		; fetch the next character
		beq	K_LINEOUT_DONE	; cleanup and return if null-terminator
		bmi	K_LINEOUT_DONE	; cleanup and return if neg-terminator
		jsr	KRNL_CHROUT	; send the character to the console
		leax	1, x		; point to the next character
		bra	K_LINEOUT_0	; continue looping until done
K_LINEOUT_DONE	puls	D,U,X,pc	; restore the saved registers and return		

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
KRNL_CSRPOS	jmp	[VECT_CSRPOS]	; proceed through the software vector
STUB_CSRPOS	pshs	d		; save the used registers onto the stack
		lda	KRNL_CURSOR_ROW	; current cursor row
		ldb	GFX_HRES+1	; current text columns
		lslb			; times two (account for the attribute)
		mul			; row * columns
		ldx	#VIDEO_START	; the buffer starting address
		leax	d, x		; add the video base address
		ldb	KRNL_CURSOR_COL	; load the current cursor column
		lslb			; times two (account for the attribute)
		leax	b, x		; add the column to the return address
		puls	d,pc		; restore the saved registers and return

; *****************************************************************************
; * KRNL_SCROLL                                                               *
; * 	Scroll the text screen up one line and blank the bottom line.         *
; *                                                                           *
; * ENTRY REQUIREMENTS: NONE                                                  *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved.                              *
; *****************************************************************************
KRNL_SCROLL	jmp	[VECT_SCROLL]	; proceed through the software vector
STUB_SCROLL	pshs	d,x,u		; save the used registers onto the stack
		ldx	#VIDEO_START	; set X to the start of the video buffer
		tfr	x, u		; copy X into U
		ldb	GFX_HRES+1	; B = Screen Columns
		lslb			; account for the attribute byte
		clra			; MSB of D needs to not be negative
		leau	d, u		; U is now one line below X
K_SCROLL_0	ldd	,u++		; load a character from where U points
		std	,x++		; store it to where X points
		cmpu	GFX_VID_END	; has U exceeded the screen buffer
		blt	K_SCROLL_0	; continue looping of not
		lda	#' '		; set SPACE as the current character
K_SCROLL_1	sta	,x++		; and store it to where X points
		cmpx	GFX_VID_END	; continue looping until the bottom ...
		blt	K_SCROLL_1	; ... line has been cleared
		tst	EDT_ENABLE	; are we using the line editor?
		beq	K_SCROLL_DONE	; nope, just clean up and return
		dec	KRNL_ANCHOR_ROW	; yup, decrease the anchor row by one
K_SCROLL_DONE	puls	d,x,u,pc	; restore the registers and return

; *****************************************************************************
; * KRNL_LINEEDIT                                                             *
; * 	Engage the text line editor,                                          *
; *                                                                           *
; * ENTRY REQUIREMENTS: NONE                                                  *
; *                                                                           *
; * EXIT CONDITIONS:	All registers preserved.                              *
; *****************************************************************************
KRNL_LINEEDIT	jmp	[VECT_LINEEDIT]	; proceed through the software vector
STUB_LINEEDIT	pshs	D,X,U,CC	; save the used registers onto the stack		
		ldd 	KRNL_CURSOR_COL	; load the current cursor position
		std	KRNL_ANCHOR_COL	;   use it to update the anchor position
		lda	#1		; load the enable condition
		sta	EDT_ENABLE	; to enable the line editor
KRNL_LEDIT_0	; display the line up to the cursor		
		ldd 	KRNL_ANCHOR_COL	; restore the line editor anchor
		std	KRNL_CURSOR_COL ; into the console cursor position
		ldu	#EDT_BUFFER	; point to the start of the edit buffer
		ldb	EDT_BFR_CSR	; the buffer csr position
		stb	KRNL_LOCAL_0	; store the edit csr position locally
KRNL_LEDIT_1	tst	KRNL_LOCAL_0	; test the edit csr position
		beq	KRNL_LEDIT_2	; if we're there, go display the cursor
		dec	KRNL_LOCAL_0	; decrement the edit csr position
		lda	,u+		; load the next character from the buffer
		beq	KRNL_LEDIT_2	; display csr if at the null terminator
		jsr	KRNL_CHROUT	; output the character to the console
		bra	KRNL_LEDIT_1	; loop until we're at the cursor
KRNL_LEDIT_2	; display the cursor at the end of the line
		lda	#' '		; load a blank SPACE character
		ldb	SYS_CLOCK_DIV	; load clock timer data
		lsrb			;	divide by 2
		lsrb			;	divide by 2
		lsrb			;	divide by 2
		lsrb			;	divide by 2
		andb	#$0F		; B now holds color cycled attribute
		tst	,u		; test the next character in the buffer
		beq	KRNL_LEDIT_3	; use the SPACE if we're at a null
		lda	,u+		; load the next character from buffer
KRNL_LEDIT_3	; finish the line
		jsr	KRNL_CSRPOS	; load X with the current cursor positino 
		std	,x		; store the character where X points to
		inc	KRNL_CURSOR_COL	; ipdate the cursor column number
		; ldb	KRNL_ATTRIB	; load the default color attribute
KRNL_LEDIT_4	lda	,u+		; fetch the next character from the buffer
		beq	KRNL_DONE	; if it's null, we're done
		jsr	KRNL_CHROUT	; output it to the console
		bra	KRNL_LEDIT_4	; continue looping until we find the null
KRNL_DONE	; space at the end	
		lda	#' '		; load the SPACE character
		jsr	KRNL_CSRPOS	; fetch the cursor position into X
		lda	#' '		; load the SPACE character
		ldb	KRNL_ATTRIB	; load the current color attribute
		std	,x		; update the console
		; test for the user pressing ENTER / RETURN
		lda	CHAR_POP	; Pop the top key from the queue
		beq	KRNL_LEDIT_0	; loop to the top if no keys we're pressed
		cmpa	#$0d		; check for the RETURN / ENTER key press
		bne	KRNL_LEDIT_0	; if not pressend, loop back to the top		
		clr	EDT_ENABLE	; disable the line editor		
		jsr	KRNL_CSRPOS	; load the cursor position into X
		lda	#' '		; load a SPACE character
		std	-2,x		; store the character, clean up artifacts
		ldd 	KRNL_ANCHOR_COL	; restore the line editor anchor
		std	KRNL_CURSOR_COL ; into the console cursor position
		ldx	#EDT_BUFFER	; point to the edit buffer
		jsr	KRNL_LINEOUT	; send the edit buffer to the console
		puls	D,X,U,CC,PC	; cleanup saved registers and return

; *****************************************************************************
; * KRNL_GETKEY                                                                *
; * 	Input a character from the console. Waits for the keypress.           *
; *                                                                           *
; * ENTRY REQUIREMENTS: NONE                                                  *
; *                                                                           *
; * EXIT CONDITIONS:	A = key code of the key that was pressed              *
; *                         All other registers preserved                     *
; *****************************************************************************
KRNL_GETKEY	jmp	[VECT_GETKEY]	; proceed through the software vector
STUB_GETKEY	pshs	b,CC		; save the used registers onto the stack
K_GETKEY_0	ldb	CHAR_POP	; pop the next key from the queue
		bne	K_GETKEY_0	; continue until the queue is empty		
K_GETKEY_1	ldb	CHAR_Q_LEN	; how many keys are in the queue
		beq	K_GETKEY_1	; loop until a key is queued
		lda	CHAR_POP	; pop the key into A to be returned
		puls	b,CC,PC	; cleanup saved registers and return

; *****************************************************************************
; * KRNL_GETHEX                                                               *
; * 	Input a hex digit from the console. Waits for the keypress.           *
; *                                                                           *
; * ENTRY REQUIREMENTS: NONE                                                  *
; *                                                                           *
; * EXIT CONDITIONS:	A = key code of the key that was pressed              *
; *                         All other registers preserved                     *
; *****************************************************************************
KRNL_GETHEX	jmp	[VECT_GETHEX]	; proceed through the software vector
STUB_GETHEX	pshs	CC		; save the used registers onto the stack
K_GETHEX_0	bsr	KRNL_GETKEY	; wait for and fetch a key press
		cmpa	#'0'		; compare with the '0' key
		blt	K_GETHEX_0	; keep scanning if less
		cmpa	#'9'		; compare with the '9' key
		bls	K_GETHEX_DONE	; found an appropriate key, return
		cmpa	#'A'		; compare with the 'A' key
		blt	K_GETHEX_0	; keep scanning if less
		cmpa	#'F'		; compare with the 'F' key
		bls	K_GETHEX_DONE	; found an appropriate key, return
		cmpa	#'a'		; compare with the 'a' key
		blt	K_GETHEX_0	; keep scanning if less
		cmpa	#'f'		; compare with the 'f' key
		bls	K_GETHEX_DONE	; found an appropriate key, return
		bra	K_GETHEX_0	; keep scanning
K_GETHEX_DONE	puls	CC,PC		; cleanup saved registers and return

; *****************************************************************************
; * KRNL_GETNUM                                                               *
; * 	Input a numeric digit from the console. Waits for the keypress.       *
; *                                                                           *
; * ENTRY REQUIREMENTS: NONE                                                  *
; *                                                                           *
; * EXIT CONDITIONS:	A = key code of the key that was pressed              *
; *                         All other registers preserved                     *
; *****************************************************************************
KRNL_GETNUM	jmp	[VECT_GETNUM]	; proceed through the software vector
STUB_GETNUM	pshs	CC		; save the used registers onto the stack
K_GETNUM_0	bsr	KRNL_GETKEY	; wait for and fetch a key press
		cmpa	#'0'		; compare with the '0' key
		blt	K_GETNUM_0	; keep scanning if less
		cmpa	#'9'		; compare with the '9' key
		bls	K_GETNUM_DONE	; found an appropriate key, return
		bra	K_GETNUM_0	; keep scanning
K_GETNUM_DONE	puls	CC,PC		; cleanup saved registers and return


; *****************************************************************************
; * KRNL_CMPSTR                                                               *
; * 	Compare two null-terminated strings of arbitrary lengths.             *
; *                                                                           *
; * ENTRY REQUIREMENTS: X = starting address of string 1                      *
; *                     Y = starting address of string 2                      *
; *                                                                           *
; * EXIT CONDITIONS:	CC = set per the comparison (less, greater, or same)  *
; *                     X = address last checked in string 1                  *
; *                     Y = address last checked in string 2                  *
; *****************************************************************************
KRNL_CMPSTR	pshs	D		; save the used registers onto the stack		
K_CMP_LOOP	tst	,x		; test the current character in string 1
		bne	K_CMP_1		; if its non-null, go test in string 2
		tst	,y		; test if character in both are null
		beq	K_CMP_EQUAL	; if so, strings are equal
		bra	K_CMP_LESS	; is LESS if str1 is null but str2 is not
K_CMP_1		tst	,y		; char in str1 is not null, but str2 is
		beq	K_CMP_GREATER	; return GREATER
		lda	,x+		; compare character from string 1
		cmpa	,y+		;    with character from string 2
		blt	K_CMP_LESS	; return LESS
		bgt	K_CMP_GREATER	; return GREATER
		bra	K_CMP_LOOP	; otherwise continue looping
K_CMP_LESS	lda	#1		; compare 1
		cmpa	#2		;    with 2
		bra	K_CMP_DONE	; return LESS
K_CMP_GREATER	lda	#2		; compare 2
		cmpa	#1		;    with 1
		bra	K_CMP_DONE	; return GREATER
K_CMP_EQUAL	clra			; set to zero
		cmpa	#0		; return EQUAL
K_CMP_DONE	puls	D,PC		; cleanup saved registers and return

; *****************************************************************************
; * KRNL_CMD_PROC                                                             *
; * 	Parse the command from the line edit buffer.                          *
; *                                                                           *
; * ENTRY REQUIREMENTS: Command text within EDT_BUFFER                        *
; *                                                                           *
; * EXIT CONDITIONS:	A = search string table index (or $FF if not found)   *
' *                     X & Y Modified                                        *
; *                     FIO_BUFFER will be modified                           *
; *****************************************************************************
KRNL_CMD_PROC	pshs	B,CC		; save the used registers onto the stack
	; copy EDT_BUFFER to FIO_BUFFER
		ldx	#EDT_BUFFER	; the start of the input buffer
		ldy	#FIO_BUFFER	; use the I/O buffer temporarily
K_CMDP_0	lda	,x+		; load a character from the input
		cmpa	#'A'		; make sure input is in lower case
		blt	K_CMDP_3	;   valid character if < 'A'
		cmpa	#'Z'		; all other characters are good to go
		bgt	K_CMDP_3	;   valid charcters above 'Z'
		ora	#$20		; convert all letters to lower case
K_CMDP_3	sta	,y+		; copy it to the output
		bne	K_CMDP_0	; branch until done copying
	; replace the null-terminator with $FF
		lda	#$ff		; the new character $FF
		sta	,y		; replace the null-terminator
	; replace SPACES with NULL (unless within '' or "")
		ldx	#FIO_BUFFER	; the start of the temp buffer
K_CMDP_1	lda	,x+		; load the next character from buffer
		cmpa	#$FF		; are we at the end of the buffer?
		beq	K_CMDP_2	;   yes, go parse the buffer
		cmpa	#"'"		; are we at a single-quote character?
		beq	K_CPROC_SKIP	;   skip through until we find another
		cmpa	#'"'		; are we at a double-quote character?
		beq	K_CPROC_SKIP	;   skip through until we find another
		cmpa	#' '		; are we at a SPACE character?
		bne	K_CMDP_1	; nope, continue scanning	
		clr	-1,x		; convert the SPACE to a NULL
		bra	K_CMDP_1	; continue scanning through the buffer
K_CPROC_SKIP	cmpa	,x+		; is character a quote character?
		beq	K_CMDP_1	;    yes, go back to scanning the buffer
		cmpa	#$ff		; are we at the end of the buffer?
		beq	K_CPROC_DONE	;    yes, cleanup and return
		bra	K_CPROC_SKIP	; continue looking for a quote character
	; FIO_BUFFER should now be prepared for parsing
K_CMDP_2	lda	#$0a		; line feed character
		jsr	KRNL_CHROUT	; send the line feed
		ldy	#KRNL_CMD_TABLE	; point to the command table to search
		ldx	#FIO_BUFFER	; point to the command to search for
	; X now points to the command to search for in the table
		bsr	KRNL_TBLSEARCH	; seach the table for the command
	; A = index of the found search string table index
K_CPROC_DONE	puls	B,CC,PC	; cleanup saved registers and return

; *****************************************************************************
; * KRNL_TBLSEARCH                                                            *
; * 	Table Search (find the string and return its index)                   *
; *                                                                           *
; * ENTRY REQUIREMENTS: X points to a string to be searched for               *
; *                     Y points to the start of a string table               *
; *                                                                           *
; * EXIT CONDITIONS:	A = string index if found, -1 ($FF) if not found      *
; *                     X = the end of the search string(next argument)       *
; *                         All other registers preserved                     *
; *****************************************************************************
KRNL_TBLSEARCH	pshs	B,Y,U,CC	; save the used registers onto the stack
		tfr	X,U		; save X in U
		clra			; set the return index to 0
K_TBLS_0	tfr	U,X		; restore X
		jsr 	KRNL_CMPSTR	; compare strings at X and at Y
		beq	K_TBLS_DONE	; found the string in the table		
		inca			; increment the index return value
K_TBLS_1	ldb	,y+		; look at the next character in table
		cmpb	#$ff		; is it the $ff terminator?
		beq	K_TBLS_NOTFOUND	; yes, the entry is not in the table
		tstb			; are we looking at a null character?
		bne	K_TBLS_1	; loop until the end of this entry
		bra	K_TBLS_0	; look at the next entry
K_TBLS_NOTFOUND	lda	#$ff		; not found error code
K_TBLS_DONE	puls	B,Y,U,CC,PC	; cleanup saved registers and return

; *****************************************************************************
; * KRNL_CPY_DWORD                                                            *
; * 	Copy 32-bits from where X points to where Y points                    *
; *                                                                           *
; * ENTRY REQUIREMENTS: X points to a DWORD to be copied from                 *
; *                     Y points to a DWORD to be copied to                   *
; *                                                                           *
; * EXIT CONDITIONS:	    All registers preserved                           *
; *****************************************************************************
KRNL_CPY_DWORD	pshs 	D,CC		; save the used registers onto the stack
		ldd	,x		; load the most-significant 16-bit word
		std	,y		; save the most-significant 16-bit word
		ldd	2,x		; load the least-significant 16-bit word
		std	2,y		; save the least-significant 16-bit word
		puls	D,CC,PC		; cleanup saved registers and return

; *****************************************************************************
; * KRNL_D_TO_RAW(A, B, or R)                                                  *
; * 	Write the D register to one of the raw float registers                *
; *                                                                           *
; * ENTRY REQUIREMENTS: D = 16-bit value to be written                        *
; *                                                                           *
; * EXIT CONDITIONS:	    All registers preserved                           *
; *****************************************************************************
KRNL_D_TO_RAWA	pshs	CC		; save the used registers onto the stack
		clr	MATH_ACA_RAW+0	; clear unneeded byte
		clr	MATH_ACA_RAW+1	; clear unneeded byte
		std	MATH_ACA_RAW+2	; store D in the ACA raw float register
		puls	CC,PC		; cleanup saved registers and return
		
KRNL_D_TO_RAWB	pshs	CC		; save the used registers onto the stack
		clr	MATH_ACB_RAW+0	; clear unneeded byte
		clr	MATH_ACB_RAW+1	; clear unneeded byte
		std	MATH_ACB_RAW+2	; store D in the ACB raw float register
		puls	CC,PC		; cleanup saved registers and return

KRNL_D_TO_RAWR	pshs	CC		; save the used registers onto the stack
		clr	MATH_ACR_RAW+0	; clear unneeded byte
		clr	MATH_ACR_RAW+1	; clear unneeded byte
		std	MATH_ACR_RAW+2	; store D in the ACR raw float register
		puls	CC,PC		; cleanup saved registers and return

; *****************************************************************************
; * KRNL_D_TO_INT(A, B, or R)                                                 *
; * 	Write the D register to one of the FP integer registers               *
; *                                                                           *
; * ENTRY REQUIREMENTS: D = 16-bit value to be written                        *
; *                                                                           *
; * EXIT CONDITIONS:	    All registers preserved                           *
; *****************************************************************************
KRNL_D_TO_INTA	pshs	CC		; save the used registers onto the stack
		clr	MATH_ACA_INT+0	; clear unneeded byte
		clr	MATH_ACA_INT+1	; clear unneeded byte
		std	MATH_ACA_INT+2	; store D in the ACA integer register
		puls	CC,PC		; cleanup saved registers and return
		
KRNL_D_TO_INTB	pshs	CC		; save the used registers onto the stack
		clr	MATH_ACB_INT+0	; clear unneeded byte
		clr	MATH_ACB_INT+1	; clear unneeded byte
		std	MATH_ACB_INT+2	; store D in the ACB integer register
		puls	CC,PC		; cleanup saved registers and return

KRNL_D_TO_INTR	pshs	CC		; save the used registers onto the stack
		clr	MATH_ACR_INT+0	; clear unneeded byte
		clr	MATH_ACR_INT+1	; clear unneeded byte
		std	MATH_ACR_INT+2	; store D in the ACR integer register
		puls	CC,PC		; cleanup saved registers and return

; *****************************************************************************
; * KRNL_RAW(A, B, or R)_TO_D                                                  *
; * 	Read one of the raw float registers into the D register          *
; *                                                                           *
; * ENTRY REQUIREMENTS: none                                                  *
; *                                                                           *
; * EXIT CONDITIONS: D = the integer value of the chosen FP register          *
; *                         All other registers preserved                     *
; *                                                                           *
; *****************************************************************************
KRNL_RAWA_TO_D	pshs	CC		; save the used registers onto the stack
		ldd	MATH_ACA_RAW+2	; load the ACA raw float value
		puls	CC,PC		; cleanup saved registers and return

KRNL_RAWB_TO_D	pshs	CC		; save the used registers onto the stack
		ldd	MATH_ACB_RAW+2	; load the ACB raw float value
		puls	CC,PC		; cleanup saved registers and return

KRNL_RAWR_TO_D	pshs	CC		; save the used registers onto the stack
		ldd	MATH_ACR_RAW+2	; load the ACR raw float value
		puls	CC,PC		; cleanup saved registers and return

; *****************************************************************************
; * KRNL_INT(A, B, or R)_TO_D                                                  *
; * 	Read one of the integer registers into the D register          *
; *                                                                           *
; * ENTRY REQUIREMENTS: none                                                  *
; *                                                                           *
; * EXIT CONDITIONS: D = the integer value of the chosen FP register          *
; *                         All other registers preserved                     *
; *                                                                           *
; *****************************************************************************
KRNL_INTA_TO_D	pshs	CC		; save the used registers onto the stack
		ldd	MATH_ACA_INT+2	; load the ACA integer value
		puls	CC,PC		; cleanup saved registers and return

KRNL_INTB_TO_D	pshs	CC		; save the used registers onto the stack
		ldd	MATH_ACB_INT+2	; load the ACB integer value
		puls	CC,PC		; cleanup saved registers and return

KRNL_INTR_TO_D	pshs	CC		; save the used registers onto the stack
		ldd	MATH_ACR_INT+2	; load the ACR integer value
		puls	CC,PC		; cleanup saved registers and return

; *****************************************************************************
; * KRNL_8BIT_MATH                                                            *
; * 	8-bit integer math                                                    *
; *                                                                           *
; * ENTRY REQUIREMENTS: A = ACA Integer                                       *
; *                     B = ACB Integer                                       *
; *                     U = Math Operation (MOP)                              *
; *                         (only least significant byte is relevant)         *
; *                                                                           *
; * EXIT CONDITIONS:	D = Result                                            *
; *                     All other registers preserved                         *
; *****************************************************************************
KRNL_8BIT_MATH	pshs	U,CC		; save the used registers onto the stack
		; A to ACA
		clr	MATH_ACA_INT+0	; clear unneeded byte
		clr	MATH_ACA_INT+1	; clear unneeded byte
		clr	MATH_ACA_INT+2	; clear unneeded byte
		sta	MATH_ACA_INT+3	; store A in the ACA integer register
		; B to ACB
		clr	MATH_ACB_INT+0	; clear unneeded byte
		clr	MATH_ACB_INT+1	; clear unneeded byte
		clr	MATH_ACB_INT+2	; clear unneeded byte
		stb	MATH_ACB_INT+3	; store B in the ACB integer register
		; U to MATH_OPERATION
		tfr	U,D		; transfer the MOP instruction to D
		stb	MATH_OPERATION	; send the MOP command (in B)
		; ACR to D
		ldd	MATH_ACR_INT+2	; load the result into the D register
		puls	U,CC,PC		; cleanup saved registers and return

; *****************************************************************************
; * KRNL_DSP_AC(A, B, or R)                                                   *
; * 	Displays the floating point number in one of the FP registers.        *
; *                                                                           *
; * ENTRY REQUIREMENTS: none                                                  *
; *                                                                           *
; * EXIT CONDITIONS:    All registers preserved                               *
; *****************************************************************************
KRNL_DSP_ACA	pshs	X,CC		; save the used registers onto the stack
		ldx	#MATH_ACA_POS
		bsr	KRNL_DSP_HELPER
		puls	X,CC,PC		; cleanup saved registers and return

KRNL_DSP_ACB	pshs	X,CC		; save the used registers onto the stack
		ldx	#MATH_ACB_POS
		bsr	KRNL_DSP_HELPER
		puls	X,CC,PC		; cleanup saved registers and return

KRNL_DSP_ACR	pshs	X,CC		; save the used registers onto the stack
		ldx	#MATH_ACR_POS
		bsr	KRNL_DSP_HELPER
		puls	X,CC,PC		; cleanup saved registers and return

;HELPER:  X=address of a FP_POS register pointed to by X
KRNL_DSP_HELPER	pshs  	A,CC		; save the used registers onto the stack
		clr	,x
K_DSP_FP_0	lda	1,x
		jsr	KRNL_CHROUT
		bne	K_DSP_FP_0
		puls	A,CC,PC		; cleanup saved registers and return

; *****************************************************************************
; * KRNL_WRITE_AC(A, B, or R)                                                 *
; * 	Sets one of the floating point registers to a FP value contained      *
; *     within a null-terminated string pointed to by X.                      *
; *                                                                           *
; * ENTRY REQUIREMENTS: X = points to a null-terminated string of numbers     *
; *                                                                           *
; * EXIT CONDITIONS:    All registers preserved                               *
; *****************************************************************************
KRNL_WRITE_ACA	pshs	X,Y,CC		; save the used registers onto the stack
		ldy	#MATH_ACA_POS	; point to the ACA chr pos register
		bsr	KRNL_WRITE_HLP	; display the number to the console
		puls	X,Y,CC,PC	; cleanup saved registers and return

KRNL_WRITE_ACB	pshs	X,Y,CC		; save the used registers onto the stack
		ldy	#MATH_ACB_POS	; point to the ACB chr pos register
		bsr	KRNL_WRITE_HLP	; display the number to the console
		puls	X,Y,CC,PC	; cleanup saved registers and return

KRNL_WRITE_ACR	pshs	X,Y,CC		; save the used registers onto the stack
		ldy	#MATH_ACR_POS	; point to the ACR chr pos register
		bsr	KRNL_WRITE_HLP	; display the number to the console
		puls	X,Y,CC,PC	; cleanup saved registers and return	

; X string to write, Y = ACn_POS
KRNL_WRITE_HLP	pshs	X,Y,CC		; save the used registers onto the stack
		clr	,y+		; set the chr pos to the start
KRNL_WRITE_0	lda	,x+		; load the next char from the string
		beq	KRNL_WRITE_DONE	; were done if it's a null-terminator
		sta	,y		; store the char into the FP port
		bra	KRNL_WRITE_0	; continue looping
KRNL_WRITE_DONE	puls	X,Y,CC,PC	; cleanup saved registers and return





; *****************************************************************************
; * KRNL_ARG_TO_A                                                             *
; * 	convert a numeric string (pointed to by X) to 0-25 and return it in A *
; *                                                                           *
; * ENTRY REQUIREMENTS: X = points to the string to be converted              *
; *                         Note: hex values must be preceeded                *
; *                               with a '$' character                        *
; *                                                                           *
; * EXIT CONDITIONS:	A = binary value represented by the input string      *
; *                     All other registers preserved                         *
; *****************************************************************************
KRNL_ARG_TO_A	pshs	B,X,CC
		ldb	,x+
		cmpb	#'$'
		beq	KARG_0
		jsr	KRNL_WRITE_ACA
		lda	MATH_ACA_INT+3
		bra	KARG_DONE
KARG_0		ldb	,x+
		bsr	KARG_HEX
		lslb
		lslb
		lslb
		lslb
		pshs	b
		ldb	,x+
		bsr	KARG_HEX
		ora	,s+
KARG_DONE	puls	B,X,CC,PC	; clean up and return
KARG_HEX	pshs	b		; save it
		subb	#'0'		; convert to binary
		bmi	2f		; go if not numeric
		cmpb	#$09		; is greater than 9?
		bls	1f		; branch if not
		orb	#$20
		subb	#$27
1		cmpb	#$0f		; greater than 15?
		bls	3f		; go if not
2		ldb	#$ff
3		cmpb	,s+
		tfr	b,a
		rts




; *****************************************************************************
; * SUBROUTINE_TEMPLATE                                                       *
; * 	xxxxxxxxxxxxxxxxxx                                                    *
; *                                                                           *
; * ENTRY REQUIREMENTS: A = xxxxxxxxxxx                                       *
; *                     B = xxxxxxxxxxx                                       *
; *                                                                           *
; * EXIT CONDITIONS:	D = Result                                            *
; * EXIT CONDITIONS:    All registers preserved                               *
; *                     All other registers preserved                         *
; *****************************************************************************






; *****************************************************************************
; * ROM BASED HARDWARE VECTORS                                                *
; *****************************************************************************
		org	$FFF0

		fdb	KRNL_EXEC	; HARD_RSRVD       EXEC Interrupt Vector
		fdb	KRNL_SWI3  	; HARD_SWI3        SWI3 Hardware Interrupt Vector
		fdb	KRNL_SWI2  	; HARD_SWI2        SWI2 Hardware Interrupt Vector
		fdb	KRNL_FIRQ  	; HARD_FIRQ        FIRQ Hardware Interrupt Vector
		fdb	KRNL_IRQ    	; HARD_IRQ         IRQ Hardware Interrupt Vector
		fdb	KRNL_SWI    	; HARD_SWI         SWI / SYS Hardware Interrupt Vector
		fdb	KRNL_NMI    	; HARD_NMI         NMI Hardware Interrupt Vector
		fdb	KRNL_RESET 	; HARD_RESET       RESET Hardware Interrupt Vector

