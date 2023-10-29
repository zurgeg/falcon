#include <registers.h>
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <boot1_key.h>
#include "hw/aes_eng.h"
#include "wii/wiitypes.h"
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
        printf("R%d = 0x%x\n", (int)round(i / 4), *(registers + i));
    }
}



void boot0_main(){
    wiiAESKey boot1_key_ptr = (wiiAESKey)malloc(0xFFFF);
    wiiIV boot1_iv_ptr = (wiiIV)malloc(16);
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
    *(memory + *(registers + R2) + 0x60) = *(registers + R1);
    *(registers + R2) = *(registers + R11) - 0x54;
    *(registers + R9) = *(memory + *(registers + R3));
    *(registers + R1) = (unsigned long)boot1_key_ptr; // boot1 key
    *(registers + R0) = *(registers + R3);
    *(registers + LR) = 0xD400000 - OFFSET;
    *(registers + R2) = 4; // number of loops
    /* set_AES_key */
    /*
    FFFF014C                 LDR     R3, [R1],#4     ; use hardcoded boot1 key
    FFFF0150                 SUBS    R2, R2, #1
    FFFF0154                 STR     R3, [R0,#0xC]
    FFFF0158                 BPL     set_AES_key
    */
   #ifdef ENABLE_BOOT0_SECURITY
   printf("setting aes key...\n");
   while (*(registers + R2) >= 1){
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
    *(registers + R12) += 0x20000;
    *(registers + R2) = 4; // reset R2 for next loop
    printf("settings aes iv...\n");
    /* set_AES_iv */
    // similar situation to the key (in fact, this code is just copy-pasted
    // from the set_AES_key code with some modifications)
    while (*(registers + R2) >= 1){
        *(registers + R3) = *(registers + R1); *(registers + R1) += 4;
        *(registers + R2) = *(registers + R2) - 1; // pointers don't support --
        *(memory + *(registers + R12) + 0x10) = *(registers + R3);
    }
    printf("set aes iv\n");
    /* oh no */
    *(registers + R3) = 0x67452301; // initial SHA context
    *(registers + R1) = 0xD030000 - OFFSET; // AFAIK there's no difference
    // between MOVL and MOV we need to watch out for here...
    *(registers + R0) = 0;
    *(registers + R2) = 0xEFCDAB89;
    // FFFF0190                 STR     LR, [R12,#4]
    *(memory + *(registers + R12 + 0x4)) = *(registers + LR);
    // FFFF0194                 STR     LR, [R12,#8]
    *(memory + *(registers + R12) + 0x8) = *(registers + LR);
    // FFFF0198                 STR     R0, [R1]
    *(memory + *(registers + R1)) = *(registers + R0);
    // FFFF019C                 STR     R3, [R1,#8]
    *(memory + *(registers + R1) + 0x8) = *(registers + R3);
    // FFFF01A0                 LDR     R3, =0x98BADCFE
    *(registers + R3) = 0x98BADCFE; // the coffee wasn't
    // THAT bad
    // In all seriousness, that's part 3/5 of the boot1
    // SHA1
    // FFFF01A4                 STR     R2, [R1,#0xC]
    *(memory + *(registers + R1) + 0xC) = *(registers + R2); 
    // FFFF01A8                 LDR     R2, =0x10325476
    // FFFF01AC                 STR     R3, [R1,#0x10]
    // FFFF01B0                 LDR     R3, =0xC3D2E1F0
    // FFFF01B4                 STR     R2, [R1,#0x14]
    // FFFF01B8                 MOV     R2, #0xD400000
    // FFFF01BC                 STR     R3, [R1,#0x18]
    // FFFF01C0                 STR     R2, [R1,#4]
    // FFFF01C4                 MOVL    R3, 0xD010000
    // FFFF01CC                 LDR     R2, [R3,#4]
    // FFFF01D0                 MOVL    R1, 0x80FF0000
    // FFFF01D8                 ORR     R2, R2, #0x8000000
    // FFFF01DC                 ADD     R1, R1, #0x8000
    // FFFF01E0                 STR     R2, [R3,#4]
    // FFFF01E4                 MOV     R4, #0xD800000
    // FFFF01E8                 STR     R0, [R3,#0x10]
    // FFFF01EC                 STR     R0, [R3,#0x14]
    // FFFF01F0                 STR     R0, [R3,#8]
    // FFFF01F4                 STR     R0, [R3,#0xC]
    // FFFF01F8                 STR     R1, [R3]
    // FFFF01FC                 MOV     R3, #0x80000000
    // FFFF0200                 MOV     R6, R0
    // FFFF0204                 STR     R3, [R4,#0x1EC] ; 0D8001EC = 0x80000000
    // FFFF0208                 SUB     R5, R11, #0x3C
    // FFFF020C                 BL      init_gpio_direction
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