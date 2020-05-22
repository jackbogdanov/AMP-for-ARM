#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>

#include "amp.h"
/*
 *  ======== main ========
 */


Int main()
{ 
    marker test_marker;

    test_marker.to = DSP_CORE_ID;
    test_marker.result = -1;
    test_marker.size = 1;
    test_marker.fun_num = 0x99;

    give_marker(&test_marker);

    System_printf("size:   %d \n", test_marker.result);

    return(0);
}
