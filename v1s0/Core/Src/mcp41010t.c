#include "mcp41010t.h"
#include "stm32f4xx_ll_gpio.h"

/* 发送命令 */
uint8_t MCP41010T_SendData(SPI_HandleTypeDef *spi, uint8_t command, uint8_t potent, uint8_t res)
{
    uint8_t data[2];
    HAL_StatusTypeDef ret;

    // 生成发送数据
    data[0] = MCP41010T_COMMAND(command) | MCP41010T_PS(potent);
    data[1] = res;

    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
    ret = HAL_SPI_Transmit(spi, data, 2, 0xffff);
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);

    return ret;
}

/* 设置电阻值 */
uint8_t MCP41010T_SetResistor(SPI_HandleTypeDef *spi, uint8_t res)
{
    // 写第一个电位器的电阻
    return MCP41010T_SendData(spi, MCP41010T_COMMAND_WRITE, MCP41010T_PS_P1, res);
}