#ifndef PDMA_H
#define PDMA_H

#include "base.h"
#include <stdint.h>

#define __PDMA_CHANNEL_OFFSET 0x8_0000


// rw registers
#define PDMA_CONTROL_REG(base, chan_id)    (base+(0x1000*(chan_id)) + 0x000) // (4) Control Register
#define PDMA_NEXT_CONF_REG(base, chan_id)  (base+(0x1000*(chan_id)) + 0x004) // (4) type to send
#define PDMA_NEXT_BYTES_REG(base, chan_id) (base+(0x1000*(chan_id)) + 0x008) // (8) how many bytes to read
#define PDMA_NEXT_DEST_REG(base, chan_id)  (base+(0x1000*(chan_id)) + 0x010) // (8) addres where to write
#define PDMA_NEXT_SRC_REG(base, chan_id)   (base+(0x1000*(chan_id)) + 0x018) // (8) addres from to read

//ro registers
#define PDMA_CURR_CONF_REG(base, chan_id)  (base+(0x1000*(chan_id)) + 0x104) // (4) actual type
#define PDMA_CURR_BYTES_REG(base, chan_id) (base+(0x1000*(chan_id)) + 0x108) // (8) actual bytes num
#define PDMA_CURR_DEST_REG(base, chan_id)  (base+(0x1000*(chan_id)) + 0x110) // (8) actual write addr
#define PDMA_CURR_SRC_REG(base, chan_id)   (base+(0x1000*(chan_id)) + 0x118) // (8) actual read addr

#define PDMA_FULL_SPEED 0xFF000008


typedef struct __pdma_control {
    uint8_t claim:   1; // 0 bit
    uint8_t run:     1; // 1 bit
    uint16_t :       11;
    uint8_t doneIe:  1; // 13 bit
    uint8_t errorIe: 1; // 14 bit
    uint16_t :       15;   
    uint8_t done:    1; // 30 bit
    uint8_t error:   1; // 31 bit
} pdma_control_t;

typedef struct __pdma_config {
    uint32_t conf;
    uint64_t nbytes;
    uint64_t write_ptr;
    uint64_t read_ptr;
} pdma_conf_t;


typedef struct __pdma_chain_descriptor {
    void *base_addr;
    int chan_id;

    pdma_control_t control;
    pdma_conf_t next_config;    
    pdma_conf_t curr_config;    
} pdma_chann_t;

pdma_chann_t pdma_init(void *base_addr, int chan_id);

// need to claim PDMA channel. Returns False, if unable to claim for some reason.
char pdma_claim(pdma_chann_t *pdma);

// need to free (unclaim) PDMA channel.
// Waits until transfer end, if has runing one
void pdma_unclaim(pdma_chann_t *pdma);



// refreshes pdma->control
void pdma_control_get(pdma_chann_t *pdma);

// writes pdma->control to PDMA control register
void pdma_control_write(pdma_chann_t *pdma);



// refreshes pdma->next_config
void pdma_config_get_next(pdma_chann_t *pdma);

// refreshes pdma->curr_config
void pdma_config_get_curr(pdma_chann_t *pdma);

// writes pdma->next_config to PDMA Next Config register
void pdma_config_write_next(pdma_chann_t *pdma);



// waits until transfer complete
void pdma_wait_transfer(pdma_chann_t *pdma);

// need to run pdma transfer.
// Automatically syncronizes next registers.
// Automatically waits until prewious transfer end.
// assumes, pdma descriptor, intialised corectly.
void pdma_run(pdma_chann_t *pdma);

#endif