	.thumb

	;.def BRTOSStartFirstTask
	.def SwitchContextToFirstTask
	;.def OS_SAVE_SP
	;.def OS_RESTORE_SP

	.ref SPvalue          

	.text

NVIC_VTABLE_R: .word  	0xE000ED08



;BRTOSStartFirstTask:
                                       
 	;ldr r0, NVIC_VTABLE_R			; Use the NVIC offset register to locate the stack. 
 	;ldr r0, [r0]         
 	;ldr r0, [r0]         
 	;msr msp, r0          			; Set the msp back to the start of the stack.
 	;cpsie i 
 	;svc #0               			; System call to start first task.
 	
 	
   	.align 2                        
SPvalue2: .word SPvalue
	
 	
; Save Stack Pointer Define
;OS_SAVE_SP:
	;LDR	  	R1, SPvalue2
	;STR   	R0, [R1]


; Restore Stack Pointer Define
;OS_RESTORE_SP:	
	;LDR	 	R1, SPvalue2
	;LDR  	R0, [R1]
	
SwitchContextToFirstTask:
	LDR	 	R1, SPvalue2
	LDR  	R0, [R1] 	
	LDM     R0, {R4-R11}
	ADDS    R0, R0, #0x20
	MSR     PSP, R0
	ORR     LR, LR, #0x04
	CPSIE   I	
	BX      LR	 	
	
OSIntExit:
	SUBS    R0, R0, #0x20
	STM     R0, {R4-R11}
	LDR	  	R1, SPvalue2
	STR   	R0, [R1]	
	LDR	 	R1, SPvalue2
	LDR  	R0, [R1] 	
	LDM     R0, {R4-R11}
	ADDS    R0, R0, #0x20
	MSR     PSP, R0
	ORR     LR, LR, #0x04
	CPSIE   I	
	BX      LR	 	 

	.end
