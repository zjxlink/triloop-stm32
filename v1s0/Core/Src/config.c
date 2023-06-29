//
// Created by Administrator on 2021/9/13.
//

#include "config.h"
#include "main.h"

//FPGA运行状态
uint8_t ADC_FPGA_Status = 0;
//数据通道数
uint8_t ADC_Sample_Channel_Num = 1;
//发射频率
uint8_t ADC_FSJ_Freq = FPGA_SEND_FREQ__CONTINUOUS;
//主采样率
uint16_t ADC_Sample_Freq = 0;
//辅采样率
uint16_t ADC_Second_Sample_Freq = 0;
//采样点数
uint16_t ADC_Sample_Size = 500;
//叠加次数
uint16_t ADC_Sample_SumPeriod = 1;
//放大倍数
uint8_t ADC_Amp_Ratio = ADC_RATIO_1;
//反向叠加
uint8_t ADC_Sample_NegSumFlag = ADC_SAMPLE_NEGSUM_CLOSE;
//数据通道
uint8_t ADC_Sample_DataChannel = ADC_SAMPLE_DATA_CHANNEL_USB;
// 数码电阻阻值
uint8_t ADC_Comp_Resistor = 0;
// 发射机补偿去使能延时
uint16_t ADC_Comp_Disable_Delay = 300*50;

//保存文件前缀
char ADC_Sample_DataFilePrefix[32] = {"temp"};
//启停标志
uint8_t ADC_Start_Flag = ADC_SAMPLE_STOP;

//数据除数， 内部使用
int ADC_Amp_Ratio_Div = 0;
