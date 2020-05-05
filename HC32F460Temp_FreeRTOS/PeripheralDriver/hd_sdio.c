#include "hc32_ddl.h"
#include "sd_card.h"
#include "ff.h"
#define DEBUGN printf
#define DEBUGE printf
#define debug_100pin 1
#if !debug_100pin
#define SDIOC_CK_PORT                   PortB
#define SDIOC_CK_PIN                    Pin06

#define SDIOC_CMD_PORT                  PortB
#define SDIOC_CMD_PIN                   Pin00

#define SDIOC_D0_PORT                   PortB
#define SDIOC_D0_PIN                    Pin13

#define SDIOC_D1_PORT                   PortB
#define SDIOC_D1_PIN                    Pin12

#define SDIOC_D2_PORT                   PortB
#define SDIOC_D2_PIN                    Pin02

#define SDIOC_D3_PORT                   PortB
#define SDIOC_D3_PIN                    Pin01

//SD NAND��GPIO PB�����乩�����Ϊ��ʱ����
#define SDIOC_VCC_PORT                   PortB
#define SDIOC_VCC_PIN                    Pin15
#else
#define SDIOC_CK_PORT                   PortC
#define SDIOC_CK_PIN                    Pin12

#define SDIOC_CMD_PORT                  PortD
#define SDIOC_CMD_PIN                   Pin02

#define SDIOC_D0_PORT                   PortC
#define SDIOC_D0_PIN                    Pin08

#define SDIOC_D1_PORT                   PortC
#define SDIOC_D1_PIN                    Pin09

#define SDIOC_D2_PORT                   PortC
#define SDIOC_D2_PIN                    Pin10

#define SDIOC_D3_PORT                   PortC
#define SDIOC_D3_PIN                    Pin11

#define SDIOC_CD_PORT                   (PortE)
#define SDIOC_CD_PIN                    (Pin14)
//SD NAND��GPIO PB�����乩�����Ϊ��ʱ����
#define SDIOC_VCC_PORT                   PortA
#define SDIOC_VCC_PIN                    Pin07
#endif
/* SD sector && count */
#define SD_SECTOR_START                 (0u)
#define SD_SECTOR_COUNT                 (1u)

static en_result_t SdiocInitPins(void);

static stc_sdcard_init_t m_stcCardInitCfg =
{
    SdiocBusWidth4Bit,
    SdiocClk50M,
    SdiocHighSpeedMode,
};

static stc_sdcard_dma_init_t m_stcDmaInitCfg =
{
    M4_DMA1,
    DmaCh0,
};

stc_sd_handle_t m_stcSdhandle =
{
#if debug_100pin
    M4_SDIOC1,
#else
    M4_SDIOC2,
#endif
    //warn: �������DMA��ǰ������Ҫ��ʼ����Ӧ��DMAӲ����Դ��������ɶ���������
//    SdCardDmaMode,
//    &m_stcDmaInitCfg,
};

static uint8_t m_u32ReadBlocks[512 * 4];
static uint8_t m_u32WriteBlocks[512 * 4];

 static en_result_t SdiocInitPins(void)
{
    PORT_SetFunc(SDIOC_D0_PORT, SDIOC_D0_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_D1_PORT, SDIOC_D1_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_D2_PORT, SDIOC_D2_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_D3_PORT, SDIOC_D3_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_CD_PORT, SDIOC_CD_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_CK_PORT, SDIOC_CK_PIN, Func_Sdio, Disable);
    PORT_SetFunc(SDIOC_CMD_PORT, SDIOC_CMD_PIN, Func_Sdio, Disable);

#if ENABLE_EXTERAL_XTAL_12MHZ && debug_100pin
	DEBUGN("enter \r\n");
    //��ʼ��SD NAND VCC����GPIO������ʱ���乩��
    stc_port_init_t stcPortInit;
    MEM_ZERO_STRUCT(stcPortInit);
    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enExInt = Disable;
    stcPortInit.enPullUp = Disable;
    PORT_Init(SDIOC_VCC_PORT, SDIOC_VCC_PIN, &stcPortInit);
    PORT_ResetBits(SDIOC_VCC_PORT, SDIOC_VCC_PIN);
    //todo: ��SD NAND������Ƿ��б�Ҫ�ȴ�����msʱ�䣬��ȥ��ʼ������
    asl_time_delayms_nonos(10);  
#endif
	return Ok;
}

static void SD_CARD_Init(void)
{
    DEBUGN("enter \r\n");

    en_result_t enTestResult = Ok;
    SdiocInitPins();
    /* Initialilze SD card */
    enTestResult = SDCARD_Init(&m_stcSdhandle, &m_stcCardInitCfg);
    if(Ok != enTestResult)
    {
		DEBUGE("sd nand init fail, enTestResult = %d \r\n", enTestResult);
//		printf("sd nand init fail, enTestResult = %d \r\n", enTestResult);
        enTestResult = Error;
    }    
    else{
        //DEBUGN("sd nand init OK , capacity : %d, u32BlockSize = %d\r\n",
        //       m_stcSdhandle.stcSdCardInfo.u32BlockNbr, m_stcSdhandle.stcSdCardInfo.u32BlockSize);
		DEBUGN("sd nand init OK , capacity : %d, u32BlockSize = %d\r\n",
			m_stcSdhandle.stcSdCardInfo.u32BlockNbr, m_stcSdhandle.stcSdCardInfo.u32BlockSize);
    }

    DEBUGN("exit \r\n");
}    
static void SD_CARD_TEST(void)
{
    en_result_t enTestResult = Ok;
    en_result_t ret_s;

     /* Erase SD card */
    ret_s = SDCARD_Erase(&m_stcSdhandle, SD_SECTOR_START, SD_SECTOR_COUNT, 20);
    if(Ok != ret_s)
    {
        enTestResult = Error;
    }
    else{
        DEBUGE("test sd nand erase error , ret_s = %d\r\n", ret_s);
    }

//    /* Read SD card */
    if(Ok != SDCARD_ReadBlocks(&m_stcSdhandle, SD_SECTOR_START, SD_SECTOR_COUNT, (uint8_t *)m_u32ReadBlocks, 20))
    {
        DEBUGE("test sdio read error \r\n");
        enTestResult = Error;
    }

    /* Check whether data value is OxFFFFFFFF after erase SD card */
    for(uint32_t i = 0;  i < 512; i++)
    {
        if(m_u32ReadBlocks[i] != 0xFFFFFFFF)
        {
            DEBUGE("test read write error \r\n");
            enTestResult = Error;
            break;
        }
    }

    //��ʼ��sdio ��������
    for(int i=0; i<512 * 4; i++){
        m_u32WriteBlocks[i] = i;
    }

    /* Write SD card */
    for(int i=0; i<SD_SECTOR_COUNT; i++){
        ret_s = SDCARD_WriteBlocks(&m_stcSdhandle, SD_SECTOR_START + i, 1, (uint8_t *)m_u32WriteBlocks, 2000);
        if(Ok != ret_s)
        {
            DEBUGE("test read write error , ret_s = %d\r\n", ret_s);
            enTestResult = Error;
        }
        else{
            DEBUGN("write sdio nand data sector OK data start\r\n");
        }
    }

    /* Read SD card */
    
    DEBUGN("start read sd nand sector \r\n");
    ret_s = SDCARD_ReadBlocks(&m_stcSdhandle, SD_SECTOR_START, SD_SECTOR_COUNT, (uint8_t *)m_u32ReadBlocks, 2000);
    if(Ok != ret_s)
    {
        DEBUGE("error to read sdio nand sector \r\n");
        enTestResult = Error;
    }
    else{
        DEBUGN("read sdio nand data sector OK data start\r\n");
        
        for(int i=0; i<SD_SECTOR_COUNT*512; i++){
          printf("0x%02x ", m_u32ReadBlocks[i]);
        }
        
        DEBUGN("read sdio nand data sector OK data end \r\n");        
    }

    /* Compare read/write data */
    if(0 != memcmp(m_u32WriteBlocks, m_u32ReadBlocks, sizeof(m_u32ReadBlocks)))
    {
        DEBUGE("data is not the same in test sdio read and write \r\n");
        enTestResult = Error;
    }
}    

FATFS fs;
FIL File_1;
void hd_sdio_hw_init(void)
{
//    DEBUGN("enter hd sdio \r\n");
//    BYTE buffer[4096];
//    UINT br;
    SD_CARD_Init();

//    SD_CARD_TEST();

//    //�鿴SDIO������� ʹ�ܱ�־�Ƿ���
//    DEBUGN("====> m_stcSdhandle.SDIOCx->NORINTSTEN_f.TCEN = 0x%x \r\n", m_stcSdhandle.SDIOCx->NORINTSTEN_f.TCEN);

//    extern void hd_fatfs_test(void);
//    hd_fatfs_test();

	//for debug����ʽ���ļ�ϵͳ
	//asl_fop_fs_format();

//    static FRESULT res;
//    res = f_mount(0, &fs);
//    if (res)
//    {
//        DEBUGN("�ļ�ϵͳ����ʧ��.\r\n");
//    }
//    else
//    {
//        DEBUGN("�ļ�ϵͳ���سɹ�.\r\n");
//    }

//    res = f_open(&File_1,"0:a.txt", FA_OPEN_EXISTING | FA_READ);
//    res = f_read(&File_1,buffer,sizeof(buffer),&br);
//    res = f_close(&File_1);
//    res = f_mount(0,NULL);
//    DEBUGN("exit \r\n");
}


void enable_sdio_hw(bool enable)
{
#if 1
    //CMD��POWEROFF��Ĭ��Ϊ��
    if(enable){
        //CMD CLK�ָ�
        SDIOC_BusPowerOn(m_stcSdhandle.SDIOCx);
        PORT_SetFunc(SDIOC_CK_PORT, SDIOC_CK_PIN, Func_Sdio, Disable);
        M4_PORT->POERC_f.POUTE12 = 0;
        M4_PORT->PODRC_f.POUT12  = 0;
    }
    else{
        //CMDά��Ϊ�� CLK�ɵ͵��߸�������
        SDIOC_BusPowerOff(m_stcSdhandle.SDIOCx);
        PORT_SetFunc(SDIOC_CK_PORT, SDIOC_CK_PIN, Func_Gpio, Disable);
        M4_PORT->POERC_f.POUTE12 = 1;

        //CLK������ ������ ����������
        M4_PORT->PODRC_f.POUT12 = 0;
        Ddl_Delay1ms(1);
        M4_PORT->PODRC_f.POUT12 = 1;
    }
#endif
}

en_result_t SD_ReadDisk(uint8_t * buf, uint32_t blk_addr, uint16_t blk_len)
{
	return SDCARD_ReadBlocks(&m_stcSdhandle, blk_addr, blk_len, buf, 2000);
}

en_result_t SD_WriteDisk(uint8_t* buf, uint32_t blk_addr, uint16_t blk_len)
{
	return SDCARD_WriteBlocks(&m_stcSdhandle, blk_addr, blk_len, buf, 2000);
}