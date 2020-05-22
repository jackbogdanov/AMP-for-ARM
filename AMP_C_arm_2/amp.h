#ifndef AMP_H_
#define AMP_H_

#define CORE_ID     0x01
#define DSP_CORE_ID 0xC1

typedef struct marker_struct marker;

struct marker_struct {
    char to;
    short size;
    char fun_num;
    char result;
};

void give_marker(marker *);

#endif /* AMP_H_ */
