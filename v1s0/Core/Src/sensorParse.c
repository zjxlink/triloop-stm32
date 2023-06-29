#include "sensorParse.h"

#include <stdio.h>
#include <string.h>

#define ONE_DATA_MAXLEN                  16   /* 不定长数据的最大长度 */

static char SensorData[SENSOR_TYPE_MAX][ONE_DATA_MAXLEN];
static uint16_t data_num = 0;

float SENSOR_ROLL, SENSOR_PITCH, SENSOR_YAW, SENSOR_HEIGHT;

/* 查看数据类型 */
int SensorType(uint8_t *data);
/* 获取角度信息 */
void GetDegree(uint8_t *data, float *roll, float *pitch, float *yaw);
/* 获取高度信息 */
float GetHeight(uint8_t *data);

/* 计算8位计算校验和 */
static uint8_t CalcCheckSum(uint8_t *data, uint16_t len)
{
    uint16_t sum = 0;

    for(int i=0; i<len; i++)
    {
        sum += data[i];
    }

    return (sum & 0xff);
}

/* 查看数据类型 */
int SensorType(uint8_t *data)
{
    struct NRA24 *p_NRA24 = (struct NRA24*)data;
    if(p_NRA24->Head == 0xAAAA && p_NRA24->End == 0x5555 && p_NRA24->CheckSum == CalcCheckSum((uint8_t*)&p_NRA24->Index, 7))
    {
        return SENSOR_TYPE_NRA24;
    }

    struct JY901 *p_JY901 = (struct JY901*)data;
    if(p_JY901->Head == 0x55 && p_JY901->Code == 0x53 && p_JY901->CheckSum == CalcCheckSum((uint8_t*)&p_JY901->Head, 10))
    {
        return SENSOR_TYPE_JY901;
    }

    return SENSOR_TYPE_NULL;
}


/* 获取角度信息 */
void GetDegree(uint8_t *data, float *roll, float *pitch, float *yaw)
{
    struct JY901 *p_JY901 = (struct JY901*)data;

    *roll = (float)p_JY901->Data.degree.roll / 32768.0 * 180;
    *pitch = (float)p_JY901->Data.degree.pitch / 32768.0 * 180;
    *yaw = (float)p_JY901->Data.degree.yaw / 32768.0 * 180;

}

/* 获取高度信息 */
float GetHeight(uint8_t *data)
{
    struct NRA24 *p_NRA24 = (struct NRA24*)data;

    return (p_NRA24->RangeH*0x100+p_NRA24->RangeL) * 0.01;
}


void SensorParse(uint8_t m_type, uint8_t *data, uint16_t len)
{
    for(int i=0; i<len; i++) {
        SensorData[m_type-1][data_num++] = data[i];
        //解析数据
        int type = SensorType((uint8_t *) SensorData[m_type-1]);
        if (m_type == SENSOR_TYPE_JY901 && type == SENSOR_TYPE_JY901) {

            GetDegree((uint8_t *) SensorData[m_type-1], &SENSOR_ROLL, &SENSOR_PITCH, &SENSOR_YAW);

            printf("JY901 Degree: %f, %f, %f\r\n", SENSOR_ROLL, SENSOR_PITCH, SENSOR_YAW);

            data_num = 0;
            memset(SensorData[m_type-1], 0, ONE_DATA_MAXLEN);
        } else if (m_type == SENSOR_TYPE_NRA24 && type == SENSOR_TYPE_NRA24) {
            float height = GetHeight((uint8_t*)SensorData[m_type-1]);

            printf("NRA24 Height: %f\r\n", height);

            if((int)(height*10000) != 0) {
                SENSOR_HEIGHT = height;
            }
            data_num = 0;
            memset(SensorData[m_type-1], 0, ONE_DATA_MAXLEN);
        } else {
            //移位存储
            if (data_num >= ONE_DATA_MAXLEN) {
                memmove(&SensorData[m_type-1][0], &SensorData[m_type-1][1], data_num - 1);
                data_num--;
            }
        }
    }
}












