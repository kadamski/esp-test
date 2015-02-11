#include "ets_sys.h"
#include "osapi.h"
#include "driver/uart.h"
#include "osapi.h"
#include "driver/uart_register.h"

static void ICACHE_FLASH_ATTR uart_config(int baud)
{
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_U0RTS);

    uart_div_modify(UART0, UART_CLK_FREQ/baud);

    WRITE_PERI_REG(UART_CONF0(UART0), STICK_PARITY_DIS | \
        (ONE_STOP_BIT << UART_STOP_BIT_NUM_S) | \
        (EIGHT_BITS << UART_BIT_NUM_S));

    //clear rx and tx fifo,not ready
    SET_PERI_REG_MASK(UART_CONF0(UART0), UART_RXFIFO_RST|UART_TXFIFO_RST);
    CLEAR_PERI_REG_MASK(UART_CONF0(UART0), UART_RXFIFO_RST|UART_TXFIFO_RST);
}

static void uart_tx(int uart, char c)
{
    uint32 fifo_cnt;

    while (true) {
        fifo_cnt = READ_PERI_REG(UART_STATUS(uart)) & \
            (UART_TXFIFO_CNT << UART_TXFIFO_CNT_S);

        if ((fifo_cnt >> UART_TXFIFO_CNT_S & UART_TXFIFO_CNT) < 126) {
            break;
        }
    }

    WRITE_PERI_REG(UART_FIFO(uart), c);
}

static void ICACHE_FLASH_ATTR uart_putc(int uart, char c)
{
    if (c == '\n') {
        uart_tx(uart, '\r');
    }
    else if (c == '\r') {
        return;
    }

    uart_tx(uart, c);
}

static void ICACHE_FLASH_ATTR uart0_putc(char c)
{
    uart_putc(UART0, c);
}

void ICACHE_FLASH_ATTR uart_init(int baud)
{
    uart_config(baud);
    os_install_putc1(uart0_putc);
}

void uart_flush(int n)
{
    int mask = UART_TXFIFO_CNT << UART_TXFIFO_CNT_S;

    while ((READ_PERI_REG(UART_STATUS(n)) & mask) != 0)
        ;

}
