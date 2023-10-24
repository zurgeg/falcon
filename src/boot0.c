#include <registers.h>
#include <memory.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <boot1_key.h>
#include "hw/aes_eng.h"

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
    *(boot1_key_ptr) = (unsigned long)BOOT1_KEY_P1;
    *(boot1_key_ptr + 4) = (unsigned long)BOOT1_KEY_P2;
    *(boot1_key_ptr + 8) = (unsigned long)BOOT1_KEY_P3;
    *(boot1_key_ptr + 12) = (unsigned long)BOOT1_KEY_P4;
    *(registers + R12) = *sp; // preserve SP onto R12
    // I don't see why you'd want that but
    // whatever
    pushRegisters();
    *(registers + R3) = 0xD000000;
    *(registers + R11) = *(registers + R12) + 4;
    *(registers + R3) += 0x20000; // AES command register
    *(registers + R9) = 0;
    *(registers + R1) = 7;
    *(registers + R2) = 0xD800000;
    printf("boot1 key");
    *(memory + *(registers + R2) + 0x60) = *(registers + R1);
    *(registers + R2) = *(registers + R11) - 0x54;
    *(registers + R9) = *(memory + *(registers + R3));
    *(registers + R1) = (unsigned long)boot1_key_ptr; // boot1 key
    *(registers + R0) = *(registers + R3);
    *(registers + LR) = 0xD400000;
    *(registers + R2) = 3;
    

}

int main(){
    printf("Initializing boot0...\n");
    // registers
    registers = (unsigned int*)malloc(4 * 15);
    // this is probably way too much ram than is needed for the boot
    // process, but whatever...
    memory = (unsigned int*)malloc(0xE000000); // 234MB
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