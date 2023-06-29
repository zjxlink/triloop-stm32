#ifndef _SD_DATA_H
#define _SD_DATA_H

#define SD_DIR "0:/"

#define FILE_MAX_SIZE (104857600) //100M

extern uint8_t sd_flag;

extern uint32_t file_no;

extern uint32_t save_file_size;

extern uint8_t sd_sync_flag;

extern void SD_DATA_Test();

extern void SD_DATA_Save(uint8_t *data, uint32_t data_len);

#endif

