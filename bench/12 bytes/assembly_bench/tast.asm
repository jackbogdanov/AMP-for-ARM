
text
  .def _c_int00
  .text
  .retain


DATA_ADR:           		.word 0xC000100
BUFF:				.word 0xC000400

FINISH_DSP_FLAG:		.word 0xC000050
START_DSP_FLAG:			.word 0xC000060
START_ARM: 			.word 0xC000040
FINISH_ARM:             	.word 0xC000030
CYCLES_COUNT: 			.word 128 	 ;MUST BE POWER OF TWO

TIME_RESULTS:			.word 0xC000200

;trunsfer data:
data_type:			.word 0x33771144

_c_int00:
;init
	ldr r1, FINISH_DSP_FLAG
	ldr r2, START_DSP_FLAG
	ldr r9, CYCLES_COUNT
	ldr r0, DATA_ADR

;prepare result registers
	mov r10, #0
	mov r11, #0
	mvn r12, #0xF0000000

;prepare start buff
	ldr r8, BUFF
	ldr r3, data_type
  	ldr r4, data_type
  	ldr r5, data_type

  	ldr r6, TIME_RESULTS
	ldr r8, BUFF
  	stmia r8, {r3, r4, r5}

;enable cycle counter
	mov r3, #0x1
	mcr p15, #0, r3, c9, c14, #0	;PMUSERENR = 1
	mcr p15, #0, r3, c9, c12, #0	;PMCR.E (bit 0) = 1

	lsl r3, r3, #31
	mcr p15, #0, r3, c9, c12, #1	;PMCNTENSET.C (bit 31) = 1

mainloop:

;waiting START_ARM bit from DSP
	ldr r8, START_ARM
	ldr r5, [r8]
	cmp r5, #0x1 		;start arm value 0x1
   	bne mainloop

;restart timer
	mrc p15, #0, r3, c9, c12, #0
	orr r3, r3, #4
	mcr p15, #0, r3, c9, c12, #0


;read start time
	mrc p15, #0, r7, c9, c13, #0

;load data to MSMC memory
	ldr r8, BUFF

	ldmia r8, {r3, r4, r5}
  	stmia r0, {r3, r4, r5}

;clear FIFNSH_ARM flag
	ldr r8, FINISH_ARM

	mov r3, #0x0
	str r3, [r8]

;give start command to DSP
	mvn r3, #0x0   		;start flag value is 0xFFFFFFFF
	str r3, [r2]

;waiting while DSP reading memory
loop:
	ldr r5, [r1]
	cmp r5, #0x1 		;finish flag value 0x1
   	bne loop

;count measure results
	MRC p15, #0, r3, c9, c13, #0

	sub r7, r3, r7

	str r7, [r6], #4
	add r10, r10, r7 	;count sum of all results

	cmp r7, r11
	movgt r11, r7    	;count max

	cmp r7, r12
	movlt r12, r7    	;count min

;decrement iterations counter
	sub r9, r9, #1
	cmp r9, #0

;tell DSP that ARM finished time measurement cycle
	ldr r8, FINISH_ARM

	mvn r3, #0x0   		;start flag value is 0xFFFFFFFF
	str r3, [r8]

;start new measure cycle
	bne mainloop

;save all results
	lsr r10, r10, #7	;devide sum by num of cycles

	ldr r6, TIME_RESULTS
	stm r6, {r10, r11, r12}

  .END

