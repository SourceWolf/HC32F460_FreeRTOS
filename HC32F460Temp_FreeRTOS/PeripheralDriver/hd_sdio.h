#ifndef _HD_SDIO_H_
#define _HD_SDIO_H_
#include "hc32_ddl.h"
#include "stdbool.h"

//��ʼ���洢оƬ SD NAND Ӳ����Դ���ļ�ϵͳ��ʼ���ڴ˽ӿ��ڲ���ɣ������ط����ٶ��ļ�ϵͳ���г�ʼ����ע��ػ�����Ҫumount�ļ�ϵͳ
void hd_sdio_hw_init(void);

//ͨ�����ػ��ƣ�����SDIO��ʹ��ʱ�Ŀ��й���
void enable_sdio_hw(bool enable);

//��дSD block
en_result_t SD_ReadDisk(uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
en_result_t SD_WriteDisk(uint8_t* buf, uint32_t blk_addr, uint16_t blk_len);
#endif
