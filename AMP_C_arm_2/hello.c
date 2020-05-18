#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>

#include "amp.h"
/*
 *  ======== main ========
 */


Int main()
{ 
   // System_printf("size:   %d", sizeof(short));
//    enableTimeshtamp();
//
//    int t = getTimeshtamp();
//
//    //main_amp();
//    System_printf("size:   %d \n", t);
//    resetTimeshtamp();
//
//    int t1 = getTimeshtamp();
//    System_printf("size:   %d \n", t1);

    give_marker(0x99);

    return(0);
}
