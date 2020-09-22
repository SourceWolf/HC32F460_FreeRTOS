#include "bsp_I2S_Full_Duplex.h"
#include "cmsis_os.h"
//extern QueueHandle_t H_xQueue_Recode;
AudioBuff_type au16Rec[2];
bool flag_ab = 0;
uint8_t u8RecordEndFlag = 0;
uint32_t u32Count = 0;
const uint8_t Start[4]="PPPP"; 
const uint8_t End[4]="SSSS";

const uint8_t BData[4]="data";
volatile stc_dma_llp_descriptor_t stcLlpDesc[2],Tx_stcLlpDesc[2];
void I2S_Counter_TimerA_config(void);
void Play_DmaConfig(void);
void Uart_Send_data(uint8_t u8Data)
{
    M4_USART4->DR = u8Data;
    while (0 == M4_USART4->SR_f.TC);
//     USART_SendData(USART_CH, Uartdata[i]);
//	while(USART_CH->SR_f.TC == 0);//等待发送完成
}

uint8_t ppp_len = 0;

void Uart_Send_data_buffer(AudioBuff_type * pData, uint32_t len)
{
    uint32_t i;
	uint32_t num;
	uint32_t j;
    PORT_Toggle(PortB,Pin05);
	
	//data len
	uint32_t packet_data_len = 32;
	uint32_t packet_cut_len = 32000/packet_data_len;
	
	
	for(num=0;num<packet_cut_len;num++)
    {
		for(i=0;i<4;i++)
		{
			//Packet head
			Uart_Send_data(Start[i]);
		}
		
		for(j=num*packet_data_len;j<(num+1)*packet_data_len;j++)
		{
			Uart_Send_data(pData->Data8bit[j]);
			//printf("P%04X\n",pData->Data16bit[i]);
		}
		
		for(i=0;i<4;i++)
		{
			//Packet tail
			Uart_Send_data(End[i]);
		}

		
	}
    PORT_Toggle(PortB,Pin05);
}
/**
 *******************************************************************************
 ** \brief  Configuration Codec to record wav file
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void WM8731_CodecConfigRecord(void)
{
     stc_wm8731_reg_t stcWm8731Reg;
    MEM_ZERO_STRUCT(stcWm8731Reg);
    /* Config codec */
    /* Reset register */
    stcWm8731Reg.RESET              = 0x00u;    // Reset WM8731
    /* Left & right line input */
    stcWm8731Reg.LLIN_f.LINVOL      = 0x17u;    // Left channel line input volume: 0dB--0x17u
    stcWm8731Reg.LLIN_f.LINMUTE     = 0u;       // Enable left channel line input mute
    stcWm8731Reg.LLIN_f.LRINBOTH    = 0u;       // Disable simultaneous input volume and mute load from left to right
    stcWm8731Reg.RLIN_f.RINVOL      = 0x17u;    // Right channel line input volume 0dB
    stcWm8731Reg.RLIN_f.RINMUTE     = 0u;       // Enable right channel line input mute
    stcWm8731Reg.RLIN_f.RINBOTH     = 0u;       // Disable simultaneous input volume and mute load from right to left
    /* Left & right headphone output */
    stcWm8731Reg.LHOUT_f.LHPVOL     = 0x5F;     // Set volume of left headphone to 0dB. 0x30(-73dB) ~ 0x7F(+6dB), 0 ~ 0x2F: mute
    stcWm8731Reg.LHOUT_f.LZCEN      = 0u;       // Disable left channel zero cross detect
    stcWm8731Reg.LHOUT_f.LRHPBOTH   = 0u;       // Disable simultaneous output volume and mute load from left to right
    stcWm8731Reg.RHOUT_f.RHPVOL     = 0x5F;     // Set volume of right headphone to 0dB. 0x30(-73dB) ~ 0x7F(+6dB), 0 ~ 0x2F: mute
    stcWm8731Reg.RHOUT_f.RZCEN      = 0u;       // Enable right channel zero cross detect
    stcWm8731Reg.RHOUT_f.RLHPBOTH   = 0u;       // Disable simultaneous output volume and mute load from right to left
    /* Analog audio path control */
#ifdef RECORD_MIC
    stcWm8731Reg.AAPC_f.MICBOOST    = 1u;       // Disable boost, 0: disable 1: enable
    stcWm8731Reg.AAPC_f.MUTEMIC     = 0u;       // Enable mute to ADC
    stcWm8731Reg.AAPC_f.INSEL       = 1u;       // Line input select to ADC, 0: linein  1: mic
    stcWm8731Reg.AAPC_f.BYPASS      = 1u;       // Enbale bypass 0: disable 1:enable
    stcWm8731Reg.AAPC_f.DACSEL      = 1u;       // Select DAC
    stcWm8731Reg.AAPC_f.SIDETONE    = 0u;       // Disable side tone 0: disable 1:enable
    stcWm8731Reg.AAPC_f.SIDEATT     = 0u;       // 0: -6dB, 1: -12dB, 2: -9dB, 3: -15dB.
#else
    stcWm8731Reg.AAPC_f.MICBOOST    = 0u;       // Disable boost, 0: disable 1: enable
    stcWm8731Reg.AAPC_f.MUTEMIC     = 0u;       // Enable mute to ADC
    stcWm8731Reg.AAPC_f.INSEL       = 0u;       // Line input select to ADC, 0: linein  1: mic
    stcWm8731Reg.AAPC_f.BYPASS      = 1u;       // Enbale bypass 0: disable 1:enable
    stcWm8731Reg.AAPC_f.DACSEL      = 1u;       // Select DAC
    stcWm8731Reg.AAPC_f.SIDETONE    = 0u;       // Disable side tone 0: disable 1:enable
    stcWm8731Reg.AAPC_f.SIDEATT     = 0u;       // 0: -6dB, 1: -12dB, 2: -9dB, 3: -15dB.
#endif
    /* Digital audio path control */
    stcWm8731Reg.DAPC_f.ADCHPD      = 0u;       // Enable high pass filter
    stcWm8731Reg.DAPC_f.DEEMP       = 0u;       // De-emphasis contrl. 0: disable, 1: 32kHz, 2: 44.1kHz, 3: 48kHz
    stcWm8731Reg.DAPC_f.DACMU       = 0u;       // 0:Disable soft mute   1: Enable soft mute
    stcWm8731Reg.DAPC_f.HPOR        = 0u;       // Clear offset when high pass
    /* Power down control */
    stcWm8731Reg.PDC_f.LINEINPD     = 0u;       // Disable line input power down
    stcWm8731Reg.PDC_f.MICPD        = 0u;       // Disable microphone input power down
    stcWm8731Reg.PDC_f.ADCPD        = 0u;       // Disable ADC power down
    stcWm8731Reg.PDC_f.DACPD        = 0u;       // Disable DAC power down
    stcWm8731Reg.PDC_f.OUTPD        = 0u;       // Disable output power down
    stcWm8731Reg.PDC_f.OSCPD        = 0u;       // Disable oscillator power down
    stcWm8731Reg.PDC_f.CLKOUTPD     = 0u;       // Disable CLKOUT power down
    stcWm8731Reg.PDC_f.POWEROFF     = 0u;       // Disable power off mode
    /* Digital audio interface format */
    stcWm8731Reg.DAIF_f.FORMAT      = 2u;       // 0: MSB-First, right justified, 1: MSB-first, left justified, 2: I2S-format, 3: DSP mode
    stcWm8731Reg.DAIF_f.IWL         = 0u;       // 0: 16 bits, 1: 20 bits, 2: 24 bits, 3: 32 bits
    stcWm8731Reg.DAIF_f.LRP         = 0u;       // 1: right channel DAC data when DACLRC (WS) is high,  0: right channel DAC data when DACLRC (WS) is low
    stcWm8731Reg.DAIF_f.LRSWAP      = 0u;       // 1: swap left channel and right channel, 0: don't swap
    stcWm8731Reg.DAIF_f.MS          = 0u;       // 1: Enable master mode, 0: Enable slave mode
    stcWm8731Reg.DAIF_f.BCLKINV     = 0u;       // Don't invert BCLK
    /* Sampling control */
    stcWm8731Reg.SC_f.NORMAL_USB    = 0u;       // 0: normal mode, 1: USB mode
    stcWm8731Reg.SC_f.BOSR          = 0u;       // Nomrmal mode: 0: 256fs, 1: 384fs
                                                // USB mode: 0: 250fs, 1:272fs
    stcWm8731Reg.SC_f.SR            = 2u;       // Sample rate setting
    stcWm8731Reg.SC_f.CLKDIV2       = 0u;       // 0: core clock is MCLK, 1: core clock is MCLK divided by 2
    stcWm8731Reg.SC_f.CLKODIV2      = 0u;       // 0: output clock is core clock, 1: core clock is core clock/2
    // Active control
    stcWm8731Reg.AC_f.ACTIVE        = 1u;       // 0: inactive, 1: active

    if(Ok != WM8731_Init(I2C_CH, &stcWm8731Reg))
    {
        while(1);
    }
    WM8731_SetHpVolume(I2C_CH, 0x6F,0x6F);  //0x2F-MUTE ~ 0x7F Maximum
}
void REC_DMA_CALLBACK(void)
{
    flag_ab = !flag_ab;
    if(flag_ab)
    {
        PORT_Toggle(PortE,Pin06);
//        xQueueSend(H_xQueue_Recode,&au16Rec[0],1);
//		memcpy(&au16ply[0],&au16Rec[0],RECORDER_WAVFILELEN);
        //Uart_Send_data_buffer(&au16Rec[0],RECORDER_WAVFILELEN);
    }
    else
    {
        PORT_Toggle(PortA,Pin07);
//        xQueueSend(H_xQueue_Recode,&au16Rec[1],1);
//		memcpy(&au16ply[1],&au16Rec[1],RECORDER_WAVFILELEN);		
        //Uart_Send_data_buffer(&au16Rec[1],RECORDER_WAVFILELEN);
    }    
}
/**
 *******************************************************************************
 ** \brief  Initialize DMAC function for recorder
 **
 ** \param  None
 **
 ** \retval None
 **
 ******************************************************************************/
static void Record_DmaConfig(void)
{
	uint8_t i;
    stc_dma_config_t stcDmaCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    MEM_ZERO_STRUCT(stcDmaCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    
    /* Set data block size. */
    stcDmaCfg.u16BlockSize = 1;  //uint16
    /* Set transfer count. */
    stcDmaCfg.u16TransferCnt = 1;
    /* Set source & destination address. */
    stcDmaCfg.u32SrcAddr = (uint32_t)(&I2S_CH->RXBUF);
    stcDmaCfg.u32DesAddr = (uint32_t)(&au16Rec[0].Data16bit[0]);

    /* Disable linked list transfer. */
    stcDmaCfg.stcDmaChCfg.enLlpEn = Disable;
    /* Enable repeat function. */
    stcDmaCfg.stcDmaChCfg.enSrcRptEn = Disable;
    stcDmaCfg.stcDmaChCfg.enDesRptEn = Disable;
    /* Set source & destination address mode. */
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressIncrease;
    /* Enable interrup. */
    stcDmaCfg.stcDmaChCfg.enIntEn = Enable;
    /* Set data width 32bit. */
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma16Bit;
	
	for(i=0;i<MaxLlpnum;i++)
    {
        stcLlpDesc[i].SARx = (uint32_t)(&I2S_CH->RXBUF);
        stcLlpDesc[i].DARx = (uint32_t)(&au16Rec[i].Data16bit[0]);//(uint32_t)&(Spwmdata[1].SPWMx.spwm[0][0]);//
        stcLlpDesc[i].DTCTLx_f.CNT = RECORDER_WAVFILELEN;//单个链表的大小
        stcLlpDesc[i].DTCTLx_f.BLKSIZE = 1u;
        if(i==(MaxLlpnum-1))//是否最后一个链表，最后一个链表链接回第一个链表，作为回环
        {
            stcLlpDesc[i].LLPx = (uint32_t)(&stcLlpDesc[0]);
        }
        else
        {
            stcLlpDesc[i].LLPx = (uint32_t)(&stcLlpDesc[i+1]);
        }
        stcLlpDesc[i].CHxCTL_f.DRPTEN = Disable;//使能目标地址重置
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
	stcDmaCfg.stcDmaChCfg.enLlpEn = Enable; 
    stcDmaCfg.stcDmaChCfg.enLlpMd = LlpRunNow;
    stcDmaCfg.u32DmaLlp = (uint32_t)(&stcLlpDesc[0]);//指向第一个链表
    /* Enable DMA clock. */
    PWC_Fcg0PeriphClockCmd(RX_DMA_CLK,Enable);

    /* Enable DMA1. */
    DMA_Cmd(RX_DMA_UNIT,Enable);
    /* Initialize DMA. */
    DMA_InitChannel(RX_DMA_UNIT, RX_DMA_CH, &stcDmaCfg);
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(RX_DMA_UNIT, RX_DMA_CH,Enable);
    
    DMA_ClearIrqFlag(RX_DMA_UNIT, RX_DMA_CH,TrnCpltIrq);
    
//    stcIrqRegiConf.enIntSrc = INT_DMA2_TC0;
//    stcIrqRegiConf.enIRQn = Int008_IRQn;
//    stcIrqRegiConf.pfnCallback =  REC_DMA_CALLBACK;   
//    
//    enIrqRegistration(&stcIrqRegiConf);
//	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//Enable Interrupt
//    
    /* Enable PTDIS(AOS) clock*/
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS,Enable); 
    DMA_SetTriggerSrc(RX_DMA_UNIT, RX_DMA_CH, RX_DMA_TRG);
}
void I2S3_ERROR_Callback(void)
{
    static uint32_t errcnt;
    if(M4_I2S3->ER_f.RXERR)
    {
        uint32_t rxbuf;
        errcnt++;
        rxbuf = M4_I2S3->RXBUF;
        M4_I2S3->ER_f.RXERR = 1;         //Clear the RX ERROR
    }
    if(M4_I2S3->ER_f.TXERR)
    {
        M4_I2S3->ER_f.TXERR = 1;
    }
}
/**
 *******************************************************************************
 ** \brief  Main function of example project
 **
 ** \param  None
 **
 ** \retval int32_t return value, if needed
 **
 ******************************************************************************/
int32_t i2s_record_duplex_init(void)
{
    stc_i2s_config_t stcI2sCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t stcPortIni;

    MEM_ZERO_STRUCT(stcI2sCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);

//    /* Initialize system clock, PLL 168M */
//    SysClkIni();
//    /* Initialize LED0 */
//    IniLedPort();
//    Ddl_UartInit();
    /* Initialize i2c port for codec wm8731 */
    PORT_SetFunc(I2C2_SCL_PORT, I2C2_SCL_PIN, Func_I2c2_Scl, Disable);
    PORT_SetFunc(I2C2_SDA_PORT, I2C2_SDA_PIN, Func_I2c2_Sda, Disable);
    /* Enable I2C Peripheral*/
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_I2C2, Enable);

    /* Initialize i2s port for codec wm8731 recorder function */
    MEM_ZERO_STRUCT(stcPortIni);
    stcPortIni.enPullUp = Enable;
    stcPortIni.enPinDrv = Pin_Drv_H;
    PORT_Init(I2S3_PORT, I2S3_CK_PIN, &stcPortIni);
    PORT_Init(I2S3_PORT, I2S3_WS_PIN, &stcPortIni);
    PORT_Init(I2S3_PORT, I2S3_SD_IN_PIN, &stcPortIni);
    PORT_SetFunc(I2S3_PORT, I2S3_CK_PIN, Func_I2s3_Ck, Disable);
    PORT_SetFunc(I2S3_PORT, I2S3_WS_PIN, Func_I2s3_Ws, Disable);
    PORT_SetFunc(I2S3_PORT, I2S3_SD_IN_PIN, Func_I2s3_Sdin, Disable);
	PORT_SetFunc(I2S3_PORT, I2S3_SD_PIN, Func_I2s3_Sd, Disable);
#ifdef EXCK_ON
    PORT_Init(I2S3_EXCK_PORT, I2S3_EXCK_PIN, &stcPortIni);
    PORT_SetFunc(I2S3_EXCK_PORT, I2S3_EXCK_PIN, Func_I2s, Disable);
#else
    PORT_Init(I2S3_MCK_PORT, I2S3_MCK_PIN, &stcPortIni);
    PORT_SetFunc(I2S3_MCK_PORT, I2S3_MCK_PIN, Func_I2s, Disable);
#endif
    /* Enable I2S Peripheral*/
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_I2S3, Enable);

    /* WM8731 configuration for recorder */
    WM8731_CodecConfigRecord();

    /* Config clock source for i2s */
    CLK_SetI2sClkSource(I2S_CH, ClkPeriSrcMpllp);
#if 1
    /* Config i2s peripheral */
    I2s_DeInit(I2S_CH);
    stcI2sCfg.enStandrad = Std_Philips;
    stcI2sCfg.enMode = I2sMaster;
    stcI2sCfg.enChanelLen = I2s_ChLen_16Bit;
    stcI2sCfg.enDataBits = I2s_DataLen_16Bit;
    stcI2sCfg.u32AudioFreq = I2S_AudioFreq_8k;
#ifdef EXCK_ON
    stcI2sCfg.enMcoOutEn = Disable;
    stcI2sCfg.enExckEn = Enable;
#else
    stcI2sCfg.enMcoOutEn = Enable;
    stcI2sCfg.enExckEn = Disable;
#endif   
    /* Config DMAC for recorder*/
	I2S_Counter_TimerA_config();
    Record_DmaConfig();
	Play_DmaConfig();
	I2s_Init(I2S_CH, &stcI2sCfg);
#ifdef SPEAKER_ON
    /* Initialize SPK_EN port for speaker */
    MEM_ZERO_STRUCT(stcPortIni);
    stcPortIni.enPinMode = Pin_Mode_Out;
    PORT_Init(SPK_EN_PORT, SPK_EN_PIN, &stcPortIni);
    SPEAKER_EN();
#endif
    /* Prepare buffer for store recoder data */
//    pu16SoundData = &au16RecorderSoundI2s[0];

    /* Wait for press key SW2 to kick start record */
//    while(0 != PORT_GetBit(PortD, Pin03));   //SW2
//    /* Indicate recording */
//    LED0_TOGGLE();
    /* Enable I2S RXI function to kick start */
    I2S_FuncCmd(I2S_CH, RxIntEn, Enable);
    /* Enable i2s RX function */
    I2S_FuncCmd(I2S_CH, RxEn, Enable);
    /* Enable i2s ER Int function */
    I2S_FuncCmd(I2S_CH, ErrIntEn, Enable);
    I2S_FuncCmd(I2S_CH, TxEn, Enable);
    I2S_CH->CTRL_f.DUPLEX = 1;
    stcIrqRegiConf.enIntSrc = INT_I2S3_ERRIRQOUT;
    stcIrqRegiConf.enIRQn = Int009_IRQn;
    stcIrqRegiConf.pfnCallback =  I2S3_ERROR_Callback;   
    
    enIrqRegistration(&stcIrqRegiConf);
	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//Enable Interrupt
#endif
	return 0;
}
void I2S_Counter_TimerA_config(void)
{
   stc_timera_base_init_t stcTimeraInit;
    stc_irq_regi_conf_t stcIrqRegiConf;
	stc_timera_orthogonal_coding_init_t Hw_Count_Trg_Cfg;
    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcTimeraInit);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(Hw_Count_Trg_Cfg);

    /* Configuration peripheral clock */
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIMA1, Enable);
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS, Enable);

    /* Configuration TIMERA compare pin */

    /* Configuration timera unit 1 base structure */
    stcTimeraInit.enClkDiv = TimeraPclkDiv1;
    stcTimeraInit.enCntMode = TimeraCountModeSawtoothWave;
    stcTimeraInit.enCntDir = TimeraCountDirUp;
    stcTimeraInit.enSyncStartupEn = Disable;
    stcTimeraInit.u16PeriodVal = RECORDER_WAVFILELEN-1;        //freq: 100Hz
    TIMERA_BaseInit(M4_TMRA1, &stcTimeraInit);


    /* Enable period count interrupt */
    TIMERA_IrqCmd(M4_TMRA1, TimeraIrqOverflow, Enable);
    /* Interrupt of timera unit 1 */
    stcIrqRegiConf.enIntSrc = INT_TMRA1_OVF;
    stcIrqRegiConf.enIRQn = TIMERA1_IRQn;
    stcIrqRegiConf.pfnCallback = REC_DMA_CALLBACK;
    enIrqRegistration(&stcIrqRegiConf);
    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_15);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
	TIMERA_SetCountTriggerSrc(TIMERA_COUNT_TRG);
	
	Hw_Count_Trg_Cfg.enIncSpecifyEventTriggerEn = Enable;
	TIMERA_OrthogonalCodingInit(M4_TMRA1,&Hw_Count_Trg_Cfg);
    M4_TMRA1->BCSTR_f.START = Enable;    
}

void Play_DmaConfig(void)
{
	uint8_t i;
    stc_dma_config_t stcDmaCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    MEM_ZERO_STRUCT(stcDmaCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    
    /* Set data block size. */
    stcDmaCfg.u16BlockSize = 1;  //uint16
    /* Set transfer count. */
    stcDmaCfg.u16TransferCnt = 1;
    /* Set source & destination address. */
//    stcDmaCfg.u32SrcAddr = (uint32_t)(&au16PixieDustSoundI2s_8[0]);
//    stcDmaCfg.u32DesAddr = (uint32_t)(&I2S_CH->TXBUF);

//    /* Disable linked list transfer. */
//    stcDmaCfg.stcDmaChCfg.enLlpEn = Disable;
//    /* Enable repeat function. */
//    stcDmaCfg.stcDmaChCfg.enSrcRptEn = Disable;
//    stcDmaCfg.stcDmaChCfg.enDesRptEn = Disable;
//    /* Set source & destination address mode. */
//    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressIncrease;//AddressFix;
//    stcDmaCfg.stcDmaChCfg.enDesInc = AddressFix;//AddressIncrease;
//    /* Enable interrup. */
//    stcDmaCfg.stcDmaChCfg.enIntEn = Enable;
//    /* Set data width 32bit. */
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma16Bit;
	
	for(i=0;i<MaxLlpnum;i++)
    {
        Tx_stcLlpDesc[i].SARx = (uint32_t)(&au16Rec[!i]);//(uint32_t)(&au16PixieDustSoundI2s_8[i*RECORDER_WAVFILELEN]);//(uint32_t)(au16Rec[i].Data16bit[0]);//(&au16PixieDustSoundI2s_8[i*RECORDER_WAVFILELEN]);
        Tx_stcLlpDesc[i].DARx = (uint32_t)(&I2S_CH->TXBUF);//(uint32_t)&(Spwmdata[1].SPWMx.spwm[0][0]);//
        Tx_stcLlpDesc[i].DTCTLx_f.CNT = RECORDER_WAVFILELEN;//单个链表的大小
        Tx_stcLlpDesc[i].DTCTLx_f.BLKSIZE = 1u;
        if(i==(MaxLlpnum-1))//是否最后一个链表，最后一个链表链接回第一个链表，作为回环
        {
            Tx_stcLlpDesc[i].LLPx = (uint32_t)(&Tx_stcLlpDesc[0]);
        }
        else
        {
            Tx_stcLlpDesc[i].LLPx = (uint32_t)(&Tx_stcLlpDesc[i+1]);
        }
        Tx_stcLlpDesc[i].CHxCTL_f.DRPTEN = Disable;//使能目标地址重置
        Tx_stcLlpDesc[i].CHxCTL_f.SRTPEN = Disable;//取消源地址重置
        Tx_stcLlpDesc[i].RPTx_f.DRPT = 0;
        Tx_stcLlpDesc[i].RPTx_f.SRPT = 0;
        Tx_stcLlpDesc[i].CHxCTL_f.SINC = AddressIncrease;//AddressFix;//源地址自增
        Tx_stcLlpDesc[i].CHxCTL_f.DINC = AddressFix;//AddressIncrease;//目标地址自增
        Tx_stcLlpDesc[i].CHxCTL_f.HSIZE = Dma16Bit;
        Tx_stcLlpDesc[i].CHxCTL_f.LLPEN = Enable;
        Tx_stcLlpDesc[i].CHxCTL_f.LLPRUN = LlpRunNow; 
        
        Tx_stcLlpDesc[i].CHxCTL_f.DNSEQEN = Disable;
        Tx_stcLlpDesc[i].CHxCTL_f.SNSEQEN = Disable;
		Tx_stcLlpDesc[i].CHxCTL_f.IE = Enable;
        Tx_stcLlpDesc[i].DNSEQCTLx_f.DNSCNT = 1;
        Tx_stcLlpDesc[i].DNSEQCTLx_f.DOFFSET = 1;       
    }
	stcDmaCfg.stcDmaChCfg.enLlpEn = Enable; 
    stcDmaCfg.stcDmaChCfg.enLlpMd = LlpRunNow;
    stcDmaCfg.u32DmaLlp = (uint32_t)(&Tx_stcLlpDesc[0]);//指向第一个链表
    /* Enable DMA clock. */
    PWC_Fcg0PeriphClockCmd(TX_DMA_CLK|RX_DMA_CLK,Enable);
	
	PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_AOS,Enable);
    
    /* Enable DMA1. */
    DMA_Cmd(TX_DMA_UNIT,Enable);
    /* Initialize DMA. */
    DMA_InitChannel(TX_DMA_UNIT, TX_DMA_CH, &stcDmaCfg);
	DMA_SetTriggerSrc(TX_DMA_UNIT, TX_DMA_CH, TX_DMA_TRG);
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(TX_DMA_UNIT, TX_DMA_CH,Enable);
    
    DMA_ClearIrqFlag(TX_DMA_UNIT, TX_DMA_CH,TrnCpltIrq);    
}
/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
