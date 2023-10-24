#include <aes_eng.h>

void callAESEngine(unsigned int *memory){
    if (GET_KTH_BIT(AES_CTRL, 0) == 1){
        printf("aes engine run\n");

    } else if (GET_KTH_BIT(AES_CTRL, 0) == 0) {
        printf("aes reset!\n")
    }
}