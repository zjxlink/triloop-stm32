#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdint.h>

#include "retarget.h"
#include "usbd_cdc_if.h"


#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define USE_USART_HAL

UART_HandleTypeDef *gUart = 0;

static void TransmissionT_Data(uint8_t *p_data,uint32_t size)
{
#ifdef USE_USART_HAL
    HAL_UART_Transmit( gUart, p_data, size, 0xffff);
    //HAL_UART_Transmit(gUart, p_data, size, 0xffff);
#else
    while(size)
    {
        LL_USART_TransmitData8(gUart,*p_data);
        while(!LL_USART_IsActiveFlag_TXE(gUart))
        {

        }

        size--;
        p_data++;
    }
#endif
}

static inline void USART_Send(uint8_t *ptr, int len)
{
    //if(!gUart)
    {
        //CDC_Transmit_FS(ptr, len);
    }
    //else
    {
        TransmissionT_Data(ptr, len);
    }
}

static inline int USART_Recv(uint8_t *ptr, int len)
{
    if(!gUart)
    {
        ;
    }
    else {
//        HAL_UART_Receive(gUart, ptr, len, 0xffff);
    }
    return len;
}

void RetargetInit(UART_HandleTypeDef *uart)
{
    gUart = uart;

    /* Disable I/O buffering for STDOUT stream, so that
     * chars are sent out as soon as they are printed. */
    setvbuf(stdout, NULL, _IONBF, 0);
}

int _isatty(int fd)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        return 1;

    errno = EBADF;
    return 0;
}

int _write(int fd, char *ptr, int len)
{
    int i;

    if (fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        USART_Send(ptr, len);
        return len;
    }
    return -1;
}

int _close(int fd)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        return 0;

    errno = EBADF;
    return -1;
}

int _lseek(int fd, int ptr, int dir)
{
    (void) fd;
    (void) ptr;
    (void) dir;

    errno = EBADF;
    return -1;
}

int _read(int fd, char *ptr, int len)
{
    int i;

    if (fd == STDIN_FILENO)
    {
        USART_Recv(ptr, len);
        return len;
    }
    return -1;
}
int _fstat(int fd, struct stat *st)
{
    if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
    {
        st->st_mode = S_IFCHR;
        return 0;
    }

    errno = EBADF;
    return 0;
}
