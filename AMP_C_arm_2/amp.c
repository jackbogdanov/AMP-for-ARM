#include "amp.h"
#include "lock.h"

// 0x0c3179b2
//--------------to h------------------------
//#define START_ARM_BUFFER_POINTER 0x0c000020
//#define START_DSP_BUFFER_POINTER 0x0c300020

// functions number for available on DSP
#define FUN_NUMBER 0;

#define MARKER_PACKAGE    0x21
#define ANSWER_PACKAGE    0x41
#define END_TRANS_PACKAGE 0x81
#define ERROR_PACKAGE     0x11

#define MARKER_TYPE 1

#define ARM_START_BUFF_P_ADR    0x0c000004
#define ARM_END_BUFF_P_ADR      0x0c000008
#define ARM_BUFF_SIZE           0x0c000000
#define START_ARM_BUFF          0x0c000024

//#define DSP_START_BUFF_P_ADR    0x0c300004
#define DSP_END_BUFF_P_ADR      0x0c180008
#define DSP_BUFF_SIZE           0x0c180000
#define START_DSP_BUFF          0x0c180024
//--------------to h------------------------


#define PACK_SIZE 18
unsigned char package[PACK_SIZE];
unsigned char *main_pointer;

net_p sending_package;

char current_state;
int trusactions_count;

unsigned int *last_end_arm_buff;

//init TODO

void write_num(unsigned int num, int bytes_count) {


    for(int i = bytes_count - 1; i >= 0; --i) {
        int mask = 0x000000FF;

        mask = mask << i*8;

        *main_pointer = (num & mask) >> i*8;
        main_pointer++;
    }
}

unsigned int read_num_by_adr(int bytes_count, unsigned char **buff) {

    unsigned int num = 0;


    for(int i = bytes_count - 1; i >= 0; --i) {
        int mask = 0x000000FF;

        mask = mask << i*8;

        num  = (**buff & mask) >> i*8;
        *buff += 1;
    }

    return num;
}


void write_num_by_adr(unsigned int num, int bytes_count, unsigned char **buff) {


    for(int i = bytes_count - 1; i >= 0; --i) {
        int mask = 0x000000FF;

        mask = mask << i*8;

        **buff = (num & mask) >> i*8;
        *buff += 1;
    }
}

void init() {
    current_state = 'A';
    main_pointer = (unsigned char *) &package;

    trusactions_count = 0;

//    int *p1 = (int *) DSP_END_BUFF_P_ADR;
//    *p1 = 0x0c18011F;
//    p1 = (int *) DSP_BUFF_SIZE;
//    *p1 = 0x00000100;

    last_end_arm_buff = (unsigned int *) START_ARM_BUFF;

    int *p1 = (int *) ARM_BUFF_SIZE;
    *p1 = 0x00000100;

    p1 = (int *) ARM_START_BUFF_P_ADR;
    *p1 = START_ARM_BUFF;
    p1 = (int *) ARM_END_BUFF_P_ADR;
    *p1 = START_ARM_BUFF;

    //timer
    //dsp_buff = START_DSP_BUFFER_POINTER;
}

void read_low_level() {
    unsigned int *p_start   = (unsigned int *) ARM_START_BUFF_P_ADR;

    unsigned char *buff = (unsigned char *) *p_start;

    int buff_size = *(unsigned int *) ARM_BUFF_SIZE;


    // write size
    int pack_size = read_num_by_adr(2, &buff); //*((short *) buff); //PACK_SIZE;

    int readed_bytes = 0;

    for(char i = 0; i < pack_size; ++i) {

        if (buff + i < (unsigned char *) START_ARM_BUFF + buff_size) {
            *(package + i) = *(buff + i);
            readed_bytes++;
        } else {
            *(package + i) = *((unsigned char *) START_ARM_BUFF + i - readed_bytes);
        }

    }


    if (readed_bytes == pack_size) {
        *p_start += pack_size;
    } else {
        *p_start = START_ARM_BUFF + pack_size - readed_bytes;
    }

}

void wait_new() {
//    resetTimeshtamp();
//
//    unsigned int start_time = getTimeshtamp();
//    unsigned int end_time = getTimeshtamp();

//Time delay TODO
//    while (true) {
//        while(end_time - start_time < cycles_wait) {
//            end_time = getTimeshtamp();
//        }
//
//        send_low_level(); //retry_send
//
//    }

    unsigned int *p = (unsigned int *) ARM_END_BUFF_P_ADR;

    while((unsigned int *) *p == last_end_arm_buff) {}

    last_end_arm_buff = (unsigned int *) *p;

    read_low_level();
}


void send_low_level() {
    unsigned int *p_end   = (unsigned int *) DSP_END_BUFF_P_ADR;
       //unsigned char *p_start = (unsigned char *) DSP_START_BUFF_P_ADR;

    unsigned char *buff = (unsigned char *) *p_end;

    int buff_size = *(unsigned int *) DSP_BUFF_SIZE;

    write_num_by_adr(18, 2, &buff);

    int written_bytes = 0;

    for(char i = 0; i < PACK_SIZE; ++i) {

        if (buff + i < (unsigned char *) START_DSP_BUFF + buff_size) {
            *(buff + i) = *(package + PACK_SIZE - i - 1);
            written_bytes++;
        } else {
            *((unsigned char *) START_DSP_BUFF + i - written_bytes) = *(package + PACK_SIZE - i - 1);
        }

    }


    if (written_bytes == PACK_SIZE) {
        *p_end += PACK_SIZE + 2;
    } else {
        *p_end = START_DSP_BUFF + PACK_SIZE - written_bytes + 2;
    }


    wait_new();
}

void net_level() {

    //control sum null for now

    write_num(0x222222, 3);

    //*main_pointer = MARKER_TYPE;
    *main_pointer = 0x21;
    main_pointer++;

    *main_pointer = 0xc1;//CORE_ID;
    main_pointer++;

    *main_pointer = DSP_CORE_ID;
        main_pointer++;


    send_low_level();

    //control sum check, not needed now
}


void send_tarnsp_mess(unsigned char type) {
    *main_pointer = type;
    main_pointer++;

    write_num(0x33333333, 4);  //trusactions_count;

    trusactions_count++;
    net_level();
}

int transp_marker_level() {

    if (current_state != 'A') {
        return -1;
    }

    unsigned char * type_check = main_pointer;
    current_state = 'B';

    send_tarnsp_mess(MARKER_PACKAGE);   // send type 1 pack
    main_pointer = type_check;

    if (*type_check != ANSWER_PACKAGE) { // if answer is not type 2
        //ERROR
        return -1;
    }

    send_tarnsp_mess(END_TRANS_PACKAGE); //send type 3 pack
    main_pointer = type_check;

    current_state = 'C';

    if (*type_check != MARKER_PACKAGE) { // start getting marker
        //ERROR
        return -1;
    }

    current_state = 'D';

    send_tarnsp_mess(ANSWER_PACKAGE); //send type 2 pack

    if (*type_check != END_TRANS_PACKAGE) { // start getting marker
        //ERROR
        return -1;
    }

    current_state = 'A';

    return 1;

}

int rep_level() {

    write_num(0x7777, 2);       //nothing code algorithm

    return transp_marker_level();
}



int give_marker(char task_id) {
     init();

     *main_pointer = task_id; //data
     main_pointer++;

     *main_pointer = 1;       //resiver_number
     main_pointer++;

     *main_pointer = -1;      //result
     main_pointer++;

     write_num(0x1234, 2);    //size

     return rep_level();
}

//void main_amp() {
//    init();
//    give_marker(0x111);
//}
