#include <registers.h>
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <boot1_key.h>
#include "hw/aes_eng.h"
#include "../config.h"

#define OFFSET 0xD000000

unsigned int *registers;
unsigned int *memory;
unsigned int *sp;

void pushOntoStack(unsigned int *armRegister){
    *sp = *armRegister;
    sp += sizeof(*armRegister);
}

// pushes r4-r14 onto stack
void pushRegisters(){
    for (int i = R4; i <= R14; i = i + 4){
        pushOntoStack((registers + i));
    }
}

void dump(){
    printf("---Registers---\n");
    for (int i = R0; i <= R14; i = i + 4){
        printf("R%d = %d\n", (int)round(i / 4), *(registers + i));
    }
}

void boot0_main(){
    unsigned int *boot1_key_ptr = (unsigned int*)malloc(0xFFFF); // we kinda cheat here
    unsigned int *boot1_iv_ptr = (unsigned int*)malloc(16);
    // boot1 IV is empty...
    *(boot1_key_ptr) = (unsigned long)BOOT1_KEY_P1;
    *(boot1_key_ptr + 4) = (unsigned long)BOOT1_KEY_P2;
    *(boot1_key_ptr + 8) = (unsigned long)BOOT1_KEY_P3;
    *(boot1_key_ptr + 12) = (unsigned long)BOOT1_KEY_P4;
    *(registers + R12) = *(registers + SP); // preserve SP onto R12
    // I don't see why you'd want that but
    // whatever
    pushRegisters();
    *(registers + R3) = 0xD000000 - OFFSET;
    *(registers + R11) = *(registers + R12) + 4;
    *(registers + R3) += 0x20000; // AES command register
    *(registers + R9) = 0;
    *(registers + R1) = 7;
    *(registers + R2) = 0xD800000 - OFFSET;
    // segfault 1 happens here!
    *(memory + *(registers + R2) + 0x60) = *(registers + R1); // here
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  <- specifically here
    *(registers + R2) = *(registers + R11) - 0x54;
    // segfault 2 happens here!
    *(registers + R9) = *(memory + *(registers + R3)); // here
    //^^^^^^^^^^^^^^^^^  <- specifically here
    *(registers + R1) = (unsigned long)boot1_key_ptr; // boot1 key
    *(registers + R0) = *(registers + R3);
    *(registers + LR) = 0xD400000 - OFFSET;
    *(registers + R2) = 3; // number of loops
    /* set_AES_key */
    /*
    FFFF014C                 LDR     R3, [R1],#4     ; use hardcoded boot1 key
    FFFF0150                 SUBS    R2, R2, #1
    FFFF0154                 STR     R3, [R0,#0xC]
    FFFF0158                 BPL     set_AES_key
    */
   #ifdef ENABLE_BOOT0_SECURITY
   printf("setting aes key...\n");
   while (*(registers + R2) >= 0){
        // LDR     R3, [R1],#4
        *(registers + R3) = *(registers + R1); *(registers + R1) += 4;
        *(registers + R2) = *(registers + R2) - 1; // pointers don't support --
        // why >:(
        *(memory + *(registers + R0) + 0xC) = *(registers + R3);
        // BPL     set_AES_key; jump back if pos flag is cleared
        // ; a.k.a checks if the R2 subtract ended with a negative number (R2 >= 0)
    }
    printf("set aes key for boot1\n");
    // Jump not taken
    *(registers + R1) = (unsigned long)boot1_iv_ptr;
    #endif

}

int main(){
    printf("Initializing boot0...\n");
    // registers
    registers = (unsigned int*)malloc(4 * 15);
    // this is probably way too much ram than is needed for the boot
    // process, but whatever...
    memory = (unsigned int*)malloc(0xD000000); // 234MB
    sp = registers + SP; 
    printf("registers and mem alloc'd\nmemory = %ld\nregister = %ld\n",
        (unsigned long)memory, (unsigned long)registers
    );
    printf("@ asm _start\n");
    *(registers + R1) = 0;
    *(registers + R4) = 0;
    *(registers + R11) = 0;
    *(registers + R11) = 0; // ??????????????
    printf("Starting boot0...\n");
    boot0_main();
    printf("!!!  P  A  N  I  C  !!!\nBOOT0 CRASH!\n===DUMP===\n");
    dump();
    return 0;
}