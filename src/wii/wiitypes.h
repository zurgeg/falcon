/* General types */
// Memory pointer (basically a reminder to subtract the respective offset)
typedef unsigned int* wiiMemPtr;
// Semi-useless type for organization's sake
typedef unsigned int* wiiRegister;
/* Stack Types */
/* A pointer to a stack value, i.e., (asm psuedo-code)
```gas
; SP = 0x10
PUSH #0xde
```
Would become (C psuedo code):
```c
wiiStack* stackPointer; // bottom of stack
*(wiiStack - 1) = 0xde; stackPointer--;
```
*/
typedef unsigned int wiiStackValue;
// An array of stack values
typedef wiiStackValue wiiStack;

/* placeholder types */
// 8-bit value
typedef unsigned int unknown8;
// 16-bit value
typedef unknown8 unknown16[2];
// 32-bit value
typedef unknown16 unknown32[2];
// 64-bit value
typedef unknown32 unknown64[2];


/* C-related types */
/* Function-related types */
// An argument passed via a register
typedef wiiRegister registerArgument;
// An argument passed via, you guessed it, the stack!
typedef wiiStackValue stackArgument;
/*
WARNING: If you're using this type, what you are treating
as an argument is probably **NOT** an argument, look for
pointers to this value on stack/in registers, those might 
be arguments
*/
typedef unknown8 memoryArgument;



/* AES engine types */
// AES key (16-bytes)
typedef unsigned int* wiiAESKey;
// IV (16-bytes, maybe too big?)
typedef unsigned int* wiiIV;