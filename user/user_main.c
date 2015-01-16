#include "ets_sys.h"

void ICACHE_FLASH_ATTR user_init(void)
{
    uint32_t heap = system_get_free_heap_size();

    uart_init(9600);

    os_printf("\n\n");
    os_printf("Free heap: %d\n", (int)heap);
}
