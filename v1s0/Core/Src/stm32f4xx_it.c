/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "fsmc2fpga.h"
#include "usbd_cdc_if.h"
#include "sd_data.h"
#include "config.h"
#include "bsp_driver_sd.h"
#include "sensorParse.h"
#include "tempacket.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t USART1_RX_BUFFER[USART_RX_BUFFER_SIZE];
uint8_t USART1_RX_HANDLE[USART_RX_HANDLE_SIZE];
uint16_t USART1_RX_CNT;

uint8_t USART2_RX_BUFFER[USART_RX_BUFFER_SIZE];
uint8_t USART2_RX_HANDLE[USART_RX_HANDLE_SIZE];
uint16_t USART2_RX_CNT;

uint8_t USART3_RX_BUFFER[USART_RX_BUFFER_SIZE];
uint8_t USART3_RX_HANDLE[USART_RX_HANDLE_SIZE];
uint16_t USART3_RX_CNT;

uint8_t UART4_RX_BUFFER[USART_RX_BUFFER_SIZE];
uint8_t UART4_RX_HANDLE[USART_RX_HANDLE_SIZE];
uint16_t UART4_RX_CNT;

uint8_t USART6_RX_BUFFER[USART_RX_BUFFER_SIZE];
uint8_t USART6_RX_HANDLE[USART_RX_HANDLE_SIZE];
uint16_t USART6_RX_CNT;

uint8_t FPGA_MESSAGE_FLAG = 0;
uint32_t FPGA_MESSAGE_ID_NUM = 0;

uint16_t Cnt_1S = 0;
uint32_t Cnt_10S = 0;

extern RTC_HandleTypeDef hrtc;
// rtc GPS对时标志，每次上电仅对时一次
uint8_t RTC_GPS_Update_flag = 0;

// GPS失步标志
uint8_t GPS_Lost_Flag = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void GPSParse(char *str, uint16_t len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern DMA_HandleTypeDef hdma_sdio_rx;
extern DMA_HandleTypeDef hdma_sdio_tx;
extern SD_HandleTypeDef hsd;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

extern uint32_t Timestamp;
extern double Longitude;
extern double Latitude;
extern float Altitude;
extern uint8_t GPSStatus;

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
    while (1)
    {
    }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

    //static uint32_t last_msg = 0;
    Cnt_1S++;
    if(Cnt_1S >= 1000)
    {
        Cnt_1S = 0;
        if(GPS_Lost_Flag == 1)
        {
            RTC_DateTypeDef GetData;  //获取日期结构体
            RTC_TimeTypeDef GetTime;   //获取时间结构体
            HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);
            Timestamp = ((GetTime.Hours+16)%24)*100000 + GetTime.Minutes*1000 + GetTime.Seconds*10;
        }
    }

    Cnt_10S++;
    if(Cnt_10S >= 10000)
    {
        Cnt_10S = 0;

        sd_sync_flag = 1;
    }
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */

  /* USER CODE END EXTI4_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
    /* USER CODE BEGIN LL_EXTI_LINE_4 */
    if(ADC_FPGA_Status == 0)
    {
        return;
    }

    FPGA_INTERRUPT_CNT++;
    /* USER CODE END LL_EXTI_LINE_4 */
  }
  /* USER CODE BEGIN EXTI4_IRQn 1 */

  /* USER CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  */
void DMA1_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */
    if(LL_DMA_IsActiveFlag_TC1(DMA1))
    {
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_1);
        LL_DMA_ClearFlag_TC1(DMA1);
    }
  /* USER CODE END DMA1_Stream1_IRQn 0 */

  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */

  /* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream2 global interrupt.
  */
void DMA1_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream2_IRQn 0 */
    if(LL_DMA_IsActiveFlag_TC2(DMA1))
    {
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_2);
        LL_DMA_ClearFlag_TC2(DMA1);
    }
  /* USER CODE END DMA1_Stream2_IRQn 0 */

  /* USER CODE BEGIN DMA1_Stream2_IRQn 1 */

  /* USER CODE END DMA1_Stream2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */

  /* USER CODE END EXTI9_5_IRQn 0 */
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
    /* USER CODE BEGIN LL_EXTI_LINE_5 */

    /* USER CODE END LL_EXTI_LINE_5 */
  }
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        LL_TIM_ClearFlag_UPDATE(TIM2);

        //处理FPGA数据
        if(FPGA_INTERRUPT_CNT > 1)
        {
            FPGA_SaveToBuffer();
        }
    }

  /* USER CODE END TIM2_IRQn 0 */
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
    uint16_t RX_DATA_LEN;

    if(LL_USART_IsActiveFlag_IDLE(USART3)) {
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_1);
        LL_DMA_ClearFlag_TC1(DMA1);
        LL_USART_ClearFlag_IDLE(USART3);

        RX_DATA_LEN = USART_RX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_1);
        if (USART3_RX_CNT + RX_DATA_LEN < USART_RX_HANDLE_SIZE) {
            memcpy(&USART3_RX_HANDLE[USART3_RX_CNT], USART3_RX_BUFFER, RX_DATA_LEN);
            USART3_RX_CNT += RX_DATA_LEN;

            if (USART3_RX_CNT) {
                USART3_RX_HANDLE[USART3_RX_CNT] = 0;
                GPSParse(USART3_RX_HANDLE, USART3_RX_CNT);

                memset(USART3_RX_HANDLE, 0, USART_RX_HANDLE_SIZE);
                USART3_RX_CNT = 0;
            }
        }

        LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_1, (uint32_t) &USART3->DR);
        LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_1, (uint32_t) USART3_RX_BUFFER);
        LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_1, USART_RX_BUFFER_SIZE);
        LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_1);
    }
  /* USER CODE END USART3_IRQn 0 */
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles SDIO global interrupt.
  */
void SDIO_IRQHandler(void)
{
  /* USER CODE BEGIN SDIO_IRQn 0 */

  /* USER CODE END SDIO_IRQn 0 */
  HAL_SD_IRQHandler(&hsd);
  /* USER CODE BEGIN SDIO_IRQn 1 */

  /* USER CODE END SDIO_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */
    uint16_t RX_DATA_LEN;

    if(LL_USART_IsActiveFlag_IDLE(UART4))
    {
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_2);
        LL_DMA_ClearFlag_TC2(DMA1);
        LL_USART_ClearFlag_IDLE(UART4);

        RX_DATA_LEN = USART_RX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_STREAM_2);
        if(UART4_RX_CNT + RX_DATA_LEN < USART_RX_HANDLE_SIZE)
        {
            memcpy(&UART4_RX_HANDLE[UART4_RX_CNT], UART4_RX_BUFFER, RX_DATA_LEN);
            UART4_RX_CNT += RX_DATA_LEN;

            if (UART4_RX_CNT) {

                SensorParse(SENSOR_TYPE_JY901, UART4_RX_HANDLE, UART4_RX_CNT);
                SensorParse(SENSOR_TYPE_NRA24, UART4_RX_HANDLE, UART4_RX_CNT);

                memset(UART4_RX_HANDLE, 0, USART_RX_HANDLE_SIZE);
                UART4_RX_CNT = 0;
            }
        }

        LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_2, (uint32_t)&UART4->DR);
        LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_2, (uint32_t)UART4_RX_BUFFER);
        LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_2, USART_RX_BUFFER_SIZE);
        LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_2);

    }
  /* USER CODE END UART4_IRQn 0 */
  /* USER CODE BEGIN UART4_IRQn 1 */

  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream1 global interrupt.
  */
void DMA2_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream1_IRQn 0 */
    if(LL_DMA_IsActiveFlag_TC1(DMA2))
    {
        LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_1);
        LL_DMA_ClearFlag_TC1(DMA2);
    }
  /* USER CODE END DMA2_Stream1_IRQn 0 */

  /* USER CODE BEGIN DMA2_Stream1_IRQn 1 */

  /* USER CODE END DMA2_Stream1_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

  /* USER CODE END DMA2_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sdio_rx);
  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

  /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles USB On The Go FS global interrupt.
  */
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */

  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */

  /* USER CODE END OTG_FS_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream6 global interrupt.
  */
void DMA2_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream6_IRQn 0 */

  /* USER CODE END DMA2_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sdio_tx);
  /* USER CODE BEGIN DMA2_Stream6_IRQn 1 */

  /* USER CODE END DMA2_Stream6_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void DMA2_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */

  /* USER CODE END DMA2_Stream7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */

  /* USER CODE END DMA2_Stream7_IRQn 1 */
}

/**
  * @brief This function handles USART6 global interrupt.
  */
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART6_IRQn 0 */
    uint16_t RX_DATA_LEN;

    if(LL_USART_IsActiveFlag_IDLE(USART6)) {
        LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_1);
        LL_DMA_ClearFlag_TC1(DMA2);
        LL_USART_ClearFlag_IDLE(USART6);

        RX_DATA_LEN = USART_RX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA2, LL_DMA_STREAM_1);
        if (USART6_RX_CNT + RX_DATA_LEN < USART_RX_HANDLE_SIZE) {
            memcpy(&USART6_RX_HANDLE[USART6_RX_CNT], USART6_RX_BUFFER, RX_DATA_LEN);
            USART6_RX_CNT += RX_DATA_LEN;

            if (USART6_RX_CNT) {
                TP_Parse(USART6_RX_HANDLE, USART6_RX_CNT);
                USART6_RX_CNT = 0;
            }
        }

        LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_1, (uint32_t) &USART6->DR);
        LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_1, (uint32_t) USART6_RX_BUFFER);
        LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, USART_RX_BUFFER_SIZE);
        LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_1);
    }
  /* USER CODE END USART6_IRQn 0 */
  /* USER CODE BEGIN USART6_IRQn 1 */

  /* USER CODE END USART6_IRQn 1 */
}

/* USER CODE BEGIN 1 */
#if 0
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
#define TY_SDIO_RETRY_MAX 3

    int i;
    for (i = 1; i <= TY_SDIO_RETRY_MAX; i ++)
    {
        if (HAL_SD_WriteBlocks(&hsd, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout) == HAL_OK)
        {
            if (i > 1) printf("[INFO] SDIO writing succeeded: retry %d.\n", i);
            return (MSD_OK); // Succeeded
        }
        else
        {
            printf("[ERROR] SDIO writing failure: retry %d, error code %#x, addr %#x, %u blocks.\n",
                   i, hsd.ErrorCode, WriteAddr, NumOfBlocks);
            HAL_SD_Abort(&hsd); //clear error flag
        }
    }
    return (MSD_ERROR);
}
#endif

void GPSParse(char *str, uint16_t len)
{
    int a=0, b=0;
    float hdop;
    float temp;
    double time, lat, lon;
    float altitude;
    uint8_t gps_status;
    uint8_t satellite_num;
    char head_str[][7] = {"$GAGGA", "$GBGGA", "$GLGGA", "$GNGGA", "$GPGGA"};
    uint32_t timestamp;

    while(1)
    {
        while(str[a] != '$')
        {
            a++;
            if(a >= len) return;
        }

        while(str[b] != '*')
        {
            b++;
            if(b >= len) return;
        }

        if(a < b && (b-a) > 40)
        {
            int i;
            for(i=0; i<5; i++)
            {
                if(strncmp(&str[a], head_str[i], 6) == 0)
                    break;
            }
            if(i<5)
            {
                str[b] = 0;
                sscanf(&str[a+7], "%lf,%lf,N,%lf,E,%hhu,%hhu,%f,%f,M,%f,M,,",
                       &time, &lat, &lon, &gps_status, &satellite_num,
                       &hdop, &altitude, &temp);

                timestamp = ((int)((time+0.05)*10));

                /* GPS时间同步正常 */
                if(timestamp != 0)
                {
                    GPS_Lost_Flag = 0;
                    Timestamp = timestamp;
                }
                else
                {
                    GPS_Lost_Flag = 1;
                }

                /* 更新RTC时间 */
                if(RTC_GPS_Update_flag == 0 && (timestamp%10) == 0 && timestamp != 0) {
                    RTC_GPS_Update_flag = 1;
                    RTC_TimeTypeDef sTime = {0};
                    sTime.Hours =   (((timestamp / 100000 + 8) / 10) << 4) + ((timestamp / 100000 + 8) % 10);
                    sTime.Minutes = (((timestamp / 1000 % 100) / 10) << 4) + ((timestamp / 1000 % 100) % 10);
                    sTime.Seconds = (((timestamp / 10   % 100) / 10) << 4) + ((timestamp / 10   % 100) % 10);
                    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
                    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
                    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
                        printf("Update RTC from GPS failed.\r\n");
                    }
                    else {
                        RTC_DateTypeDef GetData;  //获取日期结构体
                        RTC_TimeTypeDef GetTime;   //获取时间结构体
                        HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
                        HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);
                        printf("Update RTC from GPS success. %04d-%02d-%02d %02d:%02d:%02d\r\n", 2000 + GetData.Year,
                               GetData.Month, GetData.Date,
                               GetTime.Hours, GetTime.Minutes, GetTime.Seconds);
                    }
                }

                Latitude = lat;
                Longitude = lon;
                Altitude = altitude;
                GPSStatus = gps_status;
                //printf("Timestamp=%d, Latitude=%lf, Longitude=%lf, Altitude=%lf\n", Timestamp, Latitude/100, Longitude/100, Altitude);
            }
        }

        a = b;
        b ++;
    }
}
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
