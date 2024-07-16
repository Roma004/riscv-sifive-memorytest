#include "base.h"
#include "pdma.h"
#include <stddef.h>
#include <stdint.h>


pdma_chann_t pdma_init(void *base_addr, int chan_id) {
    pdma_chann_t res = {
        .base_addr = base_addr,
        .chan_id   = chan_id
    };

    pdma_control_get(&res);
    pdma_config_get_curr(&res);
    pdma_config_get_next(&res);

    return res;
}


#pragma GCC push_options
#pragma GCC optimize ("O0")

void pdma_control_get(pdma_chann_t *pdma) {
    volatile uint32_t *control_reg = PDMA_CONTROL_REG(pdma->base_addr, pdma->chan_id);
    GET_W(&pdma->control) = *control_reg;
}

void pdma_control_write(pdma_chann_t *pdma) {
    volatile uint32_t *control_reg = PDMA_CONTROL_REG(pdma->base_addr, pdma->chan_id);
    *control_reg = GET_W(&pdma->control);
}

void pdma_config_get_next(pdma_chann_t *pdma) {
    volatile pdma_conf_t *conf_reg = PDMA_NEXT_CONF_REG(pdma->base_addr, pdma->chan_id);
    pdma->next_config = *conf_reg;

    volatile uint32_t *next_conf_reg  = PDMA_NEXT_CONF_REG(pdma->base_addr, pdma->chan_id);
    volatile uint64_t *next_bytes_reg = PDMA_NEXT_BYTES_REG(pdma->base_addr, pdma->chan_id),
                      *next_dest_reg  = PDMA_NEXT_DEST_REG(pdma->base_addr, pdma->chan_id),
                      *next_src_reg   = PDMA_NEXT_SRC_REG(pdma->base_addr, pdma->chan_id);
    pdma->next_config.conf      = *next_conf_reg;
    pdma->next_config.nbytes    = *next_bytes_reg;
    pdma->next_config.write_ptr = *next_dest_reg;
    pdma->next_config.read_ptr  = *next_src_reg;
}

void pdma_config_get_curr(pdma_chann_t *pdma) {
    volatile uint32_t *curr_conf_reg  = PDMA_CURR_CONF_REG(pdma->base_addr, pdma->chan_id);
    volatile uint64_t *curr_bytes_reg = PDMA_CURR_BYTES_REG(pdma->base_addr, pdma->chan_id),
                      *curr_dest_reg  = PDMA_CURR_DEST_REG(pdma->base_addr, pdma->chan_id),
                      *curr_src_reg   = PDMA_CURR_SRC_REG(pdma->base_addr, pdma->chan_id);
    pdma->curr_config.conf      = *curr_conf_reg;
    pdma->curr_config.nbytes    = *curr_bytes_reg;
    pdma->curr_config.write_ptr = *curr_dest_reg;
    pdma->curr_config.read_ptr  = *curr_src_reg;
}


void pdma_config_write_next(pdma_chann_t *pdma) {
    volatile uint32_t *next_conf_reg  = PDMA_NEXT_CONF_REG(pdma->base_addr, pdma->chan_id);
    volatile uint64_t *next_bytes_reg = PDMA_NEXT_BYTES_REG(pdma->base_addr, pdma->chan_id),
                      *next_dest_reg  = PDMA_NEXT_DEST_REG(pdma->base_addr, pdma->chan_id),
                      *next_src_reg   = PDMA_NEXT_SRC_REG(pdma->base_addr, pdma->chan_id);
    *next_conf_reg  = pdma->next_config.conf;
    *next_bytes_reg = pdma->next_config.nbytes;
    *next_dest_reg  = pdma->next_config.write_ptr;
    *next_src_reg   = pdma->next_config.read_ptr;
}

#pragma GCC pop_options


char pdma_claim(pdma_chann_t *pdma) {
    pdma_control_get(pdma); // refresh current control register state
    if (pdma->control.run)
        return 0;

    pdma->control.claim = 1;
    pdma_control_write(pdma);

    return 1;
}

void pdma_unclaim(pdma_chann_t *pdma) {
    pdma_wait_transfer(pdma);

    pdma->control.claim = 0;
    pdma_control_write(pdma);
}


void pdma_wait_transfer(pdma_chann_t *pdma) {
    while (1) {
        pdma_control_get(pdma);
        if (!pdma->control.run) break;
    }
}

// need to run pdma transfer.
// Automatically syncronizes next registers.
// Automatically waits until prewious transfer end.
// assumes, pdma descriptor, intialised corectly.
void pdma_run(pdma_chann_t *pdma) {
    pdma_config_write_next(pdma);

    pdma_wait_transfer(pdma);

    pdma->control.run = 1;
    pdma_control_write(pdma);

    pdma_wait_transfer(pdma);

    pdma_config_get_curr(pdma);
}

