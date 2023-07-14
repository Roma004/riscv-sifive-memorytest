#include "base.h"

#define __PDMA_REGS_OFFSET 0x80000


// rw registers
#define PDMA_CONTROL_REG(base, chan_id)    (base+__PDMA_REGS_OFFSET+(0x1000*(chan_id)) + 0x000) // (4) Control Register
#define PDMA_NEXT_CONF_REG(base, chan_id)  (base+__PDMA_REGS_OFFSET+(0x1000*(chan_id)) + 0x004) // (4) type to send
#define PDMA_NEXT_BYTES_REG(base, chan_id) (base+__PDMA_REGS_OFFSET+(0x1000*(chan_id)) + 0x008) // (8) how many bytes to read
#define PDMA_NEXT_DEST_REG(base, chan_id)  (base+__PDMA_REGS_OFFSET+(0x1000*(chan_id)) + 0x010) // (8) addres where to write
#define PDMA_NEXT_SRC_REG(base, chan_id)   (base+__PDMA_REGS_OFFSET+(0x1000*(chan_id)) + 0x018) // (8) addres from to read

//ro registers
#define PDMA_EXEC_CONF_REG(base, chan_id)  (base+__PDMA_REGS_OFFSET+(0x1000*(chan_id)) + 0x104) // (4) actual type
#define PDMA_EXEC_BYTES_REG(base, chan_id) (base+__PDMA_REGS_OFFSET+(0x1000*(chan_id)) + 0x108) // (8) actual bytes num
#define PDMA_EXEC_DEST_REG(base, chan_id)  (base+__PDMA_REGS_OFFSET+(0x1000*(chan_id)) + 0x110) // (8) actual write addr
#define PDMA_EXEC_SRC_REG(base, chan_id)   (base+__PDMA_REGS_OFFSET+(0x1000*(chan_id)) + 0x118) // (8) actual read addr


// control bits mapping
enum __pdma_control {
    pdma_control_claim   = 0,
    pdma_control_run     = 1,
    pdma_control_doneIe  = 13,
    pdma_control_errorIe = 14,
    pdma_control_done    = 30,
    pdma_control_error   = 31
};


typedef struct __pdma_config {
    char repeat;
    char order;
    char wsize;
    char rsize;
} pdma_conf_t;


typedef struct __pdma_chain_descriptor {
    void *base_addr;
    int chan_id;

    pdma_conf_t next_conf;
    uint64_t next_nbytes;
    void *next_write_ptr;
    void *next_read_ptr;
} pdma_chain_t;



char pdma_get_control_bit(pdma_chain_t *pdma, enum __pdma_control bit);

void pdma_write_control_bit(pdma_chain_t *pdma, enum __pdma_control bit);

// need to write pdma descriptor's next_conf ctruct to PDMA register
void pdma_write_next_config(pdma_chain_t *pdma);

// need to syncronize next transfer configuration from pdma_chain_t struct with PDMA registers.
// auto write next config
void pdma_sync_next(pdma_chain_t *pdma);


// need to claim PDMA channel. Returns False, if unable to claim for some reason.
char pdma_claim(pdma_chain_t *pdma);


// waits until transfer complete
void pdma_wait_transfer(pdma_chain_t *pdma);


// need to run pdma transfer.
// Automatically syncronizes next registers.
// Automatically waits until prewious transfer end.
// assumes, pdma descriptor, intialised corectly.
void pdma_run(pdma_chain_t *pdma);