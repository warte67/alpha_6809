; test.asm

		include "../build/kernel_f000.sym"

		org	$0000
		fdb	start

		org	$3000
start		bra	skip_data



txt1		fcn	"Hello World!01234567890123456789012345678901234567890123456789012345678901234567890123456789\n"




skip_data	pshs	X
		lda	KRNL_ATTRIB
		tfr	a,b
		ora	#$f0
		sta	KRNL_ATTRIB

		ldx	#txt1
		jsr	KRNL_LINEOUT

		stb	KRNL_ATTRIB
		puls	X,PC

