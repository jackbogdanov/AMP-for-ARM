
MEMORY
{

    MSM_SRAM0 :  o = 0x0C000000,  l = 0x00180000  /* 1.5MB MSMC Shared SRAM Core 0 */
    MSM_SRAM1 :  o = 0x0C180000,  l = 0x00180000  /* 1.5MB MSMC Shared SRAM Core 1 */
    MSM_SRAM2 :  o = 0x0C300000,  l = 0x00180000  /* 1.5MB MSMC Shared SRAM Core 2 */
    MSM_SRAM3 :  o = 0x0C480000,  l = 0x00180000  /* 1.5MB MSMC Shared SRAM Core 3 */
    DDR0_C0   :  o = 0x80000000,  l = 0x20000000  /* 512MB external DDR0 Core 0 */
    DDR0_C1   :  o = 0xA0000000,  l = 0x20000000  /* 512MB external DDR0 Core 1 */
    DDR0_C2   :  o = 0xC0000000,  l = 0x20000000  /* 512MB external DDR0 Core 2 */
    DDR0_C3   :  o = 0xE0000000,  l = 0x20000000  /* 512MB external DDR0 Core 3 */

}

SECTIONS
{
    .text          >  MSM_SRAM3
    .stack         >  MSM_SRAM3
    .bss           >  MSM_SRAM3
    .cio           >  MSM_SRAM3
    .const         >  MSM_SRAM3
    .data          >  MSM_SRAM3
    .switch        >  MSM_SRAM3
    .sysmem        >  MSM_SRAM3
    .far           >  MSM_SRAM3
    .args          >  MSM_SRAM3
    .ppinfo        >  MSM_SRAM3
    .ppdata        >  MSM_SRAM3

    //.word          >  MSM_SRAM3
  
    /* COFF sections */
    .pinit         >  MSM_SRAM3
    .cinit         >  MSM_SRAM3
  
    /* EABI sections */
    .binit         >  MSM_SRAM3
    .init_array    >  MSM_SRAM3
    .neardata      >  MSM_SRAM3
    .fardata       >  MSM_SRAM3
    .rodata        >  MSM_SRAM3
    .c6xabi.exidx  >  MSM_SRAM3
    .c6xabi.extab  >  MSM_SRAM3
}


