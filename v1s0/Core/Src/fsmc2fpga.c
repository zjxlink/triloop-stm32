#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f407xx.h"
#include "fsmc2fpga.h"
#include "tempacket.h"
#include "config.h"
#include "usbd_cdc_if.h"
#include "sd_data.h"

// 当前数据ID
uint32_t FPGA_MESSAGE_ID = 0;
// 接收中断次数
int32_t FPGA_INTERRUPT_CNT = 0;

//内部缓存
uint8_t FPGA_BUFFER[FPGA_BUFFER_SIZE];
//监控数据缓存
uint8_t FPGA_MONITOR_BUFFER[FPGA_MONITOR_BUFFER_SIZE];
// 分帧回传标志
uint8_t FPGA_SPLITFRAME_FLAG = 0;
// 单次采集标志
uint8_t FPGA_ONESAMPLE_FLAG = 0;
// 单次采集计数器
uint8_t FPGA_ONESAMPLE_CNT = 0;

//
uint8_t MONITOR_FLAG;

// 叠加数据缓存
#ifdef USE_COMP_SOURCE
CCMRAM int32_t FPGA_ACC_BUFFER[SOURCE_NUM][FPGA_ACC_BUFFER_POINT_SIZE*FPGA_ADC_CHANNEL_NUM] = {0};
#else
CCMRAM int32_t FPGA_ACC_BUFFER[SOURCE_NUM][FPGA_BUFFER_POINT_SIZE*FPGA_ADC_CHANNEL_NUM] = {0};
#endif

// 叠加计数器
uint16_t FPGA_ACC_CNT = 0;

uint8_t FPGA_MESSAGE_SUB_ID = 0;

//缓存到内部数据
void  FPGA_SaveToBuffer()
{
    uint16_t tmp_ADC_Sample_Size;
    uint32_t data_len = 0;

    uint8_t *p_accum_datas[SOURCE_NUM] = {0};

    for(int i=0; i<SOURCE_NUM; i++)
        p_accum_datas[i] = (uint8_t *)FPGA_ACC_BUFFER[i];

    tmp_ADC_Sample_Size = ADC_Sample_Size;
    data_len = tmp_ADC_Sample_Size * ADC_Sample_Channel_Num * 4;

    ////////////////////////  读取数据
    memcpy((void*)FPGA_BUFFER, (void*)(FPGA_DATA_BASE_ADDR), data_len);

    //////////////////////// 叠加处理
    // 叠加数据
    int onesize = ADC_Sample_Channel_Num * 4;
#ifdef USE_COMP_SOURCE
    int neg = (((FPGA_MESSAGE_SUB_ID%2)==1) && ADC_Sample_NegSumFlag!=0)?-1:1;
    int ix = FPGA_MESSAGE_SUB_ID / 2;
    int sum_period = ADC_Sample_SumPeriod * 2;
#else
    int neg = (((FPGA_ACC_CNT%2)==1) && ADC_Sample_NegSumFlag!=0)?-1:1;
    int ix = 0;
    int sum_period = ADC_Sample_SumPeriod;
#endif
    for(int i=0; i<tmp_ADC_Sample_Size; i++){
        for(int j=0; j<ADC_Sample_Channel_Num; j++)
        {
            if(neg>0)
                FPGA_ACC_BUFFER[ix][i*ADC_Sample_Channel_Num+j] += *(int32_t*)(&FPGA_BUFFER[i*onesize+j*4]) / sum_period;
            else
                FPGA_ACC_BUFFER[ix][i*ADC_Sample_Channel_Num+j] -= *(int32_t*)(&FPGA_BUFFER[i*onesize+j*4]) / sum_period;
        }
    }

#ifdef USE_COMP_SOURCE
    FPGA_MESSAGE_SUB_ID++;
    if(FPGA_MESSAGE_SUB_ID >= 6){
        FPGA_MESSAGE_SUB_ID = 0;
#endif
        FPGA_ACC_CNT++;
#ifdef USE_COMP_SOURCE
    }
#endif

    // 判断叠加次数已满，存储数据
    if(FPGA_ACC_CNT >= ADC_Sample_SumPeriod)    // 叠加次数已购
    {
        FPGA_ACC_CNT = 0;

        //生成数据帧
#ifdef USE_COMP_SOURCE
        TP_GeneActiveReportAck2(FPGA_BUFFER, p_accum_datas, SOURCE_NUM, data_len);
#else
        TP_GeneActiveReportAck(FPGA_BUFFER, (uint8_t*)FPGA_ACC_BUFFER, data_len);
#endif
        // SD数据存储
        //printf("SD Write...\r\n");

        SD_DATA_Save(FPGA_BUFFER, data_len * SOURCE_NUM + FPGA_BUFFER_HEAD_SIZE);

        // 缓存监控数据
        if(MONITOR_FLAG == 0)
        {
#ifdef USE_COMP_SOURCE
            if(ADC_Sample_DataChannel == ADC_SAMPLE_DATA_CHANNEL_P900)
                TP_GeneActiveReportAck2(FPGA_MONITOR_BUFFER, p_accum_datas, SOURCE_NUM, P900_MONITOR_SAMPLE_LEN * ADC_Sample_Channel_Num * 4);
            else
                TP_GeneActiveReportAck2(FPGA_MONITOR_BUFFER, p_accum_datas, SOURCE_NUM, data_len);
#else

            if (ADC_Sample_DataChannel == ADC_SAMPLE_DATA_CHANNEL_P900) {
                if(FPGA_ONESAMPLE_FLAG == 1) {
                    TP_GeneActiveReportAck(FPGA_MONITOR_BUFFER, (uint8_t *) FPGA_ACC_BUFFER[0], data_len);
                }
                else {
                    if (FPGA_SPLITFRAME_FLAG == 1) {
                        TP_GeneActiveReportAck(FPGA_MONITOR_BUFFER, (uint8_t *) FPGA_ACC_BUFFER[0],
                                               P900_MONITOR_SAMPLE_LEN * ADC_Sample_Channel_Num * 4);
                    } else {
                        TP_GeneActiveReportAck(FPGA_MONITOR_BUFFER, (uint8_t *) FPGA_ACC_BUFFER[0],
                                               0 * ADC_Sample_Channel_Num * 4);
                    }
                }
            } else {
                TP_GeneActiveReportAck(FPGA_MONITOR_BUFFER, (uint8_t *) FPGA_ACC_BUFFER[0], data_len);
            }

#endif
            if(FPGA_ONESAMPLE_FLAG == 1) {
                if(FPGA_ONESAMPLE_CNT == 0) {
                    MONITOR_FLAG = 0;
                    FPGA_ONESAMPLE_CNT++;
                }
                else {
                    MONITOR_FLAG = 1;
                    FPGA_ONESAMPLE_CNT = 0;
                }
            }
            else
            {
                MONITOR_FLAG = 1;
            }
        }

        // 清空叠加缓存
        for(int i=0; i<SOURCE_NUM; i++)
            memset(FPGA_ACC_BUFFER[i], 0, tmp_ADC_Sample_Size * ADC_Sample_Channel_Num * 4);
    }

    FPGA_MESSAGE_ID++;
    FPGA_INTERRUPT_CNT--;
}

//获取可读的数据
uint8_t* FPGA_GetReadyBuffer()
{
    //查找可读块
    return (uint8_t*)FPGA_BUFFER;
}

//获取Monitor的数据
uint8_t* FPGA_GetMonitorBuffer()
{
    if(MONITOR_FLAG == 1)
    {
        return FPGA_MONITOR_BUFFER;
    }

    return NULL;
}

//获取Ready Buffer长度
int FPGA_GetReadyBufferLength()
{
    return ADC_Sample_Size * ADC_Sample_Channel_Num * 4;
}

//获取Monitor Buffer长度
int FPGA_GetMonitorBufferLength()
{
    if(ADC_Sample_DataChannel == ADC_SAMPLE_DATA_CHANNEL_P900) {
        if(FPGA_ONESAMPLE_FLAG == 1) {
            return SOURCE_NUM * ADC_Sample_Size * ADC_Sample_Channel_Num * 4 + FPGA_BUFFER_HEAD_SIZE;
        }
        else {
            if (FPGA_SPLITFRAME_FLAG == 1) {
                //FPGA_SPLITFRAME_FLAG = 0;
                return SOURCE_NUM * P900_MONITOR_SAMPLE_LEN * ADC_Sample_Channel_Num * 4 + FPGA_BUFFER_HEAD_SIZE;
            } else {
                return SOURCE_NUM * 0 * ADC_Sample_Channel_Num * 4 + FPGA_BUFFER_HEAD_SIZE;
            }
        }
    }
    else
        return SOURCE_NUM * ADC_Sample_Size * ADC_Sample_Channel_Num * 4 + FPGA_BUFFER_HEAD_SIZE;
}

//清除Monitor Buffer状态
void FPGA_ClearMonitorBuffer()
{
    if(1 == MONITOR_FLAG)
    {
        MONITOR_FLAG = 0;
    }

    if(FPGA_ONESAMPLE_FLAG == 1) {
        FPGA_ONESAMPLE_FLAG = 0;
        TP_FPGA_Stop();
    }
}

//重置Buffer
void FPGA_ResetBuffer()
{
    // 监控Buffer状态
    MONITOR_FLAG = 0;
    //FPGA_MESSAGE_ID=0;
    //FPGA_INTERRUPT_CNT=0;
}