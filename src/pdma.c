#include "base.h"
#include "pdma.h"


inline char pdma_get_control_bit(pdma_chain_t *pdma, enum __pdma_control bit) {
    return (char)((GET_W(PDMA_CONTROL_REG(pdma->base_addr, pdma->chan_id)) >> bit) & 1);
}


void pdma_write_control_bit(pdma_chain_t *pdma, enum __pdma_control bit) {
    __u32 val = GET_W(PDMA_CONTROL_REG(pdma->base_addr, pdma->chan_id));
    __u32 *reg = PDMA_CONTROL_REG(pdma->base_addr, pdma->chan_id);
    *reg = val | (1 << bit);
}


// need to write pdma descriptor's next_conf ctruct to PDMA register
void pdma_write_next_config(pdma_chain_t *pdma) {
    __u32 wr = 
        ((__u32)(pdma->next_conf.repeat & 0x1) << 2)  +
        ((__u32)(pdma->next_conf.order  & 0x1) << 3)  +
        ((__u32)(pdma->next_conf.wsize  & 0x7) << 24) +
        ((__u32)(pdma->next_conf.rsize  & 0x7) << 28) ;
    __u32 *regptr = PDMA_NEXT_CONF_REG(pdma->base_addr, pdma->chan_id);
    *regptr = wr;
}


// need to syncronize next transfer configuration from pdma_chain_t struct with PDMA registers.
// auto write next config
void pdma_sync_next(pdma_chain_t *pdma) {
    pdma_write_next_config(pdma);
    __u64 *nbytes_reg = PDMA_NEXT_BYTES_REG(pdma->base_addr, pdma->chan_id),
          *dest_reg   = PDMA_NEXT_DEST_REG(pdma->base_addr, pdma->chan_id),
          *src_reg    = PDMA_NEXT_SRC_REG(pdma->base_addr, pdma->chan_id);

    *nbytes_reg = pdma->next_nbytes;
    *dest_reg   = (__u64)pdma->next_write_ptr;
    *src_reg    = (__u64)pdma->next_read_ptr;
}


// need to claim PDMA channel. Returns False, if unable to claim for some reason.
char pdma_claim(pdma_chain_t *pdma) {
    if (pdma_get_control_bit(pdma, pdma_control_run)) return 0;
    
    pdma_write_control_bit(pdma, pdma_control_claim);
    return 1;
}

void pdma_wait_transfer(pdma_chain_t *pdma) {
    while (pdma_get_control_bit(pdma, pdma_control_run)) {};
}

// need to run pdma transfer.
// Automatically syncronizes next registers.
// Automatically waits until prewious transfer end.
// assumes, pdma descriptor, intialised corectly.
void pdma_run(pdma_chain_t *pdma) {
    pdma_sync_next(pdma);

    pdma_wait_transfer(pdma);

    pdma_write_control_bit(pdma, pdma_control_run);
}


// TODO Other curr regs read methods
pdma_conf_t pdma_read_curr_config(pdma_chain_t *pdma) {
    __u32 conf_reg = GET_W(PDMA_EXEC_CONF_REG(pdma->base_addr, pdma->chan_id));
    pdma_conf_t res = {
        conf_reg & (1 << 2),
        conf_reg & (1 << 3),
        conf_reg & (7 << 25),  // FIXME or 24 ????
        conf_reg & (7 << 28)
    };
    return res;
}
