; test.asm

		; include "../build/kernel_f000.sym"
		INCLUDE "kernel_header.asm"

		org	$0000
		fdb	start

		org	$3000
start		jmp	skip_data

color_attrib	fcb	0
test_str	fcn	"test.asm\n"
folder_path	fcn	"/home/jay/dev/alpha_6809/build/test"
; data here ...


skip_data
		lda	_ATTRIB
		sta	color_attrib
		lda	#$F4
		sta	_ATTRIB


		lda	#$12
		sta	MEMBANK_ONE

		lda	#$34
		sta	MEMBANK_TWO

		ldx	#MEMBANK_ONE		
0		sta	,x
		inc	,x+
		inca
		cmpx	#MEMBANK_END
		blt	0b


		
* 		; display the test.asm string
* 		ldx	#test_str
* 0		lda	,x+
* 		beq	1f
* 		jsr	[VEC_CHROUT]
* 		bra	0b		
* 1	
* 		; write the folder path to the IO Data port
* 		clr 	FIO_PATH_POS
* 		ldx	#folder_path
* 2		lda	,x+
* 		sta	FIO_PATH_DATA
* 		bne	2b
* 		; create the folder
* 		lda	#FC_MAKEDIR
* 		sta	FIO_COMMAND



done
		; restore the color attrib
		lda 	color_attrib
		sta	_ATTRIB
		
		rts


