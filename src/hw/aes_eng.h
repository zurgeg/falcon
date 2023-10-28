// adapts AES engine funcs to tiny-aes-c
#ifndef _AES_ENG_H_
#include <tiny-AES-c/aes.h>
#include "../wii/wiitypes.h"

#define AES_ENGINE_BASE   0x0d020000
#define AES_CTRL          0x0d020000
#define AES_SRC           0x0d020004
#define AES_DEST          0x0d020008
#define AES_KEY           0x0d02000c
#define AES_IV            0x0d020010
#define GET_KTH_BIT(n, k) ((n >> k) & 1)

void callAESEngine(wiiMemPtr *memory);
#endif // _AES_ENG_H