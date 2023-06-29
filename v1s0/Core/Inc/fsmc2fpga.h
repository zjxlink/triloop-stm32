#ifndef __FSMC2FPGA_H
#define __FSMC2FPGA_H

#include <stdint.h>

#include "main.h"

//ADC通道数
#define FPGA_ADC_CHANNEL_NUM 4

//缓存点数
#define FPGA_BUFFER_POINT_SIZE 3200
//叠加缓存点数
#define FPGA_ACC_BUFFER_POINT_SIZE 900
//FPGA 缓存头大小 单位为byte
#define FPGA_BUFFER_HEAD_SIZE  64
//FPGA 缓存数据大小 单位为byte
#define FPGA_BUFFER_BODY_SIZE   (FPGA_ADC_CHANNEL_NUM*FPGA_BUFFER_POINT_SIZE*4)
//FPGA 缓存大小 单位为byte
#define FPGA_BUFFER_SIZE (FPGA_BUFFER_HEAD_SIZE + FPGA_BUFFER_BODY_SIZE)

//监控数据最大点数
#define FPGA_MONITOR_BUFFER_POINT_SIZE FPGA_BUFFER_POINT_SIZE
//监控数据BUFFER大小
#define FPGA_MONITOR_BUFFER_SIZE (FPGA_BUFFER_HEAD_SIZE + FPGA_MONITOR_BUFFER_POINT_SIZE*FPGA_ADC_CHANNEL_NUM*4)

// 用于组合源设备
//#define USE_COMP_SOURCE
#ifdef USE_COMP_SOURCE
// 发射源个数
#define SOURCE_NUM 3
#else
// 发射源个数
#define SOURCE_NUM 1
#endif /* USE_COMP_SOURCE */

//电台仅回传前400个数据的监控数据
#define P900_MONITOR_SAMPLE_LEN 400

//内部缓存
extern uint8_t FPGA_BUFFER[FPGA_BUFFER_SIZE];
//监控数据缓存
extern uint8_t FPGA_MONITOR_BUFFER[FPGA_MONITOR_BUFFER_SIZE];
// 分帧回传标志
extern uint8_t FPGA_SPLITFRAME_FLAG;

// 单次采集标志
extern uint8_t FPGA_ONESAMPLE_FLAG;
// 单次采集计数器
extern uint8_t FPGA_ONESAMPLE_CNT;

// 当前数据ID
extern uint32_t FPGA_MESSAGE_ID;
// 接收中断次数
extern int32_t FPGA_INTERRUPT_CNT;
//
extern uint8_t MONITOR_FLAG;
// 发送状态
//extern uint8_t monitor_send_flag;

// 叠加数据缓存
#ifdef USE_COMP_SOURCE
extern CCMRAM int32_t FPGA_ACC_BUFFER[SOURCE_NUM][FPGA_ACC_BUFFER_POINT_SIZE*FPGA_ADC_CHANNEL_NUM];
#else
extern CCMRAM int32_t FPGA_ACC_BUFFER[SOURCE_NUM][FPGA_BUFFER_POINT_SIZE*FPGA_ADC_CHANNEL_NUM];
#endif

//////////////////////////// FPGA 参数定义 //////////////////////////////
//ADC 通道ID
#define FPGA_CHANNEL_1          1
#define FPGA_CHANNEL_2          2
#define FPGA_CHANNEL_3          3
#define FPGA_CHANNEL_4          4
#define FPGA_CHANNEL_5          5
#define FPGA_CHANNEL_6          6
// 启动 停止
#define FPGA_START              0xA5
#define FPGA_STOP               0x5A
// 发射频率
#define FPGA_SENDFREQ_05HZ 	    0b0001
#define FPGA_SENDFREQ_1HZ 	    0b0010
#define FPGA_SENDFREQ_2HZ 	    0b0011
#define FPGA_SENDFREQ_4HZ 	    0b0100
#define FPGA_SENDFREQ_8HZ 	    0b0101
#define FPGA_SENDFREQ_16HZ 	    0b0110
#define FPGA_SENDFREQ_25HZ 	    0b0111
#define FPGA_SENDFREQ_32HZ 	    0b1000
#define FPGA_SENDFREQ_50HZ 	    0b1001
#define FPGA_SENDFREQ_64HZ 	    0b1010
#define FPGA_SENDFREQ_100HZ     0b1011
// 放大倍数
#define FPGA_AMP_0_125 	        0b0000
#define FPGA_AMP_0_25 	        0b0001
#define FPGA_AMP_0_5 	        0b0010
#define FPGA_AMP_1 		        0b0011
#define FPGA_AMP_2 		        0b0100
#define FPGA_AMP_4 		        0b0101
#define FPGA_AMP_8 		        0b0110
#define FPGA_AMP_16 	        0b0111
#define FPGA_AMP_32 	        0b1000
#define FPGA_AMP_64 	        0b1001
#define FPGA_AMP_128 	        0b1010
// 下采样因子
#define FPGA_DFACTOR_4 		    0b0010
#define FPGA_DFACTOR_8 		    0b0011
#define FPGA_DFACTOR_16 		0b0100
#define FPGA_DFACTOR_32 		0b0101
#define FPGA_DFACTOR_64 		0b0110
#define FPGA_DFACTOR_128 		0b0111
#define FPGA_DFACTOR_256 		0b1000
#define FPGA_DFACTOR_512 		0b1001
#define FPGA_DFACTOR_1024 	    0b1010
#define FPGA_DFACTOR_2048 	    0b1011
#define FPGA_DFACTOR_4096 	    0b1100
#define FPGA_DFACTOR_8192 	    0b1101
#define FPGA_DFACTOR_16384 	    0b1110
// 滤波类型
#define FPGA_FTYPE_SINC1 		0b0001
#define FPGA_FTYPE_SINC2 		0b0010
#define FPGA_FTYPE_SINC3 		0b0011
#define FPGA_FTYPE_SINC4 		0b0100
#define FPGA_FTYPE_SSINC 		0b0101
#define FPGA_FTYPE_FLAT_PASSBAND 0b0110
#define FPGA_FTYPE_AVERAGING 	0b0111

//////////////////////////// FPGA FSMC地址 //////////////////////////////
#define FPGA_BASE_ADDR                  0x68000000                  // FPGA基地址
#define FPGA_START_ADDR                 (FPGA_BASE_ADDR + 0x0)      // 启停标志地址, 0xA5——启动; 0x5A——停止
#define FPGA_ADCNUM_SENDFREQ_ADDR       (FPGA_BASE_ADDR + 0x2)      // bit7:4 ADC通道数   bit3:0 send_freq
#define FPGA_SAMPLENUMHIGH_ADDR         (FPGA_BASE_ADDR + 0x4)      // sample_num 高字节
#define FPGA_SAMPLENUMLOW_ADDR          (FPGA_BASE_ADDR + 0x6)      // sample_num 低字节
#define FPGA_DFACTOR_FTYPE_ADDR         (FPGA_BASE_ADDR + 0x8)      // bit7:4 dfactor 下采样因子  bit3:0 滤波类型
#define FPGA_AMP_RATIO_ADDR             (FPGA_BASE_ADDR + 0xA)      // 放大倍数
#define FPGA_SECOND_DFACTOR_FTYPE_ADDR  (FPGA_BASE_ADDR + 0xC)      // 辅采样率 bit7:4 dfactor 下采样因子  bit3:0 滤波类型
#define FPGA_COMP_DISABLE_DELAYHIGH_ADDR  (FPGA_BASE_ADDR + 0xE)      // comp_disable_delay 高字节
#define FPGA_COMP_DISABLE_DELAYLOW_ADDR   (FPGA_BASE_ADDR + 0x10)      // comp_disable_delay 低字节

#define FPGA_DATA_BASE_ADDR     (FPGA_BASE_ADDR)      //FPGA中数据缓存起始地址

//////////////////////////// 控制命令 //////////////////////////////
// 复位
#define FPGA_Reset() {  LL_GPIO_ResetOutputPin(GPIOB , LL_GPIO_PIN_3);\
                        LL_mDelay(10);\
                        LL_GPIO_SetOutputPin(GPIOB , LL_GPIO_PIN_3);\
                        LL_mDelay(10);}
// 启动
#define FPGA_Start() { *(__IO uint16_t *)(FPGA_START_ADDR) = FPGA_START; }
// 停止
#define FPGA_Stop() { *(__IO uint16_t *)(FPGA_START_ADDR) = FPGA_STOP; }

// 设置ADC通道数和发射频率
#define FPGA_SetADCChannelNum_SendFreq(num, freq) { *(__IO uint16_t *)(FPGA_ADCNUM_SENDFREQ_ADDR) = ((num<<4) | (freq&0x0f)); }

// 设置采样长度
#define FPGA_SetSampleNum(num) { *(__IO uint16_t *)(FPGA_SAMPLENUMHIGH_ADDR) = ((num)>>8)&0xff; \
                                *(__IO uint16_t *)(FPGA_SAMPLENUMLOW_ADDR) = (num&0xff);  }
// 设置下采样因子和滤波类型
#define FPGA_SetDFactor_FType(factor, type) { *(__IO uint16_t *)(FPGA_DFACTOR_FTYPE_ADDR) = ((factor<<4) | type); }

// 设置放大倍数
#define FPGA_SetAmpRatio(ratio) { *(__IO uint16_t *)(FPGA_AMP_RATIO_ADDR) = ratio; }

// 辅采样率 设置下采样因子和滤波类型
#define FPGA_SetSecondDFactor_FType(factor, type) { *(__IO uint16_t *)(FPGA_SECOND_DFACTOR_FTYPE_ADDR) = ((factor<<4) | type); }

// 设置补偿去使能延时
#define FPGA_SetCompDisableDelay(num) { *(__IO uint16_t *)(FPGA_COMP_DISABLE_DELAYHIGH_ADDR) = ((num)>>8)&0xff; \
                                *(__IO uint16_t *)(FPGA_COMP_DISABLE_DELAYLOW_ADDR) = (num&0xff);  }

//////////////////////////// 数据接口 //////////////////////////////
//数据获取接口
//#define FPGA_GetData_One(channelId, index) ({*(__IO uint32_t *) (FPGA_DATA_BASE_ADDR + (FPGA_ADC_CHANNEL_NUM*4*index+(channelId-1)*4));})

//判断Buffer是否满
int FPGA_IsBufferFull();

//缓存到内部数据
void FPGA_SaveToBuffer();

//获取可读的数据
uint8_t* FPGA_GetReadyBuffer();

//获取监视的数据
uint8_t* FPGA_GetMonitorBuffer();

//获取Ready Buffer长度
int FPGA_GetReadyBufferLength();

//获取Monitor Buffer长度
int FPGA_GetMonitorBufferLength();

//清除Monitor Buffer状态
void FPGA_ClearMonitorBuffer();

//重置Buffer
void FPGA_ResetBuffer();


#endif