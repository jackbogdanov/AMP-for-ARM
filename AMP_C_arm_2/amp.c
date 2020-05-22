#include "amp.h"
#include "utils/lock.h"
#include "utils/clock.h"

// functions number for available on DSP
#define FUN_NUMBER 0;

#define NET_HEADER_SIZE     7
#define TRANSP_HEADER_SIZE  5
#define REP_HEADER_SIZE     2


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
#define DSP_START_FLAG          0x0c00000c
#define DSP_BUFF_SIZE           0x0c180000
#define START_DSP_BUFF          0x0c180024

#define MAX_PACK_SIZE 255


unsigned char package[MAX_PACK_SIZE];
unsigned char *main_pointer;

int current_pack_size;
char current_state;
int trusactions_count;
char destination;


unsigned int *last_end_arm_buff;

void wait_new();

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

    if (bytes_count == 1) {
        **buff = (unsigned char) num;
        *buff += 1;
        return;
    }

    for(int i = bytes_count - 1; i >= 0; --i) {
        int mask = 0x000000FF;

        mask = mask << i*8;

        **buff = (num & mask) >> i*8;
        *buff += 1;
    }
}

void write_num(unsigned int num, int bytes_count) {
    write_num_by_adr(num, bytes_count, &main_pointer);
}

void init(char to) {
    current_state = 'A';
    main_pointer = (unsigned char *) &package;
    destination = to;
    trusactions_count = 0;

// ------- for testing without DSP program -----------------------

//    int *p1 = (int *) DSP_END_BUFF_P_ADR;
//    *p1 = 0x0c18011F;
//    p1 = (int *) DSP_BUFF_SIZE;
//    *p1 = 0x00000100;

// ---------------------------------------------------------------

    last_end_arm_buff = (unsigned int *) START_ARM_BUFF;

    int *p1 = (int *) ARM_BUFF_SIZE;
    *p1 = 0x00000100;

    p1 = (int *) ARM_START_BUFF_P_ADR;
    *p1 = START_ARM_BUFF;
    p1 = (int *) ARM_END_BUFF_P_ADR;
    *p1 = START_ARM_BUFF;

    current_pack_size = 17;

    unsigned int *p_dsp_start = (unsigned int *) DSP_START_FLAG;

    *p_dsp_start = 1;
}

void net_level_read() {

    unsigned char dest = *(main_pointer + current_pack_size - 1);
    unsigned char from = *(main_pointer + current_pack_size - 2);
    unsigned int control_sum = *(main_pointer + current_pack_size - 3);

    if (dest != CORE_ID || from != destination || control_sum != 0) {
        wait_new();
        return;
    }
}


void read_low_level() {
    unsigned int *p_start   = (unsigned int *) ARM_START_BUFF_P_ADR;

    unsigned char *buff = (unsigned char *) *p_start;

    int buff_size = *(unsigned int *) ARM_BUFF_SIZE;
    int pack_size = read_num_by_adr(2, &buff);

    int readed_bytes = 0;

    for(char i = 0; i < pack_size; ++i) {

        if (buff + i < (unsigned char *) START_ARM_BUFF + buff_size) {
            *(package + pack_size - i - 1) = *(buff + i);
            readed_bytes++;
        } else {
            *(package + pack_size - i - 1) = *((unsigned char *) START_ARM_BUFF + i - readed_bytes);
        }

    }


    if (readed_bytes == pack_size) {
        *p_start += pack_size + 2;
    } else {
        *p_start = START_ARM_BUFF + pack_size + 2 - readed_bytes;
    }

    current_pack_size = pack_size;
    last_end_arm_buff = (unsigned int *) *p_start;

    main_pointer = (unsigned char *) &package;

    net_level_read();
}

void send_low_level() {
    unsigned int *p_end = (unsigned int *) DSP_END_BUFF_P_ADR;

    unsigned char *buff = (unsigned char *) *p_end;

    int buff_size = *(unsigned int *) DSP_BUFF_SIZE;

    write_num_by_adr(current_pack_size, 2, &buff);

    int written_bytes = 0;

    for(char i = 0; i < current_pack_size; ++i) {

        if (buff + i < (unsigned char *) START_DSP_BUFF + buff_size) {
            *(buff + i) = *(package + current_pack_size - i - 1);
            written_bytes++;
        } else {
            *((unsigned char *) START_DSP_BUFF + i - written_bytes) = *(package + current_pack_size - i - 1);
        }

    }


    if (written_bytes == current_pack_size) {
        *p_end += current_pack_size + 2;
    } else {
        *p_end = START_DSP_BUFF + current_pack_size - written_bytes + 2;
    }
}


void wait_new() {
    resetTimeshtamp();
    unsigned long long start_time = getTimeshtamp();

    unsigned int *p = (unsigned int *) ARM_END_BUFF_P_ADR;

    while((unsigned int *) *p == last_end_arm_buff) {
        if (getTimeshtamp() - start_time > WAIT) {
            send_low_level();
            resetTimeshtamp();
            start_time = getTimeshtamp();
        }
    }

    last_end_arm_buff = (unsigned int *) *p;

    read_low_level();
}



void net_level_send() {

    write_num(0, 3);            //control sum null for now

    write_num(CORE_ID, 1);      //from field

    write_num(destination, 1);  //destination field


    send_low_level();
}

void send_tarnsp_mess(unsigned char type) {

    write_num(trusactions_count, 4);    //trusactions_count;
    write_num(type, 1);                 //type of transport packet

    net_level_send();

    wait_new();

    main_pointer = main_pointer + current_pack_size - NET_HEADER_SIZE - 3;
}

int handle_state_B(unsigned char type) {

    switch(type) {
        case ANSWER_PACKAGE:
            send_tarnsp_mess(END_TRANS_PACKAGE);
            current_state = 'C';
            break;

        case MARKER_PACKAGE:
        case END_TRANS_PACKAGE:
            send_tarnsp_mess(ERROR_PACKAGE);
            break;

        case ERROR_PACKAGE:
            send_tarnsp_mess(ANSWER_PACKAGE);
            break;

        default:
            return -1;
    }

    return 1;
}

int handle_state_C(unsigned char type) {

    switch(type) {
        case MARKER_PACKAGE:
            send_tarnsp_mess(ANSWER_PACKAGE);
            current_state = 'D';
            break;

        case ANSWER_PACKAGE:
            send_tarnsp_mess(END_TRANS_PACKAGE);
            break;
        case END_TRANS_PACKAGE:
            wait_new();
            break;

        case ERROR_PACKAGE:
            current_state = 'B';
            break;

        default:
            return -1;
    }

    return 1;
}

int handle_state_D(unsigned char type) {

    switch(type) {
        case MARKER_PACKAGE:
            send_tarnsp_mess(ANSWER_PACKAGE);
            break;

        case ANSWER_PACKAGE:
        case ERROR_PACKAGE:
            send_tarnsp_mess(ERROR_PACKAGE);
            break;

        case END_TRANS_PACKAGE:
            current_state = 'A';
            break;

        default:
            return -1;
    }

    return 1;
}

int transp_marker_level() {

    if (current_state != 'A') {
        return -1;
    }

    trusactions_count++;

    int err = 1;

    do {
        switch(current_state) {
        case 'A':
            current_state = 'B';
            send_tarnsp_mess(MARKER_PACKAGE);
            break;
        case 'B':
            err = handle_state_B(*(main_pointer + 4));
            break;
        case 'C':
            err = handle_state_C(*(main_pointer + 4));
            break;
        case 'D':
            err = handle_state_D(*(main_pointer + 4));
            break;
        }

        if (err == -1) {
            return -1;
        }

    } while (current_state != 'A');

    return 1;
}

int rep_level() {

    write_num(0, 2);            //nothing code algorithm

    return transp_marker_level();
}



void give_marker(marker * m) {
     init(m->to);

     write_num(m->fun_num, 1);   //data
     write_num(m->to, 1);        //resiver_number
     write_num(-1, 1);           //result
     write_num(m->size, 2);      //size

     m->result = rep_level();
}

