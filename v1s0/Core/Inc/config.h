//
// Created by Administrator on 2021/9/13.
//

#ifndef V2_CONFIG_H
#define V2_CONFIG_H

#include <stdint.h>

#define ADC_SAMPLE_START    0X01
#define ADC_SAMPLE_STOP     0X02

#define ADC_SAMPLE_FREQ_250K      0x00
#define ADC_SAMPLE_FREQ_125K      0x01
#define ADC_SAMPLE_FREQ_62K5      0x02
#define ADC_SAMPLE_FREQ_31K25     0x03
#define ADC_SAMPLE_FREQ_15K6      0x04
#define ADC_SAMPLE_FREQ_7K8       0x05
#define ADC_SAMPLE_FREQ_3K9       0x06
#define ADC_SAMPLE_FREQ_1K95      0x07
#define ADC_SAMPLE_FREQ_0K977     0x08
#define ADC_SAMPLE_FREQ_0K488     0x09
#define ADC_SAMPLE_FREQ_0K244     0x0A
#define ADC_SAMPLE_FREQ_0K122     0x0B
#define ADC_SAMPLE_FREQ_0K61      0x0C

#define ADC_RATIO_0_125    0x00
#define ADC_RATIO_0_25     0x01
#define ADC_RATIO_0_5      0x02
#define ADC_RATIO_1        0x03
#define ADC_RATIO_2        0x04
#define ADC_RATIO_4        0x05
#define ADC_RATIO_8        0x06
#define ADC_RATIO_16       0x07
#define ADC_RATIO_32       0x08
#define ADC_RATIO_64       0x09
#define ADC_RATIO_128      0x0A
#define ADC_RATIO_AUTO     0x0B

#define ADC_SAMPLE_NEGSUM_OPEN    0x01
#define ADC_SAMPLE_NEGSUM_CLOSE   0x00

#define ADC_SAMPLE_DATA_CHANNEL_USB    0x01
#define ADC_SAMPLE_DATA_CHANNEL_P900   0x02


//Error Code
#define P_ACK_ERROR_OK              0x00
#define P_ACK_ERROR_SAMPLEFREQ      0x01
#define P_ACK_ERROR_SAMPLESIZE      0x02
#define P_ACK_ERROR_FSJFREQ         0x03
#define P_ACK_ERROR_AMPRATIO        0x04
#define P_ACK_ERROR_CHANNEL         0x05
#define P_ACK_ERROR_START           0x06
#define P_ACK_ERROR_STOP            0x07

//放大系数值
#define AMP_RATIO_K_128       16777215      //6375341
#define AMP_RATIO_K_64        33554431      //12750683
#define AMP_RATIO_K_32        67108863      //25501367
#define AMP_RATIO_K_16        134217727     //51002736
#define AMP_RATIO_K_8         268435455     //102005472
#define AMP_RATIO_K_4         536870911     //204010946
#define AMP_RATIO_K_2         1073741823    //408021892
#define AMP_RATIO_K_1         2147483647
#define AMP_RATIO_UP          0.46
#define AMP_RATIO_DOWN        0.46

#define FPGA_SEND_FREQ_0Hz5 0x00
#define FPGA_SEND_FREQ_1Hz 0x01
#define FPGA_SEND_FREQ_2Hz 0x02
#define FPGA_SEND_FREQ_4Hz 0x03
#define FPGA_SEND_FREQ_8Hz 0x04
#define FPGA_SEND_FREQ_16Hz 0x05
#define FPGA_SEND_FREQ_25Hz 0x06
#define FPGA_SEND_FREQ_32Hz 0x07
#define FPGA_SEND_FREQ_50Hz 0x08
#define FPGA_SEND_FREQ_64Hz 0x09
#define FPGA_SEND_FREQ_100Hz 0x0A
#define FPGA_SEND_FREQ__CONTINUOUS 0x0F

//FPGA运行状态
extern uint8_t ADC_FPGA_Status;
//数据通道数
extern uint8_t ADC_Sample_Channel_Num;
//发射机频率
extern uint8_t ADC_FSJ_Freq;
//主采样率
extern uint16_t ADC_Sample_Freq;
//辅采样率
extern uint16_t ADC_Second_Sample_Freq;
//采样点数
extern uint16_t ADC_Sample_Size;
//叠加次数
extern uint16_t ADC_Sample_SumPeriod;
//放大倍数
extern uint8_t ADC_Amp_Ratio;
//反向叠加
extern uint8_t ADC_Sample_NegSumFlag;
//数据通道
extern uint8_t ADC_Sample_DataChannel;
// 数码电阻阻值
extern uint8_t ADC_Comp_Resistor;
// 发射机补偿去使能延时
extern uint16_t ADC_Comp_Disable_Delay;

//保存文件前缀
extern char ADC_Sample_DataFilePrefix[32];




#endif //V2_CONFIG_H
