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
#include <assert.h>
unsigned int *global0; // r1
int global1 = 0x0; // r4
int global2 = 0x0; // r11

unsigned int* memory;

// main
void realmain(){
    int var1, var3, var4, var5, var6; // registers
    int returnValue; // LR
    global0 = (unsigned int*)malloc(16); // R1 is used to store the AES key
    unsigned int *var2 = (uint32_t*)malloc(16); 
    memory =  (unsigned int *)malloc(0x3D09000); // we don't need this much ram
    // but whatever
    int *aesCommandRegister = memory + 0xD800060; // AES command register (D800060)
    // MOV     R12, SP
    var1 = 0xD417C00; // top of SRAM, R12
    // SP is set to 0xD417C00 in _start, so it doesn't matter
    // STMFD   SP!, {R4-R12,LR,PC}
    // push args onto stack(?)
    // MOV     R3, #0xD000000
    *var2 = 0xD000000; // R3
    printf("var2 addr = %u var2 val = %d\n", var2, *var2);
    // SUB     R11, R12, #4
    global2 = var1 - 0x4;
    // ADD     R3, R3, #0x20000 ; R3 = 0D020000 = AES command reg
    *var2 = *var2 + 0x20000; // why wasn't this the case when init?
    var3 = 0; // R9
    var4 = 7; // R1
    var5 = 0xD800000; // R2
    // STR     R1, [R2,#0x60]
    *(memory + (var5 + 60)) = var4;
    // SUB     R2, R11, #0x54
    var5 = global2 - 0x54;
    // STR     R9, [R3]         ; write 0 to AES command reg
    // R3 is used as a pointer here, but
    // it just points to AES command register so
    // we dereference it into R9 (var3)
    var3 = *aesCommandRegister;
    printf("setting boot1 key\n");
    *global0 = BOOT1_KEY_P1; // r1 is the boot1 key
    *(global0 + 4) = BOOT1_KEY_P2;
    *(global0 + 8) = BOOT1_KEY_P3;
    *(global0 + 12) = BOOT1_KEY_P4; // please help
    assert((*global0 == BOOT1_KEY_P1));
    *(memory + 0xD417BA8) = var3; // what is this on real hw?
    var6 = *var2; // R0
    var5 = 3;
    returnValue = 0xD400000;
    printf("set_AES_key start\n");
    for (var5 >= 0; var5--;) {
        // why is this set inside the loop?
        *var2 = *global0; // help me
        *(var2 + 4) = *(global0 + 4);
        *(var2 + 8) = *(global0 + 8);
        *(var2 + 12) = *(global0 + 12);
        // STR     R3, [R0,#0xC]
        // is this really needed? the AES key is stored in a C file
    }
    printf("set_AES_key loop done\n");
    var1 = 0xD000000;
    global0 = 0; // R1 IS NORMAL NOW
    var1 = var1 + 0x20000;
    var5 = 3;
    printf("set_AES_iv start\n");
}

// _start
void main(){
    realmain();
}