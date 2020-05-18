#ifndef AMP_H_
#define AMP_H_

#define SYNCH_BLOCK_ARM_BUFF_FLAGS 0x0c000000
#define SYNCH_BLOCK_DSP_BUFF_FLAGS 0x0c300000

typedef union network_package net_p;
typedef union transport_package trans_p;
typedef union representation_package rep_p;
typedef struct app_package app_p;

struct app_package {
    char data;
    char resiver_number; //????!!
    char result;
    char size[2];
}; //


union representation_package {

    struct {
        char coding_algorithm_number[2];
        app_p app_package;
    };


    char raw_data[8];
};

union transport_package {

    struct {
        int transaction_number;
        char type_of_transport;
        rep_p rep_package;
    };

    char raw_data[13];
};


union network_package {

    struct {
        char resiver_number;
        char sender_number;
        char type_of_package;
        char control_sum[3];

        trans_p trans_package;

    };

    char raw_data[19];
};

int give_marker(char task_id);


#endif /* AMP_H_ */
