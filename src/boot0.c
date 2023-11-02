#include <registers.h>
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <boot1_key.h>
#include "hw/aes_eng.h"
#include "wii/wiitypes.h"
#include "../config.h"

#define MEM1_UNCACHED_OFFSET 0xC0000000
#define MEM1_CACHED_OFFSET   0x80000000
#define MEM2_CACHED_OFFSET   0x90000000
#define MEM2_UNCACHED_OFFSET 0xD000000
#define YES_OR_NO(b) b ? "Yes" : "No"

void dump(){
    printf("---Falcon Information---\n");
    printf("Current stage: boot0 (Nintendo)\n");
    printf("Falcon version: %s\n", PACKAGE_VERSION);
    printf("---Build options---\n");
    printf("Use boot0 security? %s\n", YES_OR_NO(ENABLE_BOOT0_SECURITY));
    printf("Memory offset: 0x%x", MEM2_UNCACHED_OFFSET);
}



void boot0_main(){
    wiiAESKey boot1_key_ptr = (wiiAESKey)malloc(0xFFFF);
    wiiIV boot1_iv_ptr = (wiiIV)malloc(16);
    unsigned long* aesRegisters = (wiiMemPtr)malloc(0x14);
    unsigned int memvalD000000; // 0xD000000 is top of Mem2???
    unsigned int memvalD800000;
    unsigned int memvalD800060;
    // boot1 IV is empty...
    *(boot1_key_ptr) = (unsigned long)BOOT1_KEY_P1;
    *(boot1_key_ptr + 4) = (unsigned long)BOOT1_KEY_P2;
    *(boot1_key_ptr + 8) = (unsigned long)BOOT1_KEY_P3;
    *(boot1_key_ptr + 12) = (unsigned long)BOOT1_KEY_P4;
    unsigned int* varR2 = &memvalD000000;
    unsigned long* varR3 = aesRegisters;
    //*(registers + R9) = 0;
    //*(registers + R1) = 7;
    varR2 = &memvalD800000;
    memvalD800060 = 7;
    *(registers + R2) = *(registers + R11) - 0x54;
    *(registers + R9) = *(mem2Uncached + *(registers + R3));
    *(registers + R1) = (unsigned long)boot1_key_ptr; // boot1 key
    *(registers + R0) = *(registers + R3);
    *(registers + LR) = 0xD400000 - MEM2_UNCACHED_OFFSET;
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
        *(mem2Uncached + *(registers + R0) + 0xC) = *(registers + R3);
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
        *(mem2Uncached + *(registers + R12) + 0x10) = *(registers + R3);
    }
    printf("set aes iv\n");
    /* oh no */
    *(registers + R3) = 0x67452301; // initial SHA context
    *(registers + R1) = 0xD030000 - MEM2_UNCACHED_OFFSET; // AFAIK there's no difference
    // between MOVL and MOV we need to watch out for here...
    *(registers + R0) = 0;
    *(registers + R2) = 0xEFCDAB89;
    // FFFF0190                 STR     LR, [R12,#4]
    *(mem2Uncached + *(registers + R12 + 0x4)) = *(registers + LR);
    // FFFF0194                 STR     LR, [R12,#8]
    *(mem2Uncached + *(registers + R12) + 0x8) = *(registers + LR);
    // FFFF0198                 STR     R0, [R1]
    *(mem2Uncached + *(registers + R1)) = *(registers + R0);
    // FFFF019C                 STR     R3, [R1,#8]
    *(mem2Uncached + *(registers + R1) + 0x8) = *(registers + R3);
    // FFFF01A0                 LDR     R3, =0x98BADCFE
    *(registers + R3) = 0x98BADCFE; // the coffee wasn't
    // THAT bad
    // In all seriousness, that's part 3/5 of the boot1
    // SHA1
    // FFFF01A4                 STR     R2, [R1,#0xC]
    *(mem2Uncached + *(registers + R1) + 0xC) = *(registers + R2);
    // FFFF01A8                 LDR     R2, =0x10325476
    *(registers + R2) = 0x10325476;
    // FFFF01AC                 STR     R3, [R1,#0x10]
    *(mem2Uncached + *(registers + R1) + 0x10) = *(registers + R3);
    // FFFF01B0                 LDR     R3, =0xC3D2E1F0
    *(registers + R3) = 0xC3D2E1F0;
    // FFFF01B4                 STR     R2, [R1,#0x14]
    *(mem2Uncached + *(registers + R1) + 0x14) = *(registers + R2);
    // FFFF01B8                 MOV     R2, #0xD400000
    *(registers + R2) = 0xD400000 - MEM2_UNCACHED_OFFSET;
    // FFFF01BC                 STR     R3, [R1,#0x18]
    *(mem2Uncached + *(registers + R1) + 0x18) = *(registers + R3);
    // FFFF01C0                 STR     R2, [R1,#4]
    *(mem2Uncached + *(registers + R1) + 0x4) = *(registers + R2);
    // FFFF01C4                 MOVL    R3, 0xD010000
    *(registers + R3) = 0xD010000 - MEM2_UNCACHED_OFFSET;
    // FFFF01CC                 LDR     R2, [R3,#4]
    *(registers + R2) = *(mem2Uncached + *(registers + R3) + 0x4);
    // FFFF01D0                 MOVL    R1, 0x80FF0000
    *(registers + R3) = 0x0FF0000;
    // FFFF01D8                 ORR     R2, R2, #0x8000000
    *(registers + R2) = *(registers + R2) | 0x8000000;
    // FFFF01DC                 ADD     R1, R1, #0x8000
    *(registers + R1) += 0x8000;
    // FFFF01E0                 STR     R2, [R3,#4]
    printf("R3 = 0x%x\n", *(registers + R3));
    *(mem1Cached + *(registers + R3) + 4) = *(registers + R2); // sigsegv here
    // FFFF01E4                 MOV     R4, #0xD800000
    *(registers + R3) = 0xD800000 - MEM2_UNCACHED_OFFSET;
    /* clearing memory values? Apepars to be from 0x8-0xC*/
    // FFFF01E8                 STR     R0, [R3,#0x10]
    *(mem2Cached + *(registers + R3) + 0x10) = *(registers + R0);
    // FFFF01EC                 STR     R0, [R3,#0x14]
    *(mem2Cached + *(registers + R3) + 0x14) = *(registers + R0);
    // FFFF01F0                 STR     R0, [R3,#8]
    *(mem2Cached + *(registers + R3) + 0x8) = *(registers + R0);
    // FFFF01F4                 STR     R0, [R3,#0xC]
    *(mem2Cached + *(registers + R3) + 0xC) = *(registers + R0);
    // FFFF01F8                 STR     R1, [R3]
    *(mem2Cached + *(registers + R3)) = *(registers + R1);
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