#include "lock.h"

void lock(int *p_flags) {

    *p_flags       = 1; //  set flag[0] = true
    *(p_flags + 2) = 0; //  set victim = id (= 0 for arm)

    while(*(p_flags + 1) && *(p_flags + 2) == 0) {

    }
}

void unlock(int *p_flags) {
    *p_flags = 0;
}
