#include <osapi.h>
#include "ets_sys.h"

int *i = 0;

struct XTensa_exception_frame_s {
    uint32_t xt_pc;      // Program Counter Register
    uint32_t xt_ps;      // Program Status Register
    uint32_t xt_sar;     // Shift Amount Register
    uint32_t xt_wtf;     // ??
    uint32_t xt_a0;
    uint32_t xt_a2;
    uint32_t xt_a3;
    uint32_t xt_a4;
    uint32_t xt_a5;
    uint32_t xt_a6;
    uint32_t xt_a7;
    uint32_t xt_a8;
};

void handler(struct XTensa_exception_frame_s *p)
{
    int cause, epc1, stack;
    char *cause_name;

    asm volatile("mov %0, a1" : "=r" (stack));
    asm volatile("rsr.exccause %0" : "=r" (cause));

    switch(cause) { // page 89 (114)f
        case 0: cause_name = "illegal"; break;
        case 2: cause_name = "instruction fetch"; break;
        case 3: cause_name = "load store error"; break;
        case 8: cause_name = "privileged"; break;
        case 9: cause_name = "unaligned"; break;
        case 28: cause_name = "load prohibited"; break;
        case 29: cause_name = "store prohibited"; break;
    }

    os_printf("Fatal exception: %d (%s)\n", cause, cause_name);
    os_printf("PC: 0x%08x, PS: 0x%08x, SAR: 0x%08x, ST: 0x%08x\n", p->xt_pc,
              p->xt_ps, p->xt_sar, stack);
    os_printf("A0: 0x%08x, A2: 0x%08x,  A3: 0x%08x, A4: 0x%08x\n",
              p->xt_a0, p->xt_a2, p->xt_a3, p->xt_a4);
    os_printf("A5: 0x%08x, A6: 0x%08x,  A7: 1x%08x, A8: 0x%08x\n",
              p->xt_a5, p->xt_a6, p->xt_a7, p->xt_a8);
    for(;;);
}

void ICACHE_FLASH_ATTR user_init(void)
{
    uint32_t heap = system_get_free_heap_size();
    int t;

    _xtos_set_exception_handler(0, handler);
    _xtos_set_exception_handler(2, handler);
    _xtos_set_exception_handler(3, handler);
    _xtos_set_exception_handler(8, handler);
    _xtos_set_exception_handler(9, handler);
    _xtos_set_exception_handler(28, handler);
    _xtos_set_exception_handler(29, handler);

    uart_init(9600);

    os_printf("\n\nHello\n\n");

    asm volatile("mov a0, a1");
    // asm volatile("movi.n a2, 0x10");
    // asm volatile("movi.n a3, 0x13");
    // asm volatile("movi.n a4, 0x14");
    // asm volatile("movi.n a5, 0x15");
    // asm volatile("movi.n a6, 0x16");
    // asm volatile("movi.n a7, 0x17");
    // asm volatile("movi.n a8, 0x18");
    // asm volatile("wsr.sar a8");
    asm volatile("l32i.n a6, a2,0");  // load prohibited
    asm volatile("s32i.n a6, a2,0");  // store prohibited
}
