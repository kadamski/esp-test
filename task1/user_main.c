#include <osapi.h>
#include <os_type.h>
#include <ets_sys.h>
#include <mem.h>

#define MYPRIO 0
#define QUEUELEN 10

static void mytask(struct ETSEventTag *e)
{
    os_printf("Task sig=%d, par=%d\n", e->sig, e->par);
}


void ICACHE_FLASH_ATTR user_init(void)
{
    struct ETSEventTag *queue;

    uart_init(9600);

    os_printf("\n\nHello\n\n");

    queue = (void *) os_malloc(sizeof(struct ETSEventTag) * QUEUELEN);

    system_os_task(mytask, MYPRIO, queue, QUEUELEN);
    system_os_post(MYPRIO, 42, 69);
}
