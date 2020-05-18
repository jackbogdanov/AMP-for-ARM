
#include "clock.h"


unsigned int getTimeshtamp() {

    unsigned cc;

    __asm__("mrc p15, 0, %0, c9, c13, 0" : "=r" (cc));

    return cc;

}

void resetTimeshtamp() {
    __asm__("mcr p15, 0, %0, c9, c13, 0" :: "r"(1 << 2));

}

void enableTimeshtamp()
{
  // PMUSERENR = 1
    __asm__("mcr p15, 0, %0, c9, c14, 0" :: "r"(1));

  // PMCR.E (bit 0) = 1
    __asm__("mcr p15, 0, %0, c9, c12, 0" :: "r"(1));

  // PMCNTENSET.C (bit 31) = 1
    __asm__("mcr p15, 0, %0, c9, c12, 1" :: "r"(1 << 31));
}

