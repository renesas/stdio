#include <stdio.h>
#include "rs_stdio_uart.h"

void syscalls_picolib_stdio_to_uart_demo(void)
{
    extern fsp_err_t rs_stdio_config_gen(void);

    fsp_err_t err = FSP_SUCCESS;

    err = rs_stdio_config_gen();

    while(FSP_SUCCESS != err);

    err = RS_STDIO_UART_CheckApplicationConfiguration();

    while(FSP_SUCCESS != err);

    printf("\rSystem Init\r\n");

    while (1)
    {
        int c = getchar();
        printf("You typed: %c\r\n", c);
    }
    
}
