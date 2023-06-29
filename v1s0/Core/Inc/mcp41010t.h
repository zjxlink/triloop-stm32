#ifndef __MCP41010T_H
#define __MCP41010T_H

#include <stdint.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal_spi.h"

/* 命令选择位 */
#define MCP41010T_COMMAND_NULL          0x00
#define MCP41010T_COMMAND_WRITE         0x01
#define MCP41010T_COMMAND_SHUTDOWN      0x02
#define MCP41010T_COMMAND_NONE          0x03

/* 电位器选择位 */
#define MCP41010T_PS_NONE               0x00
#define MCP41010T_PS_P1                 0x01
#define MCP41010T_PS_P2                 0x02
#define MCP41010T_PS_P1_P2              0x03

#define MCP41010T_COMMAND(command) (command << 4)
#define MCP41010T_PS(ps) (ps << 0)


/* 设置电阻值 */
extern uint8_t MCP41010T_SetResistor(SPI_HandleTypeDef *spi, uint8_t res);

#endif