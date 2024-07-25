; filetest.asm

		; include "../build/kernel_f000.sym"
		INCLUDE "kernel_header.asm"

		org	$0000
		fdb	start

		org	$3000
start		jmp	skip_data


color_attrib	fcb	0
file_handle	fcb	0
txt1		fcn	"\n\nHello World!\n"
test_file	fcn	"/home/jay/GitHub/alpha_6809/build/test.txt"
file_error	fcn	"ERROR: File does not exist!\n"
file_EOF	fcn	"ERROR: Input past end of file!\n"

file_contents 	fcc	"\n"
		fcc	"This is a test file.\n"
		fcc	"And a second line to test!\n"
		fcc	"What about a third line?\n"
		fcc	"Apparently three isn't enough.\n"
		fcc	"\n"
		fcc	"This is another line, right here!\n"
		fcc	"Yup! Look! Up there!\n"
		fcn	"\n"
append_line	fcn	"This line was appended to the back.\n"


skip_data	
		; save the current color attribute
		lda	_ATTRIB
		sta	color_attrib
		lda	#$F4
		sta	_ATTRIB

		jsr	write_the_file
		jsr	append_the_file
		jsr	read_the_file
		rts

write_the_file
		jsr	push_the_path

		; open the file for writing
		lda	#FC_OPENWRITE
		sta	FIO_COMMAND
		
		; save the file handle
		lda	FIO_HANDLE
		sta	file_handle

		; send the text
		ldx	#file_contents
1		
		lda	,x+
		beq	2f
		sta	FIO_IODATA
		lda	#FC_WRITEBYTE
		sta	FIO_COMMAND
		lda	FIO_ERROR
		beq	1b

2		; close the file

		bra 	done


append_the_file
		jsr	push_the_path

		; does it exist
		lda	#FC_FILEEXISTS
		sta	FIO_COMMAND
		lda	FIO_ERROR
		cmpa	#FE_NOTFOUND
		bne	2f
		ldx	#file_error
		jsr	[VEC_LINEOUT]
		bra	done	
2		; open the file for appending
		lda	#FC_OPENAPPEND
		sta	FIO_COMMAND
		; save the file handle
		lda	FIO_HANDLE
		sta	file_handle

		; send the data
		ldx	#append_line
		bra	1b
		
		
read_the_file
		jsr	push_the_path
		lda	#$F4
		sta	_ATTRIB

		; does it exist
		lda	#FC_FILEEXISTS
		sta	FIO_COMMAND
		lda	FIO_ERROR
		cmpa	#FE_NOTFOUND
		bne	2f
not_found	ldx	#file_error
		jsr	[VEC_LINEOUT]
		bra	done		

2		; open the file for reading
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
		; restore the color attrib
		lda 	color_attrib
		sta	_ATTRIB

		; restore the handle
		lda	file_handle
		sta	FIO_HANDLE

		; close the file
		lda	#FC_CLOSEFILE
		sta	FIO_COMMAND
		
		rts		
		
push_the_path
		; push the file path
		clr	FIO_PATH_POS
		ldx	#test_file
0		lda	,x+
		sta	FIO_PATH_DATA
		bne	0b

		rts