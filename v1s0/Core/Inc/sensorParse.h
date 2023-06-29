#ifndef SENSORPARSE_H__
#define SENSORPARSE_H__

#include <stdint.h>

#define SENSOR_TYPE_NULL 		0
#define SENSOR_TYPE_NRA24 	1
#define SENSOR_TYPE_JY901 	2
#define SENSOR_TYPE_MAX 	2

#pragma pack(1)

/* ���ײ��״����� */
struct NRA24
{
		uint16_t Head;
		uint16_t MsgId;			//0x70C
		uint8_t Index;			//Ŀ�� ID
		uint8_t Rcs;				//Ŀ�귴������
		uint8_t RangeH;			//Ŀ�����
		uint8_t RangeL;			//Ŀ�����
		uint8_t Rsvd1;
		uint16_t RollCount;
		uint8_t CheckSum;
		uint16_t End;
};

/* ���������� */
struct JY901
{
		uint8_t Head;
		uint8_t Code;
		union{
			/* ʱ�� */
			struct {
				uint8_t year;
				uint8_t month;
				uint8_t day;
				uint8_t hour;
				uint8_t minute;
				uint8_t second;
				uint16_t msecond;
			} Time;  
			/* �Ƕ� */
			struct {
				int16_t roll;				//��ת�ǣ�x �ᣩ
				int16_t pitch;				//�����ǣ�y �ᣩ
				int16_t yaw;					//ƫ���ǣ�z �ᣩ
				int16_t temperature;
			} degree;
		}Data;
		uint8_t CheckSum;
};

#pragma pack(4)

extern float SENSOR_ROLL, SENSOR_PITCH, SENSOR_YAW, SENSOR_HEIGHT;

extern void SensorParse(uint8_t m_type, uint8_t *data, uint16_t len);

#endif
