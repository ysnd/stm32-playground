.syntax unified
.cpu cortex-m3
.thumb

.global _estack
.global Reset_Handler

.section .isr_vector,"a",%progbits
.word _estack
.word Reset_Handler

.word 0 /* NMI */ 
.word 0 /* HardFault */ 
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0

.section .text
.thumb_func
Reset_Handler:
    bl main
    b .
