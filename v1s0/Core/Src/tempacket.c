#include "tempacket.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gpio.h"
#include "retarget.h"
#include "config.h"
#include "usbd_cdc_if.h"
#include "fsmc2fpga.h"
#include "tool.h"
#include "sd_data.h"
#include "sensorParse.h"
#include "mcp41010t.h"
#include "spi.h"

/* 分帧传输相关变量 */
uint16_t frameReqNo = 0;       // 帧序号
uint8_t frameStopFlag = 0;  // 结束分帧传输标志
uint8_t frameReqFlag = 0;   // 请求数据帧标志
uint8_t frameBuffer[256];   // 分帧数据Buffer

uint8_t tempData[128];
uint16_t has_recv_len=0, should_recv_len=0;
uint8_t head_flag=0;

uint32_t Timestamp = 0;
double Longitude = 0;
double Latitude = 0;
float Altitude = 0;
uint8_t GPSStatus = 0;

uint8_t TP_PACKET_HEAD[] = {0x68, 0x68, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x68, 0x68};

extern uint32_t Monitor_Send_Delay;
extern uint8_t FPGA_MESSAGE_SUB_ID;
extern uint16_t FPGA_ACC_CNT;

/* 发送响应 */
static void TP_SendAck(uint8_t func, uint8_t result, char *reason)
{
    struct TemPacket packet;

    TP_Init(&packet);

    packet.head.func = 0x80 | func;
    packet.head.len = 54;
    packet.Ack.result = result;
    strncpy(packet.Ack.reason, reason, strlen(reason)>53?53:strlen(reason));

    if(ADC_Sample_DataChannel == ADC_SAMPLE_DATA_CHANNEL_USB)
    {
        CDC_Transmit_FS((uint8_t *)&packet, sizeof(struct TemPacket));
    }
    else {
        My_USART_Send(USART6, (uint8_t *)&packet, sizeof(struct TemPacket));
    }
}

static void send_para_to_FPGA()
{
    /// 通道数和发射频率
    FPGA_SetADCChannelNum_SendFreq(ADC_Sample_Channel_Num, ADC_FSJ_Freq+1);

    /// 采样点数
    FPGA_SetSampleNum(ADC_Sample_Size);

    /// 主采样率 下采样因子 和 滤波类型
    uint8_t ftype = FPGA_FTYPE_SINC1;
    uint8_t dfactor;
    if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_250K)
    {
        dfactor = (FPGA_DFACTOR_4);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_125K)
    {
        dfactor = (FPGA_DFACTOR_8);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_62K5)
    {
        dfactor = (FPGA_DFACTOR_16);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_31K25)
    {
        dfactor = (FPGA_DFACTOR_32);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_15K6)
    {
        dfactor = (FPGA_DFACTOR_64);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_7K8)
    {
        dfactor = (FPGA_DFACTOR_128);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_3K9)
    {
        dfactor = (FPGA_DFACTOR_256);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_1K95)
    {
        dfactor = (FPGA_DFACTOR_512);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_0K977)
    {
        dfactor = (FPGA_DFACTOR_1024);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_0K488)
    {
        dfactor = (FPGA_DFACTOR_2048);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_0K244)
    {
        dfactor = (FPGA_DFACTOR_4096);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_0K122)
    {
        dfactor = (FPGA_DFACTOR_8192);
    }
    else if(ADC_Sample_Freq == ADC_SAMPLE_FREQ_0K61)
    {
        dfactor = (FPGA_DFACTOR_16384);
    }
    FPGA_SetDFactor_FType(dfactor, ftype);

    /// 设置放大倍数
    FPGA_SetAmpRatio(ADC_Amp_Ratio);

    /// 辅采样率 下采样因子 和 滤波类型
    if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_250K)
    {
        dfactor = (FPGA_DFACTOR_4);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_125K)
    {
        dfactor = (FPGA_DFACTOR_8);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_62K5)
    {
        dfactor = (FPGA_DFACTOR_16);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_31K25)
    {
        dfactor = (FPGA_DFACTOR_32);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_15K6)
    {
        dfactor = (FPGA_DFACTOR_64);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_7K8)
    {
        dfactor = (FPGA_DFACTOR_128);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_3K9)
    {
        dfactor = (FPGA_DFACTOR_256);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_1K95)
    {
        dfactor = (FPGA_DFACTOR_512);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_0K977)
    {
        dfactor = (FPGA_DFACTOR_1024);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_0K488)
    {
        dfactor = (FPGA_DFACTOR_2048);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_0K244)
    {
        dfactor = (FPGA_DFACTOR_4096);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_0K122)
    {
        dfactor = (FPGA_DFACTOR_8192);
    }
    else if(ADC_Second_Sample_Freq == ADC_SAMPLE_FREQ_0K61)
    {
        dfactor = (FPGA_DFACTOR_16384);
    }
    FPGA_SetSecondDFactor_FType(dfactor, ftype);

    /// 设置补偿去使能延时
    FPGA_SetCompDisableDelay(ADC_Comp_Disable_Delay);
}

static void TP_Handle(struct TemPacket *packet)
{
    if(packet->head.func == TP_TYPE_SETUP_REQ)
    {
        printf("TP_Handle, TP_TYPE_SETUP_REQ\r\n");

        /// 保存参数
        ADC_Sample_Channel_Num = packet->SetupReq.channelNum;
        ADC_FSJ_Freq = packet->SetupReq.sendFreq;
        ADC_Sample_DataChannel = packet->SetupReq.dataChannel;
        ADC_Sample_Freq = packet->SetupReq.sampleFreq;
        ADC_Second_Sample_Freq = packet->SetupReq.secondSampleFreq;
        ADC_Sample_Size = packet->SetupReq.sampleDepth;
        ADC_Amp_Ratio = packet->SetupReq.ampRatio;
        ADC_Sample_SumPeriod = packet->SetupReq.accNum;
        ADC_Sample_NegSumFlag = packet->SetupReq.negAcc;
        ADC_Comp_Resistor = packet->SetupReq.compRes;
        ADC_Comp_Disable_Delay = packet->SetupReq.compDisableDelay * 50;

        strncpy(ADC_Sample_DataFilePrefix, packet->SetupReq.filePrex, 16);

        printf("ADC_Sample_Channel_Num=%d\r\n", ADC_Sample_Channel_Num);
        printf("ADC_FSJ_Freq=%d\r\n", ADC_FSJ_Freq);
        printf("ADC_Sample_Freq=%d\r\n", ADC_Sample_Freq);
        printf("ADC_Sample_Size=%d\r\n", ADC_Sample_Size);
        printf("ADC_Sample_SumPeriod=%d\r\n", ADC_Sample_SumPeriod);
        printf("ADC_Amp_Ratio=%d\r\n", ADC_Amp_Ratio);
        printf("ADC_Comp_Resistor=%d\r\n", ADC_Comp_Resistor);

        /// 发送参数到FPGA
        send_para_to_FPGA();

        /// 设置数码电阻阻值
        MCP41010T_SetResistor(&hspi1, ADC_Comp_Resistor);

        /// 防止之前通讯依次导致的分帧传输异常未退出
        frameStopFlag = 1;

        /// 回复
        TP_SendAck(packet->head.func, TP_ACK_RESULT_SUCCESS, "ok");
    }
    else if(packet->head.func == TP_TYPE_CONTINUOUSSAMPLET_REQ)
    {
        printf("TP_Handle, TP_TYPE_CONTINUOUSSAMPLET_REQ\r\n");

        /// 启动连续采集
        printf("Start FPGA, message id = %d\r\n", FPGA_MESSAGE_ID);

        TP_FPGA_Start();

        /// 回复
        TP_SendAck(packet->head.func, TP_ACK_RESULT_SUCCESS, "ok");
    }
    else if(packet->head.func == TP_TYPE_SINGLESAMPLET_REQ) {
        printf("TP_Handle, TP_TYPE_SINGLESAMPLET_REQ\r\n");

        /// 启动连续采集
        printf("Start FPGA, message id = %d\r\n", FPGA_MESSAGE_ID);

        FPGA_ONESAMPLE_FLAG = 1;

        TP_FPGA_Start();

        /// 防止之前通讯依次导致的分帧传输异常未退出
        frameStopFlag = 1;

        /// 回复
        TP_SendAck(packet->head.func, TP_ACK_RESULT_SUCCESS, "ok");
    }
    else if(packet->head.func == TP_TYPE_STOPSAMPLET_REQ)
    {
        printf("TP_Handle, TP_TYPE_STOPSAMPLET_REQ\r\n");
        /// 停止采集
        printf("Stop FPGA, message id = %d\r\n", FPGA_MESSAGE_ID);

        TP_FPGA_Stop();

        /// 防止之前通讯依次导致的分帧传输异常未退出
        frameStopFlag = 1;

        /// 回复
        TP_SendAck(packet->head.func, TP_ACK_RESULT_SUCCESS, "ok");
    }
    else if(packet->head.func == TP_TYPE_SPLITFRAME_REQ)
    {
        printf("TP_Handle, TP_TYPE_SPLITFRAME_REQ\r\n");

        uint8_t subCmd = packet->SplitFrameReq.subCmd;

        if(subCmd == TP_ACK_SPLITFRAME_START)
        {
            FPGA_SPLITFRAME_FLAG = 1;
            frameStopFlag = 0;
            frameReqFlag = 0;
            frameReqNo = 0;

            /// 回复
            TP_SendAck(packet->head.func, TP_ACK_SPLITFRAME_START, "ok");
        }
        else if(subCmd == TP_ACK_SPLITFRAME_NORMAL)
        {
            frameReqFlag = 1;
            frameReqNo = packet->SplitFrameReq.frameNo;
        }
        else if(subCmd == TP_ACK_SPLITFRAME_STOP)
        {
            frameStopFlag = 1;

            /// 回复
            TP_SendAck(packet->head.func, TP_ACK_SPLITFRAME_STOP, "ok");
        }
    }
    else
    {
        printf("TP_Handle, Unknown\r\n");
    }
}

// 初始化帧头
void TP_Init(struct TemPacket *packet)
{
    memset(packet, 0, sizeof(struct TemPacket));
    memcpy(packet, TP_PACKET_HEAD, TP_PACKET_HEAD_LEN);
}

/* 解析数据 */
void TP_Parse(char *data, uint16_t len)
{
    int i;

    for(i=0;i<len;i++)
    {
        if(head_flag == 0)
        {
            tempData[TP_PACKET_HEAD_LEN-1] = data[i];

            if(     tempData[0]==0x68 && tempData[1]==0x68
                    && tempData[2]==0xff && tempData[3]==0xff && tempData[4]==0xff
                    && tempData[8]==0x68 && tempData[9]==0x68
                    )
            {
                head_flag = 1;

                has_recv_len = TP_PACKET_HEAD_LEN;
                should_recv_len = TP_PACKET_HEAD_LEN + tempData[6] + tempData[7]* 0x100;
                continue;
            }
            else
            {
                memmove(&tempData[0], &tempData[1], TP_PACKET_HEAD_LEN);
            }
            continue;
        }

        tempData[has_recv_len++] = data[i];

        if(has_recv_len >= should_recv_len)
        {
            //处理一帧数据
            TP_Handle((struct TemPacket *)tempData);

            //清除状态
            head_flag = 0;
            has_recv_len=0;
            should_recv_len=0;
            memset(tempData, 0, 128);
        }
    }
}

/* 生成主动上传帧数据 */
void TP_GeneActiveReportAck(uint8_t *buffer, uint8_t *data, uint32_t data_len)
{
    struct TemPacket *packet = (struct TemPacket *)buffer;
    TP_Init(packet);

    uint16_t len = data_len + TP_PACKET_MIN_LEN;

    packet->head.func = TP_TYPE_ACTIVEREPORT_ACK;
    packet->head.len = len;

    packet->ActiveReportAck.devId = 0x01;

    packet->ActiveReportAck.utc = Timestamp;

    packet->ActiveReportAck.longitude = Longitude;
    packet->ActiveReportAck.latitude = Latitude;
    packet->ActiveReportAck.altitude = Altitude;
    packet->ActiveReportAck.height = SENSOR_HEIGHT;
    packet->ActiveReportAck.gpsStatus = GPSStatus;
    packet->ActiveReportAck.sdStatus = sd_flag;
    packet->ActiveReportAck.ampRatio = ADC_Amp_Ratio;
    packet->ActiveReportAck.roll = SENSOR_ROLL;
    packet->ActiveReportAck.pitch = SENSOR_PITCH;
    packet->ActiveReportAck.yaw = SENSOR_YAW;
    packet->ActiveReportAck.channelNum = ADC_Sample_Channel_Num;

    //数据
    if(data) memcpy(&buffer[FPGA_BUFFER_HEAD_SIZE], data, data_len);
}


/* 生成组合源主动上传帧数据 */
void TP_GeneActiveReportAck2(uint8_t *buffer, uint8_t *data[], uint32_t data_channel_num, uint32_t data_len)
{
    struct TemPacket *packet = (struct TemPacket *)buffer;
    TP_Init(packet);

    uint16_t len = data_channel_num * data_len + TP_PACKET_MIN_LEN;

    packet->head.func = TP_TYPE_ACTIVECOMPREPORT_ACK;
    packet->head.len = len;

    packet->ActiveReportAck.devId = 0x01;

    packet->ActiveReportAck.utc = Timestamp;

    packet->ActiveReportAck.longitude = Longitude;
    packet->ActiveReportAck.latitude = Latitude;
    packet->ActiveReportAck.altitude = Altitude;
    packet->ActiveReportAck.height = SENSOR_HEIGHT;
    packet->ActiveReportAck.gpsStatus = GPSStatus;
    packet->ActiveReportAck.sdStatus = sd_flag;
    packet->ActiveReportAck.ampRatio = ADC_Amp_Ratio;
    packet->ActiveReportAck.roll = SENSOR_ROLL;
    packet->ActiveReportAck.pitch = SENSOR_PITCH;
    packet->ActiveReportAck.yaw = SENSOR_YAW;
    packet->ActiveReportAck.channelNum = ADC_Sample_Channel_Num;

    //数据
    for(int i=0; i<data_channel_num; i++) {
        memcpy(&buffer[FPGA_BUFFER_HEAD_SIZE+i*data_len], data[i], data_len);
    }
}

/* 生成分帧传输响应帧 */
uint8_t *TP_GeneSplitFrameAck(uint8_t *data, uint32_t data_len, uint16_t frameNo, uint8_t lastFrame)
{
    struct TemPacket *packet = (struct TemPacket *)frameBuffer;
    TP_Init(packet);

    packet->head.func = TP_TYPE_SPLITFRAMEA_ACK;
    packet->head.len = 4 + data_len;

    packet->SplitFrameAck.subCmd = (lastFrame==1 ? 0x82 : 0x02);
    packet->SplitFrameAck.data[0] = frameNo & 0xff;
    packet->SplitFrameAck.data[1] = (frameNo>>8) & 0xff;

    //数据
    memcpy(&packet->SplitFrameAck.data[2], data, data_len);

    //计算和校验
    uint8_t checksum = 0;
    for (int i = 0; i < data_len; i++) {
        checksum += data[i];
    }
    packet->SplitFrameAck.data[data_len+2] = checksum;

    return frameBuffer;
}

void TP_FPGA_Start()
{
    // 重置参数
    ADC_FPGA_Status = 1;
    Monitor_Send_Delay = 1;
    FPGA_MESSAGE_SUB_ID = 0;
    FPGA_ACC_CNT = 0;
    FPGA_MESSAGE_ID = 0;
    FPGA_INTERRUPT_CNT = 0;
    FPGA_ResetBuffer();
    for(int i=0; i<SOURCE_NUM; i++)
        memset(FPGA_ACC_BUFFER[i], 0, FPGA_ACC_BUFFER_POINT_SIZE * FPGA_ADC_CHANNEL_NUM * 4);

    // 复位FPGA
    //FPGA_Reset();

    // 发送配置参数到FPGA
    send_para_to_FPGA();
    LL_mDelay(100);

    // 发送启动命令
    FPGA_Start();
}

void TP_FPGA_Stop()
{
    // 重置参数
    ADC_FPGA_Status = 0;
    Monitor_Send_Delay = 1;
    FPGA_MESSAGE_SUB_ID = 0;
    FPGA_ACC_CNT = 0;
    FPGA_MESSAGE_ID = 0;
    FPGA_INTERRUPT_CNT = 0;
    FPGA_ResetBuffer();
    for(int i=0; i<SOURCE_NUM; i++)
        memset(FPGA_ACC_BUFFER[i], 0, FPGA_ACC_BUFFER_POINT_SIZE * FPGA_ADC_CHANNEL_NUM * 4);

    // 复位FPGA
    FPGA_Reset();
}