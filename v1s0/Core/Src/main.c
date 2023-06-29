/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "fatfs.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fsmc2fpga.h"
#include "usbd_cdc_if.h"
#include "tool.h"
#include "config.h"
#include "sd_data.h"
#include "retarget.h"
#include "tempacket.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Bank1_SRAM1_ADDR    ((uint32_t)(0x60000000))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t cnt;

/* 控制串口发送速率 */
uint32_t Monitor_Send_Tick = 0;
uint32_t Monitor_Send_Delay = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_FSMC_Init();
    MX_USB_DEVICE_Init();
    MX_USART1_UART_Init();
    MX_RTC_Init();
    MX_SDIO_SD_Init();
    MX_SPI1_Init();
    MX_SPI2_Init();
    MX_TIM1_Init();
    MX_UART4_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_USART6_UART_Init();
    MX_FATFS_Init();
    MX_TIM2_Init();
    /* USER CODE BEGIN 2 */

    RetargetInit(&huart1);

    FPGA_Reset();

    //TP_FPGA_Start();
    //LL_mDelay(100);
    //TP_FPGA_Stop();

    SD_DATA_Test();

    printf("Begin Main Task...\r\n");

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    uint32_t send_index = 0;

    while (1) {

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        uint8_t result;
        uint8_t *buffer = FPGA_GetMonitorBuffer();
        Monitor_Send_Delay = 88;
        if (buffer) {
            if (FPGA_SPLITFRAME_FLAG == 1)
            {
                int len = FPGA_GetMonitorBufferLength();
                int s = 128;
                uint8_t *pData;
                int slen;

                while(1) {
                    // 判断是否需要结束分帧传输
                    if(frameStopFlag == 1)
                    {
                        frameStopFlag = 0;
                        frameReqFlag = 0;
                        frameReqNo = 0;
                        FPGA_SPLITFRAME_FLAG = 0;
                        FPGA_ClearMonitorBuffer();
                        break;
                    }

                    // 等待请求分帧数据
                    if (frameReqFlag == 1) {
                        frameReqFlag = 0;

                        // 生成分帧传输帧
                        int k = frameReqNo;

                        slen = len-k*s;
                        // 最后一帧
                        if(slen <= s) {
                            pData = TP_GeneSplitFrameAck((uint8_t *) &buffer[k * s], slen, k, 1);
                        }
                        else {
                            slen = s;
                            pData = TP_GeneSplitFrameAck((uint8_t *) &buffer[k * s], slen, k, 0);
                        }

                        My_USART_Send(USART6, pData, slen + 14);
                        //LL_mDelay(Monitor_Send_Delay);
                    }
                    else
                    {
                        LL_mDelay(1);
                    }
                }
            }
            else {
                // 开始发送
                uint32_t send_cnt = 0;
                //数据
                int len = FPGA_GetMonitorBufferLength();
                int s = 128;
                int m = len / s;
                int n = len % s;

                for (int k = 0; k < m; k++) {
                    //停止状态 退出发送状态
                    if (0 == ADC_FPGA_Status) break;
                    // 发送数据到USB
                    if (ADC_Sample_DataChannel == ADC_SAMPLE_DATA_CHANNEL_USB) {
                        do {
                            result = CDC_Transmit_FS((uint8_t *) &buffer[k * s], s);
                            LL_mDelay(1);
                        } while (result == USBD_BUSY);
                    } else {
                        My_USART_Send(USART6, (uint8_t *) &buffer[k * s], s);
                        LL_mDelay(Monitor_Send_Delay);
                    }
                    send_cnt += s;

                }
                if (n) {
                    //停止状态 退出发送状态
                    if (0 == ADC_FPGA_Status) continue;

                    // 发送数据到USB
                    if (ADC_Sample_DataChannel == ADC_SAMPLE_DATA_CHANNEL_USB) {
                        do {
                            result = CDC_Transmit_FS((uint8_t *) &buffer[m * s], n);
                            LL_mDelay(1);
                        } while (result == USBD_BUSY);
                    } else {
                        My_USART_Send(USART6, (uint8_t *) &buffer[m * s], n);
                        LL_mDelay(Monitor_Send_Delay);
                    }
                    send_cnt += s;
                }
                FPGA_ClearMonitorBuffer();
            }
        }
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // 先将时钟源选择为内部时钟
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    // 禁用内部高速时钟
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
        LL_mDelay(1000);
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
