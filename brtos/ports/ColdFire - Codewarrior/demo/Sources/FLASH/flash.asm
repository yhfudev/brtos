;**************************************************************
;* This stationery is meant to serve as the framework for a   *
;* user application. For a more comprehensive program that    *
;* demonstrates the more advanced functionality of this       *
;* processor, please see the demonstration applications       *
;* located in the examples subdirectory of the                *
;* Metrowerks Codewarrior for the HC08 Program directory      *
;**************************************************************
; export symbols
            XDEF FlashErase
            XDEF FlashProg
            ; we use export 'Entry' as symbol. This allows us to
            ; reference 'Entry' either in the linker .prm file
            ; or from C/C++ later on
                       
; include derivative specific macros

            Include 'mc9s08aw60.inc'

; Flash commands:
;mByteProg  equ   $20    ; Byte program command
;mPageErase equ   $40    ; Page erase command


;**************************************************************
; this assembly routine is called the C/C++ application
;*****************************************************************

DoOnStack:
          pshx                  ; Flash address
          pshh
          psha                  ; Flash command
          ldhx   #SpSubEnd      ; Last byte to move to stack
SpMoveLoop:
          lda    ,x             ; Transfer byte to stack
          psha
          aix    #-1            ; Adjust stack pointer
          cphx   #SpSub-1       ; Test for end of sub-routine data
          bne    SpMoveLoop     ; Loop if not
          tsx                   ; H:X = start of stack subroutine
          tpa                   ; Move CCR to ACC for testing
          and    #$08           ; Check I mask
          bne    I_set          ; Branch if already set
          sei                   ; Disable interrupts
          lda    SpSubSize+6,sp ; Command byte
          jsr    ,x             ; Execute subroutine on stack
          cli                   ; Re-enable interrupts
          bra    I_cont         ; Branch always

I_set:    lda    SpSubSize+6,sp ; Command byte
          jsr    ,x             ; Execute subroutine on stack
I_cont:   ais    #SpSubSize+3   ; De-allocate stack memory used
                                ; H:X unchanged from SpSub
          lsla                  ; ACC is non-zero if PVIOL or ACCERR
          rts		
;**************************************************************

; Flash commands:
;mByteProg  equ   $20    ; Byte program command
;mPageErase equ   $40    ; Page erase command

;*****************************************************************
; FlashProg1
; ON ENTRY:  H:X = flash address to be programmed
;            ACC = byte value to be programmed
; ON RETURN: H:X unchanged & ACC = FSTAT << 1
;            ZF = 1 if OK, ZF = 0 if protect violation or access error
; Uses 32 bytes on stack + 2 bytes for call return address

FlashProg:
          psha                  ; Save byte value to be programmed
          lda    #(mFSTAT_FPVIOL|mFSTAT_FACCERR)
          sta    FSTAT          ; Abort any command & clear errors
          lda    #mByteProg     ; Byte program command
          bsr    DoOnStack      ; Execute code from stack
          ais    #1             ; Adjust stack pointer
          rts

;*****************************************************************
; FlashErase1
; ON ENTRY:  H:X = address within flash page to be erased
; ON RETURN: H:X unchanged & ACC = FSTAT << 1
;            ZF = 1 if OK, ZF = 0 if protect violation or access error
; Uses 32 bytes on stack + 2 bytes for call return address

FlashErase:
          psha                  ; Adjust SP for DoOnStack entry
          lda    #(mFSTAT_FPVIOL|mFSTAT_FACCERR)
          sta    FSTAT          ; Abort any command & clear errors
          lda    #mPageErase    ; Page erase command
          bsr    DoOnStack      ; Execute code from stack
          ais    #1             ; Adjust stack pointer
          rts

;*****************************************************************

SpSub:    ldhx   <SpSubSize+4,sp; Get flash address from stack
          sta    ,x             ; Latch address & data
          lda    SpSubSize+3,sp ; Get flash command
          sta    FCMD           ; Write flash command
          lda    #mFSTAT_FCBEF  ; Mask to initiate command
          sta    FSTAT          ; [pwpp]
          ; Pad for min 4~ from write to read so FCCF is valid
          nop                   ; [p]
ChkDone:  lda    FSTAT          ; [prpp]
          lsla                  ; MS bit = FCCF
          bpl    ChkDone        ; Wait for FCCF flag set
SpSubEnd: rts                   ; Return to DoOnStack in flash

SpSubSize: equ   (*-SpSub)