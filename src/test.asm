; test.asm

		include "../build/kernel_f000.sym"
		* INCLUDE "memory_map.asm"

		org	$0000
		fdb	start

		org	$3000
start		jmp	skip_data



txt1		fcn	"\n\nHello World!\n"

test_file	fcn	"/home/jay/dev/alpha_6809/build/test.txt"
file_error	fcn	"ERROR: File does not exist!\n"


skip_data	
		; FC_GETLENGTH

		ldx	#test_file
		clr	FIO_PATH_POS
0		
		lda	,x+
		sta	FIO_PATH_DATA
		bne	0b
		lda	#FC_GETLENGTH
		sta	FIO_COMMAND
		lda	FIO_ERR_FLAGS
		cmpa	#$80		; file not found error flag
		bne	1f
		ldx	#file_error
		jsr	KRNL_LINEOUT
		bra	done
1		
		jsr	KRNL_DPS_INTR	; KRNL_DSP_ACR
		jsr	KRNL_NEWLINE

		

		nop

		; test text
		* bsr	test_text
done		rts

test_text	pshs	X
		lda	KRNL_ATTRIB
		tfr	a,b
		ora	#$f0
		sta	KRNL_ATTRIB

		ldx	#txt1
		jsr	KRNL_LINEOUT

		stb	KRNL_ATTRIB
		puls	X,PC

