#ifndef SENSORPARSE_H__
#define SENSORPARSE_H__

#include <stdint.h>

#define SENSOR_TYPE_NULL 		0
#define SENSOR_TYPE_NRA24 	1
#define SENSOR_TYPE_JY901 	2
#define SENSOR_TYPE_MAX 	2

#pragma pack(1)

/* 毫米波雷达数据 */
struct NRA24
{
		uint16_t Head;
		uint16_t MsgId;			//0x70C
		uint8_t Index;			//目标 ID
		uint8_t Rcs;				//目标反射截面积
		uint8_t RangeH;			//目标距离
		uint8_t RangeL;			//目标距离
		uint8_t Rsvd1;
		uint16_t RollCount;
		uint8_t CheckSum;
		uint16_t End;
};

/* 陀螺仪数据 */
struct JY901
{
		uint8_t Head;
		uint8_t Code;
		union{
			/* 时间 */
			struct {
				uint8_t year;
				uint8_t month;
				uint8_t day;
				uint8_t hour;
				uint8_t minute;
				uint8_t second;
				uint16_t msecond;
			} Time;  
			/* 角度 */
			struct {
				int16_t roll;				//滚转角（x 轴）
				int16_t pitch;				//俯仰角（y 轴）
				int16_t yaw;					//偏航角（z 轴）
				int16_t temperature;
			} degree;
		}Data;
		uint8_t CheckSum;
};

#pragma pack(4)

extern float SENSOR_ROLL, SENSOR_PITCH, SENSOR_YAW, SENSOR_HEIGHT;

extern void SensorParse(uint8_t m_type, uint8_t *data, uint16_t len);

#endif
