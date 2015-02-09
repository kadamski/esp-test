#include <osapi.h>
#include <os_type.h>
#include <ets_sys.h>


static os_timer_t timer;

void timerfunc(void *arg)
{
    static int i = 0;

    os_printf("[%d] Free heap: %d\n", i++, (int) system_get_free_heap_size());
}

void ICACHE_FLASH_ATTR user_init(void)
{
    uart_init(9600);

    os_printf("\n\nHello\n\n");

    os_timer_disarm(&timer);
    os_timer_setfn(&timer, (os_timer_func_t *)timerfunc, NULL);
    os_timer_arm(&timer, 1000, 1);
}
