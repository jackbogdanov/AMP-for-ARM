#include <xdc/std.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
/*
 *  ======== main ========
 */

#define SIZE 23

#define LOAD_ADR          0x0c000100
#define LOAD_ADR_1        0x0c000200
#define LOAD_ADR_2        0x0c000300

#define FINISH_DSP_FLAG 0x0c000050
#define START_DSP_FLAG  0x0c000060

#define START_ARM       0xC000040
#define FINISH_ARM      0xC000030
#define CYCLES_COUNT    128    //MUST BE POWER OF TWO

#define TIME_RESULTS    0xC000010



Int main()
{ 
    int info[SIZE];
    int *p_data       = (int *) LOAD_ADR;
    int *p1_data      = (int *) LOAD_ADR_1;
    int *p2_data      = (int *) LOAD_ADR_2;

    int *dsp_finish = (int *) FINISH_DSP_FLAG;
    int *dsp_start  = (int *) START_DSP_FLAG;

    int *arm_start   = (int *) START_ARM;
    int *arm_finish  = (int *) FINISH_ARM;
    int iter_count   = CYCLES_COUNT;

    int sum = 0;
    int min = 0x0FFFFFFF;
    int max = 0;

    // init
    for(int i = 0; i < SIZE; ++i) {
        info[i] = 0x55225522;
    }

    while(iter_count) {

        while (*arm_start != 0x1) {}

        unsigned int start_time = Timestamp_get32();

        for(int i = 0; i < SIZE; ++i) {
            p_data[i] = info[i];
        }

        *arm_finish = 0x0;
        *dsp_start  = 0xFFFFFFFF;

         while (*dsp_finish != 0x1) {}

         //read
         for(int i = 0; i < SIZE; ++i) {
             info[i] = p1_data[i];
         }

         //answer
         for(int i = 0; i < SIZE; ++i) {
             p2_data[i] = info[i];
         }

         unsigned int end_time = Timestamp_get32();

         int res = end_time - start_time;

         System_printf("%u \n", res);

         sum += res;

         if (res < min) {
             min = res;
         }

         if (res > max) {
             max = res;
         }

         *arm_finish = 0xFFFFFFFF;
         --iter_count;

    }

    sum /= CYCLES_COUNT;

    int control = 0xFFFFFFFF;

    System_printf("ONE: min: - %x   max: - %x   evg: - %x, control: - %x\n", min, max, sum, control);

    System_printf("TWO: min: - %u   max: - %u   evg: - %u, control: - %u\n", min, max, sum, control);

    return(0);
}


