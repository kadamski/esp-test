#include <osapi.h>
#include <os_type.h>
#include <ets_sys.h>
#include <gpio.h>

static os_timer_t timer;
static const int led = 12;

void timerfunc(void *arg)
{
    static bool s = 0;
    s = !s;
    GPIO_OUTPUT_SET(led, s);
}

void ICACHE_FLASH_ATTR user_init(void)
{
    uart_init(9600);
    gpio_init();
    system_timer_reinit();

    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    GPIO_OUTPUT_SET(led, 0);

    os_printf("\n\nHello\n\n");

    os_timer_disarm(&timer);
    os_timer_setfn(&timer, (os_timer_func_t *)timerfunc, NULL);
    os_timer_arm(&timer, 1000, 1);
}
