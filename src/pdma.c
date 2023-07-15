#include "base.h"
#include "pdma.h"
#include <stdint.h>


inline char pdma_get_control_bit(pdma_chain_t *pdma, enum __pdma_control bit) {
    return (char)((GET_W(PDMA_CONTROL_REG(pdma->base_addr, pdma->chan_id)) >> bit) & 1);
}


void pdma_write_control_bit(pdma_chain_t *pdma, enum __pdma_control bit) {
    uint32_t val = GET_W(PDMA_CONTROL_REG(pdma->base_addr, pdma->chan_id));
    volatile uint32_t *reg = PDMA_CONTROL_REG(pdma->base_addr, pdma->chan_id);
    *reg = val | (1 << bit);
}


// need to syncronize next transfer configuration from pdma_chain_t struct with PDMA registers.
// auto write next config
void pdma_sync_next(pdma_chain_t *pdma) {
    volatile uint32_t *config = PDMA_NEXT_CONF_REG(pdma->base_addr, pdma->chan_id);
    volatile uint64_t *nbytes_reg = PDMA_NEXT_BYTES_REG(pdma->base_addr, pdma->chan_id),
          *dest_reg   = PDMA_NEXT_DEST_REG(pdma->base_addr, pdma->chan_id),
          *src_reg    = PDMA_NEXT_SRC_REG(pdma->base_addr, pdma->chan_id);

    *config     = pdma->next_conf;
    *nbytes_reg = pdma->next_nbytes;
    *dest_reg   = (uint64_t)(size_t)pdma->next_write_ptr;
    *src_reg    = (uint64_t)(size_t)pdma->next_read_ptr;
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
    uint32_t conf_reg = GET_W(PDMA_EXEC_CONF_REG(pdma->base_addr, pdma->chan_id));
    pdma_conf_t res = {
        conf_reg & (1 << 2),
        conf_reg & (1 << 3),
        conf_reg & (7 << 25),  // FIXME or 24 ????
        conf_reg & (7 << 28)
    };
    return res;
}
