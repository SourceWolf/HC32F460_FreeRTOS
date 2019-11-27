#include "hc32_ddl.h"
#include "Hw_SPI3.h"

#define SPI3_TX_IRQn            Int004_IRQn
#define SPI3_RX_IRQn            Int005_IRQn
#define SPI3_ERR_IRQn           Int006_IRQn
#define SPI3_IDEL_IRQn          Int007_IRQn
#define DMA2_CH0_IRQn           Int003_IRQn
//unsigned char SPI_DATA;
/* Choose SPI master or slave mode */
#define SPI_MASTER_MODE
//#define SPI_SLAVE_MODE
/* SPI_SCK Port/Pin definition */
#define SPI3_SCK_PORT                    PortB
#define SPI3_SCK_PIN                     Pin12
#define SPI3_SCK_FUNC                    Func_Spi3_Sck

/* SPI_NSS Port/Pin definition */
#define SPI3_NSS_PORT                    PortB
#define SPI3_NSS_PIN                     Pin13
#define SPI3_NSS_FUNC                    Func_Spi3_Nss0

/* SPI_MOSI Port/Pin definition */
#define SPI3_MOSI_PORT                   PortB
#define SPI3_MOSI_PIN                    Pin14
#define SPI3_MOSI_FUNC                   Func_Spi3_Mosi

/* SPI_MISO Port/Pin definition */
#define SPI3_MISO_PORT                   PortB
#define SPI3_MISO_PIN                    Pin10
#define SPI3_MISO_FUNC                   Func_Spi3_Miso

/* SPI unit and clock definition */
#define SPI3_UNIT                        M4_SPI3
#define SPI3_UNIT_CLOCK                  PWC_FCG1_PERIPH_SPI3
#define SPI3_TX_INT_SOURCE               INT_SPI3_SRTI
#define SPI3_RX_INT_SOURCE               INT_SPI3_SRRI
#define SPI3_ERR_INT_SOURCE              INT_SPI3_SPEI
#define SPI3_ERR_IDEL_SOURCE             INT_SPI3_SPII

#define SPI3_DMA_UNIT2                (M4_DMA2)
#define SPI3_DMA_CH                  (DmaCh0)
#define SPI3_DMA_CLK                 PWC_FCG0_PERIPH_DMA2
#define SPI3_DMA_TRNCNT              (50u)//传输次数
#define DMA_BLKSIZE             (1u)
#define SPI3_DMA_RPT_SIZE            (50u)
#define DMA_INT_SRC             INT_DMA2_BTC0
#define DMA_Trg_Src             EVT_SPI3_SRRI

uint8_t SPI3_RX_Data;
bool flag_SPI3_RX, flag_SPI3_TX;
uint8_t SPI3_RXbuff[SPI3_DMA_TRNCNT],spi3_rx_counter;
void Hw_SPI3_TX_Callback(void)
{  
    SPI_SendData8(SPI3_UNIT,0xFF);
    flag_SPI3_TX = true;
}
void Hw_SPI3_RX_Callback(void)
{   
//    SPI_SetReadDataRegObject(SPI_UNIT,SpiReadReceiverBuffer);
    SPI3_RX_Data = SPI_ReceiveData8(SPI3_UNIT);
    if(SPI3_RX_Data == 0x55 && spi3_rx_counter > 1)
    {
        if(SPI3_RXbuff[1] != 0x22)
        {
            spi3_rx_counter = 0;
        }       
    }
    SPI3_RXbuff[spi3_rx_counter++] = SPI3_RX_Data;
    if(spi3_rx_counter>=50)
    {
        spi3_rx_counter = 0;
    }
    flag_SPI3_RX = true;
}
void Hw_SPI3_IDEL_Callback(void)
{
//    SPI_UNIT->SR_f.IDLNF = 1;
    flag_SPI3_RX = 0;;
}
void Hw_SPI3_ERR_Callback(void)
{
    if(SPI3_UNIT->SR_f.MODFERF)//模式故障
    {
        SPI3_UNIT->SR_f.MODFERF = 0;
    }
    if(SPI3_UNIT->SR_f.UDRERF)//欠载错误
    {
        SPI3_UNIT->SR_f.UDRERF = 0;
    }
    if(SPI3_UNIT->SR_f.PERF)//奇偶校验错误
    {
        SPI3_UNIT->SR_f.PERF = 0;
    }
    if(SPI3_UNIT->SR_f.OVRERF)//过载错误
    {
        SPI3_UNIT->SR_f.OVRERF = 0;
    }
    SPI3_UNIT->CR1_f.SPE = 1;
}
void Hw_SPI3_Init(void)
{
    stc_spi_init_t stcSpiInit;
    stc_irq_regi_conf_t stcIrqRegiConf;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcSpiInit);
    MEM_ZERO_STRUCT(stcIrqRegiConf);

    /* Configuration peripheral clock */
    PWC_Fcg1PeriphClockCmd(SPI3_UNIT_CLOCK, Enable);

    /* Configuration SPI pin */
    PORT_SetFunc(SPI3_SCK_PORT, SPI3_SCK_PIN, SPI3_SCK_FUNC, Disable);
    PORT_SetFunc(SPI3_NSS_PORT, SPI3_NSS_PIN, SPI3_NSS_FUNC, Disable);
    PORT_SetFunc(SPI3_MOSI_PORT, SPI3_MOSI_PIN, SPI3_MOSI_FUNC, Disable);
    PORT_SetFunc(SPI3_MISO_PORT, SPI3_MISO_PIN, SPI3_MISO_FUNC, Disable);

    /* Configuration SPI structure */
    stcSpiInit.enClkDiv = SpiClkDiv128;
    stcSpiInit.enFrameNumber = SpiFrameNumber1;
    stcSpiInit.enDataLength = SpiDataLengthBit8;
    stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
    stcSpiInit.enSckPolarity = SpiSckIdleLevelLow;
    stcSpiInit.enSckPhase = SpiSckOddSampleEvenChange;
    stcSpiInit.enReadBufferObject = SpiReadReceiverBuffer;
    stcSpiInit.enWorkMode = SpiWorkMode4Line;
    stcSpiInit.enTransMode = SpiTransFullDuplex;
    stcSpiInit.enCommAutoSuspendEn = Disable;
    stcSpiInit.enModeFaultErrorDetectEn = Disable;
    stcSpiInit.enParitySelfDetectEn = Disable;
    stcSpiInit.enParityEn = Disable;
    stcSpiInit.enParity = SpiParityEven;

#ifdef SPI_MASTER_MODE
    stcSpiInit.enMasterSlaveMode = SpiModeMaster;
    stcSpiInit.stcDelayConfig.enSsSetupDelayOption = SpiSsSetupDelayCustomValue;
    stcSpiInit.stcDelayConfig.enSsSetupDelayTime = SpiSsSetupDelaySck1;
    stcSpiInit.stcDelayConfig.enSsHoldDelayOption = SpiSsHoldDelayCustomValue;
    stcSpiInit.stcDelayConfig.enSsHoldDelayTime = SpiSsHoldDelaySck1;
    stcSpiInit.stcDelayConfig.enSsIntervalTimeOption = SpiSsIntervalCustomValue;
    stcSpiInit.stcDelayConfig.enSsIntervalTime = SpiSsIntervalSck6PlusPck2;
    stcSpiInit.stcSsConfig.enSsValidBit = SpiSsValidChannel0;
    stcSpiInit.stcSsConfig.enSs0Polarity = SpiSsLowValid;
#endif

#ifdef SPI_SLAVE_MODE
    stcSpiInit.enMasterSlaveMode = SpiModeSlave;
    stcSpiInit.stcSsConfig.enSsValidBit = SpiSsValidChannel0;
    stcSpiInit.stcSsConfig.enSs0Polarity = SpiSsLowValid;
#endif
    SPI_Init(SPI3_UNIT, &stcSpiInit);

    /* SPI3 tx interrupt */
    stcIrqRegiConf.enIntSrc = SPI3_TX_INT_SOURCE;
    stcIrqRegiConf.enIRQn = SPI3_TX_IRQn;
    stcIrqRegiConf.pfnCallback = Hw_SPI3_TX_Callback;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);
    /* Enable software trigger interrupt */
//    enIntEnable(Int5);

    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
//    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);

    /* SPI3 rx interrupt */
    stcIrqRegiConf.enIntSrc = SPI3_RX_INT_SOURCE;
    stcIrqRegiConf.enIRQn = SPI3_RX_IRQn;
    stcIrqRegiConf.pfnCallback = Hw_SPI3_RX_Callback;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
//    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    
    /* SPI3 Error interrupt */
    stcIrqRegiConf.enIntSrc = SPI3_ERR_INT_SOURCE;
    stcIrqRegiConf.enIRQn = SPI3_ERR_IRQn;
    stcIrqRegiConf.pfnCallback = Hw_SPI3_ERR_Callback;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    
    /* SPI3 idel interrupt */
    stcIrqRegiConf.enIntSrc = SPI3_ERR_IDEL_SOURCE;
    stcIrqRegiConf.enIRQn = SPI3_IDEL_IRQn;
    stcIrqRegiConf.pfnCallback = Hw_SPI3_IDEL_Callback;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    
    /* Enable SPI */
    SPI_Cmd(SPI3_UNIT, Enable);
//    SPI_UNIT->CR1_f.SPE = 1;
    Ddl_Delay1ms(10);
    SPI_IrqCmd(SPI3_UNIT, SpiIrqReceive, Enable);
    SPI_IrqCmd(SPI3_UNIT, SpiIrqSend, Enable);
    SPI_IrqCmd(SPI3_UNIT, SpiIrqError, Enable);
//    SPI_IrqCmd(SPI_UNIT, SpiIrqIdel, Enable);
//    SPI_Cmd(SPI_UNIT, Enable);
    Hw_SPI3_DMA_Init();
}

void Hw_SPI3_TEST(void)
{	
    while(SPI3_UNIT->SR_f.TDEF == 0);
    SPI_SendData8(SPI3_UNIT,0x55);	
}
void SPI3_DMA_Callback(void)
{
    DMA_ClearIrqFlag(SPI3_DMA_UNIT2,SPI3_DMA_CH, TrnCpltIrq);
    DMA_SetTransferCnt(SPI3_DMA_UNIT2,SPI3_DMA_CH,SPI3_DMA_TRNCNT);
    DMA_SetDesAddress(SPI3_DMA_UNIT2,SPI3_DMA_CH,(uint32_t)(SPI3_RXbuff));
    DMA_ChannelCmd(SPI3_DMA_UNIT2, SPI3_DMA_CH,Enable); 
}
void Hw_SPI3_DMA_Init(void)
{
    stc_dma_config_t stcDmaCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    MEM_ZERO_STRUCT(stcDmaCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    
    stcDmaCfg.u16BlockSize = DMA_BLKSIZE;//
    stcDmaCfg.u16TransferCnt = SPI3_DMA_TRNCNT;//
    
    stcDmaCfg.u32DesAddr = (uint32_t)(&SPI3_RXbuff[0]);//(&DMA0_Dre_Data[0]);//Target Address
    stcDmaCfg.u32SrcAddr = (uint32_t)(&(SPI3_UNIT->DR));//USART2_DR_ADDRESS;//(uint32_t)(&DMA0_Src_data[0]);//Source Address
    
    /* Set repeat size. */
    stcDmaCfg.u16SrcRptSize = 1;
    stcDmaCfg.u16DesRptSize = SPI3_DMA_RPT_SIZE;

    /* Disable linked list transfer. */
    stcDmaCfg.stcDmaChCfg.enLlpEn = Disable;     
    /* Enable repeat function. */
    stcDmaCfg.stcDmaChCfg.enSrcRptEn = Enable;
    stcDmaCfg.stcDmaChCfg.enDesRptEn = Enable;   
    /* Set source & destination address mode. */
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;//地址不变
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressIncrease;
    /* Enable interrup. */
    stcDmaCfg.stcDmaChCfg.enIntEn = Enable;
    /* Set data width 32bit. */
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma8Bit;

//    M4_MSTP->FCG0PC = 0xA5A50001;
//    M4_MSTP->FCG0_f.DMA1 = Reset;
//    M4_MSTP->FCG0PC = 0xA5A50000;
    PWC_Fcg0PeriphClockCmd(SPI3_DMA_CLK, Enable);
   
    /* Enable DMA1. */
    DMA_Cmd(SPI3_DMA_UNIT2,Enable);   
    /* Initialize DMA. */
    DMA_InitChannel(SPI3_DMA_UNIT2, SPI3_DMA_CH, &stcDmaCfg);
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(SPI3_DMA_UNIT2, SPI3_DMA_CH,Enable);
    /* Clear DMA transfer complete interrupt flag. */
    DMA_ClearIrqFlag(SPI3_DMA_UNIT2, SPI3_DMA_CH,TrnCpltIrq);
    
    stcIrqRegiConf.enIntSrc = INT_DMA2_TC0;
    stcIrqRegiConf.enIRQn = DMA2_CH0_IRQn;
    stcIrqRegiConf.pfnCallback =  SPI3_DMA_Callback;   
    
    enIrqRegistration(&stcIrqRegiConf);
	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//Enable Interrupt

    
    /* Enable PTDIS(AOS) clock*/
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS,Enable);
    
    DMA_SetTriggerSrc(SPI3_DMA_UNIT2,SPI3_DMA_CH,DMA_Trg_Src);
       
//    M4_AOS->INT_SFTTRG_f.STRG = 1;
//    
//    while(Set != DMA_GetIrqFlag(DMA_UNIT,DMA_CH, TrnCpltIrq))
//    {
//        M4_AOS->INT_SFTTRG_f.STRG = 1;
//    }
}
