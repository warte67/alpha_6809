; test.asm

		; include "../build/kernel_f000.sym"
		INCLUDE "kernel_header.asm"

		org	$0000
		fdb	start

		org	$3000
start		jmp	skip_data



file_handle	fcb	0
txt1		fcn	"\n\nHello World!\n"
test_file	fcn	"/home/jay/dev/alpha_6809/build/test.txt"
file_error	fcn	"ERROR: File does not exist!\n"
file_EOF	fcn	"ERROR: Input past end of file!\n"


skip_data	
		; push the file path
		clr	FIO_PATH_POS
		ldx	#test_file
0		lda	,x+
		sta	FIO_PATH_DATA
		bne	0b
		; open the file for reading
		lda	#FC_OPENREAD
		sta	FIO_COMMAND
		; save the file handle
		lda	FIO_HANDLE
		sta	file_handle

1
		; read a byte
		lda	#FC_READBYTE
		sta	FIO_COMMAND
		ldb	FIO_ERROR
		bne	done
		; output the character
		lda	FIO_IODATA
		jsr	[VEC_CHROUT] 
		bra	1b




done
		; restore the handle
		lda	file_handle
		sta	FIO_HANDLE

		; close the file
		lda	#FC_CLOSEFILE
		sta	FIO_COMMAND
		
		rts		
		
