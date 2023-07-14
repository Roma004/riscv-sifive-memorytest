#ifndef BASE_H
#define BASE_H


typedef unsigned long size_t;

typedef volatile unsigned int       __u16;  // sifive word type
typedef volatile unsigned long      __u32;  // sifive dword type
typedef volatile unsigned long long __u64;  // sifive qword type
typedef volatile unsigned char      __u8;   // sifive byte type

#define GET_B(addr) (*(__u8 *)(addr))
#define GET_W(addr) (*(__u32 *)(addr))
#define GET_DW(addr) (*(__u64 *)(addr))

typedef enum fault_t {
	ok = 0,
	size_too_small,
	tx_not_empty,
	data_not_ready
} fault_t;

// defined in LD script
extern volatile unsigned char *__uart_base_addr;
extern volatile unsigned char *__pdma_base_addr;

#define UART0_BASE_ADDR &__uart_base_addr
#define PDMA_BASE_ADDR &__pdma_base_addr


#endif