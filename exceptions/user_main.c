#include <osapi.h>
#include <user_interface.h>
#include <mem.h>
#include <ets_sys.h>
#include "driver/uart.h"

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

static void reboot(void)
{
    uart_flush(0);
    uart_flush(1);
    system_restart();
}

static void handler(struct XTensa_exception_frame_s *p)
{
    struct rst_info info;

    int cause, epc1, epc2, epc3, stack, excvaddr, depc;
    char *cause_name;

    asm volatile("mov %0, a1" : "=r" (stack));
    asm volatile("rsr.exccause %0" : "=r" (cause));
    asm volatile("rsr.epc1 %0" : "=r" (epc1));
    asm volatile("rsr.epc2 %0" : "=r" (epc2));
    asm volatile("rsr.epc3 %0" : "=r" (epc3));
    asm volatile("rsr.excvaddr %0" : "=r" (excvaddr));
    asm volatile("rsr.depc %0" : "=r" (depc));

    system_rtc_mem_read(0, &info, sizeof(struct rst_info));
    if (info.flag != 2) {
        info.flag = 2;
        info.exccause = cause;
        info.epc1 = epc1;
        info.epc2 = epc2;
        info.epc3 = epc3;
        system_rtc_mem_write(0, &info, sizeof(struct rst_info));
    }

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

    reboot();
}

static char *get_reset_reason(struct rst_info *info)
{
    switch(info->flag) {
        case DEFAULT_RST_FLAG: return "RESET";
        case WDT_RST_FLAG: return "WATCHDOG";
        case EXP_RST_FLAG: return "EXCEPTION";
        default: return "UNKNOWN";
    }
}

static void mytask(struct ETSEventTag *e)
{
    struct rst_info info;
    struct rst_info zeroinfo = {0};

    system_rtc_mem_read(0, &info,sizeof(struct rst_info));
    system_rtc_mem_write(0, &zeroinfo,sizeof(struct rst_info));

    if (info.flag < 3) {
        os_printf("\n\n\nReset reason: %d (%s)\n",
                   info.flag, get_reset_reason(&info));
        os_printf("Saved information:\n");
        os_printf("exccause (%d), epc1 = 0x%08x, epc2=0x%08x, epc3=0x%08x,"
                  " excvaddr=0x%08x, depc=0x%08x,\n",
                   info.exccause, info.epc1, info.epc2,
                   info.epc3, info.excvaddr, info.depc);
    } else {
        os_printf("\nUnknown reset flag!\n");
    }

    os_delay_us(1 * 1000 * 1000);

    if (info.flag == DEFAULT_RST_FLAG) {
        os_printf("Reset, now let's make an exception\n");
        asm volatile("mov a0, a1");
        asm volatile("l32i.n a6, a2,0");  // load prohibited
        asm volatile("s32i.n a6, a2,0");  // store prohibited
    } else if(info.flag == EXP_RST_FLAG) {
        os_printf("Exception, now let's kick watchdog\n");
        os_delay_us(10 * 1000 * 1000);
    } else {
        os_printf("Watchog, now let's reboot\n");
        reboot();
    }
}


void ICACHE_FLASH_ATTR user_init(void)
{
    struct ETSEventTag *queue;

    _xtos_set_exception_handler(0, handler);
    _xtos_set_exception_handler(2, handler);
    _xtos_set_exception_handler(3, handler);
    _xtos_set_exception_handler(8, handler);
    _xtos_set_exception_handler(9, handler);
    _xtos_set_exception_handler(28, handler);
    _xtos_set_exception_handler(29, handler);

    uart_init(9600);

    os_printf("\r\n");
    os_printf("\r\nHello\n\n");

    os_delay_us(5 * 1000 * 1000); // Note that we can safly do this here,
                                  // watchdog is not configured yet

    queue = (void *) os_malloc(sizeof(struct ETSEventTag) * 3);
    system_os_task(mytask, 0, queue, 3);
    system_os_post(0, 0, 0);
}
