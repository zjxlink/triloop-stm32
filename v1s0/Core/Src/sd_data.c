#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sd_data.h"
#include "fatfs.h"
#include "config.h"
#include "usbd_cdc_if.h"

/* 文件编号 */
uint32_t file_no = 1;
/* 文件名 */
char filename[32];

uint32_t save_file_size = FILE_MAX_SIZE;

uint8_t sd_flag = 0;
uint8_t sd_sync_flag = 0;

void get_rand_str(char s[],int num)
//如果是在一个程序的循环中不断调用这个函数，那么是没有效果的
//虽然也是使用的系统的时间函数来初始化随机数发生器，但程序的
//执行速度太快了，可能执行1000次循环返回的秒数都是一样的
//time返回时间戳
{
    //定义随机生成字符串表
    char *str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int i,lstr;
    char ss[2] = {0};
    lstr = strlen(str);//计算字符串长度
    srand(HAL_GetTick());//使用系统时间来初始化随机数发生器
    for(i = 1; i <= num; i++){//按指定大小返回相应的字符串
        sprintf(ss,"%c",str[(rand()%lstr)]);//rand()%lstr 可随机返回0-71之间的整数, str[0-71]可随机得到其中的字符
        strcat(s,ss);//将随机生成的字符串连接到指定数组后面
    }
}


void SD_New_File(void)
{
    /*关闭之前的文件*/
    retSD = f_close(&SDFile);
    if (retSD == FR_OK) {

    } else
    {
        printf("Close %s Failed for: %d\n", filename, retSD);
    }

    char str[32] = {0};
    get_rand_str(str,16);
    //sprintf(filename, "%s_%d.dat", ADC_Sample_DataFilePrefix, file_no);
    sprintf(filename, "%s_%s.dat", ADC_Sample_DataFilePrefix, str);
    retSD = f_open(&SDFile, filename, FA_OPEN_APPEND | FA_WRITE | FA_READ); // 追加数据
    if (retSD == FR_OK) {
        save_file_size = 0;

        file_no++;
    } else {
        printf("Create %s Failed for: %d\n", filename, retSD);
        LL_mDelay(1);
    }
}

void SD_DATA_Test()
{
    //挂载系统
    retSD = f_mount(&SDFatFS, SD_DIR, 1);
    if (retSD == FR_OK) {
        printf("MOUNT OK\n");
    } else {
        printf("Mount Failed for %d\r\n", retSD);
        sd_flag = 1;
        return;
    }
    sd_flag = 0;
}

void SD_DATA_Save(uint8_t *data, uint32_t data_len)
{
    UINT byteswritten;
    if(sd_flag == 1) return;

    /*写数据*/
    retSD = f_write(&SDFile, data, data_len, &byteswritten);
    if(retSD==FR_OK)
    {
        save_file_size += byteswritten;

        if( 1 == sd_sync_flag )
        {
            f_sync(&SDFile);
            sd_sync_flag = 0;
        }
        sd_flag = 0;
    }
    else {
        printf("Write data to %s Failed for: %d\r\n", filename, retSD);

        sd_flag = 2;

        SD_New_File();
        LL_mDelay(1);
    }

    /*创建新文件*/
    if(save_file_size + data_len > FILE_MAX_SIZE)
    {
        SD_New_File();
    }
}
