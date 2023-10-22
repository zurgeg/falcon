// Reimpl of boot0.s in C

/*
FFFF0040                 MOV     R1, #0
FFFF0044                 MOV     R4, #0
FFFF0048                 MOV     R11, #0
FFFF004C                 MOV     R11, #0
FFFF0050                 MOV     LR, #0
*/
#include <boot1_key.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
unsigned char *global0; // r1
int global1 = 0x0; // r4
int global2 = 0x0; // r11

// main
void realmain(){
    int var1, var2, var3, var4, var5; // registers
    int memval_D800060; // memory
    int *aesCommandRegister = &memval_D800060; // AES command register (D800060)
    global0 = (unsigned char*)malloc(16);
    // MOV     R12, SP
    var1 = 0xD417C00; // top of SRAM, R12
    // SP is set to 0xD417C00 in _start, so it doesn't matter
    // STMFD   SP!, {R4-R12,LR,PC}
    // push args onto stack(?)
    // MOV     R3, #0xD000000
    var2 = 0xD000000; // R3
    // SUB     R11, R12, #4
    global2 = var1 - 0x4;
    // ADD     R3, R3, #0x20000 ; R3 = 0D020000 = AES command reg
    var2 = var2 + 0x20000; // why wasn't this the case when init?
    var3 = 0; // R9
    var4 = 7; // R1
    var5 = 0xD800000; // R2
    // STR     R1, [R2,#0x60]
    memval_D800060 = var4;
    // SUB     R2, R11, #0x54
    var5 = global2 = 0x54;
    // STR     R9, [R3]         ; write 0 to AES command reg
    // R3 is used as a pointer here, but
    // it just points to AES command register so
    // we dereference it into R9 (var3)
    var3 = *aesCommandRegister;
    *global0 = (unsigned char)BOOT1_KEY;
    printf("boot1 aes key addr = %u", global0);


}

// _start
void main(){
    realmain();
}