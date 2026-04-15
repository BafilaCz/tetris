#include "../include/utils.h"
#include <stdlib.h>

// int fce
int randomInt(int min, int max){
    return rand() % (max - min + 1) + min;
}