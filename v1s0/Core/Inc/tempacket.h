#ifndef __TEM_PACKET_H
#define __TEM_PACKET_H

#include <stdint.h>

// 头大小
#define TP_PACKET_HEAD_LEN 10
// 最小数据长度
#define TP_PACKET_MIN_LEN 54

/* 帧类型 */
#define TP_TYPE_SETUP_REQ               0x01    // 配置参数请求
#define TP_TYPE_CONTINUOUSSAMPLET_REQ   0x02    // 连续采集请求
#define TP_TYPE_SINGLESAMPLET_REQ       0x03    // 单次采集请求
#define TP_TYPE_STOPSAMPLET_REQ         0x04    // 停止采集请求
#define TP_TYPE_ACTIVEREPORT_REQ        0x05    // 数据请求
#define TP_TYPE_SPLITFRAME_REQ          0x08    // 分帧传送请求

#define TP_TYPE_SETUP_ACK               0x81    // 配置参数响应
#define TP_TYPE_CONTINUOUSSAMPLET_ACK   0x82    // 连续采集响应
#define TP_TYPE_SINGLESAMPLET_ACK       0x83    // 单次采集响应
#define TP_TYPE_STOPSAMPLET_ACK         0x84    // 停止采集响应
#define TP_TYPE_ACTIVEREPORT_ACK        0x85    // 数据响应
#define TP_TYPE_ACTIVECOMPREPORT_ACK    0x95    // 组合源数据响应
#define TP_TYPE_SPLITFRAMEA_ACK         0x88    // 分帧传送响应

/* 响应结果类型 */
#define TP_ACK_RESULT_SUCCESS             0x01    // 成功
#define TP_ACK_RESULT_FAIL                0x02    // 失败

#define TP_ACK_SPLITFRAME_START           0x01    // 启动
#define TP_ACK_SPLITFRAME_NORMAL          0x02    // 请求数据（结束标志）
#define TP_ACK_SPLITFRAME_STOP            0x03    // 请求结束

#pragma pack(1)
struct TemPacket{
    /* 控制头 10字节 */
    struct {
        uint8_t head1[5];   // 0x68 0x68 0xff 0xff 0xff
        uint8_t func;       // 功能码
        uint16_t len;       // 数据域长度
        uint8_t head2[2];   // 0x68
    }head;
    /* 数据域 */
    union{
        /* 1.1 配置参数请求 */
        struct {
            uint8_t channelNum;         // 通道数
            uint8_t sendFreq;           // 发射频率
            uint8_t sampleFreq;         // 主采样频率
            uint16_t sampleDepth;       // 采样深度
            uint16_t accNum:15;         // 叠加次数
            uint8_t negAcc:1;           // 反向叠加
            uint8_t ampRatio;           // 放大倍数
            uint8_t dataChannel;        // 回传通道
            uint8_t compRes;            // 补偿电阻
            uint8_t secondSampleFreq;   // 辅采样频率
            uint16_t compDisableDelay;  // 补偿去使能延时
            uint8_t reserved[25];       // 预留
            char filePrex[16];          // SD卡文件前缀
        }SetupReq;
        /* 2.1 连续采集请求 */
        struct {
            uint8_t reserved[54];
        }ContinuousSampleReq;
        /* 3.1 单次采集请求 */
        struct {
            uint8_t reserved[54];
        }SingleSampleReq;
        /* 4.1 停止采集请求 */
        struct {
            uint8_t reserved[54];
        }StopSampleReq;
        /* 5.1 数据请求 */
        struct {
            uint8_t devId;          // 设备ID
            uint8_t mode;           // 响应模式
            uint8_t reserved[52];   // 预留
        }ActiveReportReq;
        /* 5.2 数据响应 */
        struct {
            uint8_t devId;          // 设备ID
            uint32_t utc;           // UTC秒
            double longitude;       // 经度
            double latitude;        // 纬度
            float altitude;         // 海拔
            float height;           // 高度
            uint8_t sdStatus:4;     // SD卡状态
            uint8_t gpsStatus:4;    // GPS状态
            uint8_t ampRatio;       // 当前放大倍数
            float roll;             // 陀螺仪Roll
            float pitch;            // 陀螺仪Pitch
            float yaw;              // 陀螺仪Yaw
            uint8_t channelNum;     // 通道数
            uint8_t reserved[10];
        }ActiveReportAck;
        /* 8.1 请求分帧传输 */
        struct {
            uint8_t subCmd;   // 子命令码
            uint16_t frameNo; // 帧序号
            uint8_t reserved[52];
        }SplitFrameReq;
        /* 8.2 数据响应 */
        struct {
            uint8_t subCmd;   // 子命令码
            uint8_t data[10]; // 根据子命令码填充不同的内容
        }SplitFrameAck;

        /* 响应 */
        struct {
            uint8_t result;         // 响应结果
            uint8_t reserved[21];   // 预留
            char reason[32];        // 原因
        }Ack;
    };
};
#pragma pack()

/* 分帧传输相关变量 */
extern uint8_t frameStopFlag;  // 结束分帧传输标志
extern uint8_t frameReqFlag;   // 请求数据帧标志
extern uint16_t frameReqNo;    // 帧序号

// 初始化帧头
void TP_Init(struct TemPacket *packet);

/* 解析数据 */
void TP_Parse(char *data, uint16_t len);

/* 生成数据响应帧 */
void TP_GeneActiveReportAck(uint8_t *buffer, uint8_t *data, uint32_t data_len);

/* 生成组合源数据响应帧 */
void TP_GeneActiveReportAck2(uint8_t *buffer, uint8_t *data[], uint32_t data_channel_num, uint32_t data_len);

/* 生成分帧传输响应帧 */
uint8_t *TP_GeneSplitFrameAck(uint8_t *data, uint32_t data_len, uint16_t frameNo, uint8_t lastFrame);

void TP_FPGA_Start();
void TP_FPGA_Stop();

#endif