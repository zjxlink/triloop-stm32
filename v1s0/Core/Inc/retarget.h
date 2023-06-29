//
// Created by Administrator on 2021/7/5.
//

#ifndef PLC_32DEMO_RETARGET_H
#define PLC_32DEMO_RETARGET_H


#include <sys/stat.h>
#include <stdio.h>

#include "usart.h"

#define UART_HandleTypeDef void
void RetargetInit(UART_HandleTypeDef *uart);

int _isatty(int fd);

int _write(int fd, char *ptr, int len);

int _close(int fd);

int _lseek(int fd, int ptr, int dir);

int _read(int fd, char *ptr, int len);

int _fstat(int fd, struct stat *st);

#endif //PLC_32DEMO_RETARGET_H
