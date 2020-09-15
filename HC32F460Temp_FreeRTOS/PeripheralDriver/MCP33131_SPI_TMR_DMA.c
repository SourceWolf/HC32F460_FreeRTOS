#include "hc32_ddl.h"
#include "MCP33131_SPI_TMR_DMA.h"
#include "User_FFT.h"
/*
接线
PE00 -- SCK  ---- SCK
PE02 -- MOSI ---- CNVST
PE03 -- MISO ---- SDO
GND  ----  GND
3.3V ----  3.3V
*/
#define DMA1_UNIT  M4_DMA1
#define DMA2_UNIT  M4_DMA2
#define MaxLlpnum (2u)

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
#define SPI3_SCK_PORT                    PortE
#define SPI3_SCK_PIN                     Pin00
#define SPI3_SCK_FUNC                    Func_Spi3_Sck

/* SPI_NSS Port/Pin definition */
#define SPI3_NSS_PORT                    PortE
#define SPI3_NSS_PIN                     Pin01
#define SPI3_NSS_FUNC                    Func_Spi3_Nss0

/* SPI_MOSI Port/Pin definition */
#define SPI3_MOSI_PORT                   PortE
#define SPI3_MOSI_PIN                    Pin02
#define SPI3_MOSI_FUNC                   Func_Spi3_Mosi

/* SPI_MISO Port/Pin definition */
#define SPI3_MISO_PORT                   PortE
#define SPI3_MISO_PIN                    Pin03
#define SPI3_MISO_FUNC                   Func_Spi3_Miso

/* SPI unit and clock definition */
#define SPI3_UNIT                        M4_SPI3
#define SPI3_UNIT_CLOCK                  PWC_FCG1_PERIPH_SPI3
#define SPI3_TX_INT_SOURCE               INT_SPI3_SRTI
#define SPI3_RX_INT_SOURCE               INT_SPI3_SRRI
#define SPI3_ERR_INT_SOURCE              INT_SPI3_SPEI
#define SPI3_ERR_IDEL_SOURCE             INT_SPI3_SPII



volatile stc_dma_llp_descriptor_t stcLlpDesc[2];

int16_t ADC_Data[2][2048];
float f_voltage[2048],fft_result[2048];
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
	stc_port_init_t Port_CFG;
    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcSpiInit);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
	MEM_ZERO_STRUCT(Port_CFG);
	
	Port_CFG.enPinMode = Pin_Mode_Out;
	Port_CFG.enPinOType = Pin_OType_Od;//OD输出
	Port_CFG.enPullUp = Enable;
	PORT_Init(SPI3_MOSI_PORT,SPI3_MOSI_PIN,&Port_CFG);
	
	
    /* Configuration peripheral clock */
    PWC_Fcg1PeriphClockCmd(SPI3_UNIT_CLOCK, Enable);

    /* Configuration SPI pin */
    PORT_SetFunc(SPI3_SCK_PORT, SPI3_SCK_PIN, SPI3_SCK_FUNC, Disable);
    PORT_SetFunc(SPI3_NSS_PORT, SPI3_NSS_PIN, SPI3_MOSI_FUNC, Disable);
//    PORT_SetFunc(SPI3_MOSI_PORT, SPI3_MOSI_PIN, SPI3_MOSI_FUNC, Disable);
    PORT_SetFunc(SPI3_MISO_PORT, SPI3_MISO_PIN, SPI3_MISO_FUNC, Disable);

    /* Configuration SPI structure */
    stcSpiInit.enClkDiv = SpiClkDiv16;//SPI datarate,PCLK1/128
    stcSpiInit.enFrameNumber = SpiFrameNumber1;
    stcSpiInit.enDataLength = SpiDataLengthBit16;
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
    
    /* SPI3 Error interrupt */
    stcIrqRegiConf.enIntSrc = SPI3_ERR_INT_SOURCE;
    stcIrqRegiConf.enIRQn = SPI3_ERR_IRQn;
    stcIrqRegiConf.pfnCallback = Hw_SPI3_ERR_Callback;
    /* Registration IRQ */
    enIrqRegistration(&stcIrqRegiConf);

    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
        
    /* Enable SPI */
    SPI_Cmd(SPI3_UNIT, Enable);
    Ddl_Delay1ms(10);
    SPI_IrqCmd(SPI3_UNIT, SpiIrqError, Enable);
}
void Hw_TimerA3_Init(void)
{
	stc_timera_base_init_t stcTimeraInit;
    stc_timera_compare_init_t stcTimerCompareInit;
    stc_port_init_t stcPortInit;
	uint16_t period = 0xF00;//0x2BC
//	uint32_t i;
//	for(i=0;i<4096;i++)
//	{
//		f_voltage[i] = 0;
//	}
//	
    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcTimeraInit);
    MEM_ZERO_STRUCT(stcTimerCompareInit);
	MEM_ZERO_STRUCT(stcPortInit);

    /* Configuration peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA3, Enable);

    /* Configuration TIMERA compare pin */
    PORT_SetFunc(PortE, Pin02, Func_Tima0, Disable);//reedit

    /* Configuration timera unit 1 base structure */
    stcTimeraInit.enClkDiv = TimeraPclkDiv1;
    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
    stcTimeraInit.enCntDir = TimeraCountDirUp;
    stcTimeraInit.enSyncStartupEn = Disable;
    stcTimeraInit.u16PeriodVal = period;//采样率  120Ksps   主频168MHZ，PCLK 84MHZ,   
    TIMERA_BaseInit(M4_TMRA3, &stcTimeraInit);

    /* Configuration timera unit 1 compare structure */
    stcTimerCompareInit.u16CompareVal = 100;
    stcTimerCompareInit.enStartCountOutput = TimeraCountStartOutputLow;//TimeraCountStartOutputLow//TimeraCountStartOutputHigh
    stcTimerCompareInit.enStopCountOutput = TimeraCountStopOutputLow;
    stcTimerCompareInit.enCompareMatchOutput = TimeraCompareMatchOutputReverse;
    stcTimerCompareInit.enPeriodMatchOutput = TimeraPeriodMatchOutputLow;
    stcTimerCompareInit.enSpecifyOutput = TimeraSpecifyOutputInvalid;
    stcTimerCompareInit.enCacheEn = Disable;
    stcTimerCompareInit.enTriangularTroughTransEn = Enable;
    stcTimerCompareInit.enTriangularCrestTransEn = Disable;
    stcTimerCompareInit.u16CompareCacheVal = stcTimerCompareInit.u16CompareVal;
    /* Configure Channel 1 */
	TIMERA_CompareInit(M4_TMRA3, TimeraCh5, &stcTimerCompareInit);
    TIMERA_CompareCmd(M4_TMRA3, TimeraCh5, Enable);
	
	TIMERA_SetCompareValue(M4_TMRA3, TimeraCh5, period/2);
	Hw_SPI3_DMA_TX_Init();
	Hw_SPI3_DMA_RX_Init();
	
	TIMERA_Cmd(M4_TMRA3, Enable);
	
}
void DMA_SPI_RX_Callback(void)
{
	static uint8_t cnt,pos;
	uint32_t i;
	/******
	当前Ready的数据是ADC_Data[cnt][x];
	*******/
	pos = cnt;
	cnt++;
	if(cnt>1)
	{
		cnt = 0;
	}
	DMA_SetDesAddress(M4_DMA1,DmaCh0,(uint32_t)(&ADC_Data[cnt][0]));
    DMA_SetTransferCnt(M4_DMA1,DmaCh0,2048);
	DMA_SetTransferCnt(M4_DMA1,DmaCh1,2048);
    DMA_ClearIrqFlag(M4_DMA1,DmaCh0, TrnCpltIrq);
    DMA1_UNIT->CHEN |= 0x03; 
	printf("fft_input:");
	for(i=0;i<2048;i++)
	{
		f_voltage[i] = ADC_Data[pos][i] * 4.0*2/65536;//基准4.0V
		printf("%f ",f_voltage[i]);
        printf("\r\n");
	}
	fft_test(f_voltage,fft_result);
	printf("fft_output:");
    for(i = 0;i < 2048; i++)
    {
        printf("%f ",fft_result[i]/512);
        printf("\r\n");
    }
    printf("\r\n");
}
void Hw_SPI3_DMA_RX_Init(void)
{
	uint8_t i;
    stc_dma_config_t stcDmaCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_dma_nseq_cfg_t stcSrcNseqCfg;
    MEM_ZERO_STRUCT(stcSrcNseqCfg);
    MEM_ZERO_STRUCT(stcDmaCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    
    stcDmaCfg.u16BlockSize = 1u;//
    stcDmaCfg.u16TransferCnt = 2048;//每个周期的总数据
    
    stcDmaCfg.u32DesAddr = (uint32_t)(&ADC_Data[0][0]);//(&DMA0_Dre_Data[0]);//Target Address//(uint32_t)&(Spwmdata[1].SPWMx.spwm[0][0]);//
    stcDmaCfg.u32SrcAddr = (uint32_t)(&SPI3_UNIT->DR);//USART2_DR_ADDRESS;//(uint32_t)(&DMA0_Src_data[0]);//Source Address
    
    /* Set repeat size. */
    stcDmaCfg.u16SrcRptSize = 0;
    stcDmaCfg.u16DesRptSize = 0;
    for(i=0;i<MaxLlpnum;i++)
    {
        stcLlpDesc[i].SARx = (uint32_t)&(SPI3_UNIT->DR);
        stcLlpDesc[i].DARx = (uint32_t)(&ADC_Data[i][0]);//(uint32_t)&(Spwmdata[1].SPWMx.spwm[0][0]);//
        stcLlpDesc[i].DTCTLx_f.CNT = 2048u;//单个链表的大小
        stcLlpDesc[i].DTCTLx_f.BLKSIZE = 1u;
        if(i==(MaxLlpnum-1))//是否最后一个链表，最后一个链表链接回第一个链表，作为回环
        {
            stcLlpDesc[i].LLPx = (uint32_t)(&stcLlpDesc[0]);
        }
        else
        {
            stcLlpDesc[i].LLPx = (uint32_t)(&stcLlpDesc[i+1]);
        }
        stcLlpDesc[i].CHxCTL_f.DRPTEN = Enable;//使能目标地址重置
        stcLlpDesc[i].CHxCTL_f.SRTPEN = Disable;//取消源地址重置
        stcLlpDesc[i].RPTx_f.DRPT = 0;
        stcLlpDesc[i].RPTx_f.SRPT = 0;
        stcLlpDesc[i].CHxCTL_f.SINC = AddressFix;//源地址自增
        stcLlpDesc[i].CHxCTL_f.DINC = AddressIncrease;//目标地址自增
        stcLlpDesc[i].CHxCTL_f.HSIZE = Dma16Bit;
        stcLlpDesc[i].CHxCTL_f.LLPEN = Enable;
        stcLlpDesc[i].CHxCTL_f.LLPRUN = LlpRunNow; 
        
        stcLlpDesc[i].CHxCTL_f.DNSEQEN = Disable;
        stcLlpDesc[i].CHxCTL_f.SNSEQEN = Disable;
		stcLlpDesc[i].CHxCTL_f.IE = Enable;
        stcLlpDesc[i].DNSEQCTLx_f.DNSCNT = 1;
        stcLlpDesc[i].DNSEQCTLx_f.DOFFSET = 1;       
    }
    
    
    /* Disable linked list transfer. */
//    stcDmaCfg.stcDmaChCfg.enLlpEn = Enable; 
//    stcDmaCfg.stcDmaChCfg.enLlpMd = LlpRunNow;
//    stcDmaCfg.u32DmaLlp = (uint32_t)(&stcLlpDesc[0]);//指向第一个链表
//    stcDmaCfg.stcDmaChCfg.enDesNseqEn = Enable;
//    stcDmaCfg.stcDesNseqCfg.u16Cnt = 1;
//    stcDmaCfg.stcDesNseqCfg.u32Offset = 1;
    /* Enable repeat function. */
    stcDmaCfg.stcDmaChCfg.enSrcRptEn = Disable;
    stcDmaCfg.stcDmaChCfg.enDesRptEn = Disable;   
//    stcDmaCfg.stcDmaChCfg.enSrcNseqEn = Disable;
//    stcDmaCfg.stcDmaChCfg.enDesNseqEn = Enable;
    /* Set source & destination address mode. */
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;//源地址自增
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressIncrease;
    /* Enable interrup. */
    stcDmaCfg.stcDmaChCfg.enIntEn = Enable;
    /* Set data width 32bit. */
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma16Bit;
    
	stcIrqRegiConf.enIntSrc = INT_DMA1_TC0;
    stcIrqRegiConf.enIRQn = Int030_IRQn;
    stcIrqRegiConf.pfnCallback = &DMA_SPI_RX_Callback;
    enIrqRegistration(&stcIrqRegiConf);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);

    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA1, Enable);
    
    /* Initialize DMA. */
    DMA_InitChannel(DMA1_UNIT, DmaCh0, &stcDmaCfg);
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(DMA1_UNIT, DmaCh0,Enable);
    /* Clear DMA transfer complete interrupt flag. */
    DMA_ClearIrqFlag(DMA1_UNIT, DmaCh0,TrnCpltIrq);

    
    /* Enable PTDIS(AOS) clock*/
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS,Enable);
    
    DMA_SetTriggerSrc(DMA1_UNIT,DmaCh0,EVT_SPI3_SRRI);//SPI3 接收事件触发
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(DMA1_UNIT, DmaCh0,Enable);
    /* Clear DMA transfer complete interrupt flag. */
    DMA_ClearIrqFlag(DMA1_UNIT, DmaCh0,TrnCpltIrq); 
    /* Enable DMA1. */
    DMA_Cmd(DMA1_UNIT,Enable);
}
void Hw_SPI3_DMA_TX_Init(void)
{
	static uint16_t data = CFG_CHANGE|INCC_GND|CHANNEL0|REF_EXT_TDIS|SEQ_DIS|NOTReadBackCFG;
	stc_dma_config_t stcDmaCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    MEM_ZERO_STRUCT(stcDmaCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    
    stcDmaCfg.u16BlockSize = 1u;//
    stcDmaCfg.u16TransferCnt = 2048;//无限传输
    
    stcDmaCfg.u32DesAddr = (uint32_t)(&SPI3_UNIT->DR);//(&DMA0_Dre_Data[0]);//Target Address
    stcDmaCfg.u32SrcAddr = (uint32_t)(&(data));//USART2_DR_ADDRESS;//(uint32_t)(&DMA0_Src_data[0]);//Source Address
    
    /* Set repeat size. */
    stcDmaCfg.u16SrcRptSize = 0;
    stcDmaCfg.u16DesRptSize = 0;

    /* Disable linked list transfer. */
    stcDmaCfg.stcDmaChCfg.enLlpEn = Disable;     
    /* Enable repeat function. */
    stcDmaCfg.stcDmaChCfg.enSrcRptEn = Enable;
    stcDmaCfg.stcDmaChCfg.enDesRptEn = Enable;   
    /* Set source & destination address mode. */
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;//地址不变
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressFix;
    /* Enable interrup. */
    stcDmaCfg.stcDmaChCfg.enIntEn = Disable;
    /* Set data width 32bit. */
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma16Bit;

    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA1, Enable);
    /* Enable DMA1. */
    DMA_Cmd(DMA1_UNIT,Enable);   
    /* Initialize DMA. */
    DMA_InitChannel(DMA1_UNIT, DmaCh1, &stcDmaCfg);
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(DMA1_UNIT, DmaCh1,Enable);
    /* Clear DMA transfer complete interrupt flag. */
    DMA_ClearIrqFlag(DMA1_UNIT, DmaCh1,TrnCpltIrq);
        
    /* Enable PTDIS(AOS) clock*/
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS,Enable);
    
    DMA_SetTriggerSrc(DMA1_UNIT,DmaCh1,EVT_TMRA3_OVF);      
}