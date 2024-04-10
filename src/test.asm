; test.asm

		; include "../build/kernel_f000.sym"
		INCLUDE "kernel_header.asm"

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
		lda	FIO_ERROR
		cmpa	#FE_NOTFOUND	; file not found error flag
		bne	1f
		ldx	#file_error
		jsr	[VEC_LINEOUT]
		bra	done
1		
		jsr	[VEC_DSP_INTR]	; KRNL_DSP_ACR
		jsr	[VEC_NEWLINE]

		

		nop

		; test text
		* bsr	test_text
done		rts

test_text	pshs	X
		lda	_ATTRIB
		tfr	a,b
		ora	#$f0
		sta	_ATTRIB

		ldx	#txt1
		jsr	[VEC_LINEOUT]

		stb	_ATTRIB
		puls	X,PC

