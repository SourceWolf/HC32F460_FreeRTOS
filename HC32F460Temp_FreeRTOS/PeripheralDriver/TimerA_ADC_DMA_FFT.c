#include "hc32_ddl.h"
#include "User_Gpio.h"
#include <math.h>
#include <arm_math.h>
#include "User_FFT.h"
#include "System_InterruptCFG_Def.h"

#define FFT_SIZE         256
#define FS               (12800)
#define PI2 6.28318530717959f
#define TMR01_UNIT            M4_TMR01
#define TMR01_INI_GCMA        INT_TMR01_GCMA
#define TMR01_INI_GCMB        INT_TMR01_GCMB
#define TIMER02_B_IRQn          Int007_IRQn
#define TIMER01_B_IRQn          Int000_IRQn
#define DMA_UNIT                (M4_DMA1)
#define DMA_CH                  (DmaCh0)
#define DMA_TRNCNT              FFT_SIZE//传输次数
#define DMA_BLKSIZE             (1u)
#define DMA_RPT_SIZE            (1u)
#define FFT_SIZE_256            FFT_SIZE
#define NPT FFT_SIZE_256//1024点FFT
//#undef  RFFT_MODE
//#define RFFT_MODE RFFT_FAST
uint16_t ADC1_AIN10_Data[DMA_TRNCNT],ADC1_AIN0_Data;
bool flag_ADC1_EOC;
float voltage[DMA_TRNCNT*2],fft_result[DMA_TRNCNT];
volatile float testsignal[DMA_TRNCNT];

void DatatoVoltage(uint16_t* data,float *Voltage);
void Data_FFT(float *sourcedata, float *Result);
void Timer01_Init(void)
{
    stc_tim0_base_init_t stcTimerCfg;
    MEM_ZERO_STRUCT(stcTimerCfg);
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM01,Enable);
    /*config register for channel B */
    stcTimerCfg.Tim0_CounterMode = Tim0_Sync;
    stcTimerCfg.Tim0_SyncClockSource = Tim0_Pclk1;
    stcTimerCfg.Tim0_ClockDivision = Tim0_ClkDiv2;
    stcTimerCfg.Tim0_CmpValue = 3260;
    TIMER0_BaseInit(TMR01_UNIT,Tim0_ChannelB,&stcTimerCfg);

    /* Enable channel B interrupt */
    TIMER0_IntCmd(TMR01_UNIT,Tim0_ChannelB,Enable);
    /*start timer0*/
    TIMER0_Cmd(TMR01_UNIT,Tim0_ChannelB,Enable);
    
}

void ADC_EOCA_CallBack(void)
{
    Test_GPIO();
}
void DMA1_CH0_Callback(void)
{
    TIMER0_Cmd(TMR01_UNIT,Tim0_ChannelB,Disable);
   DMA_ClearIrqFlag(DMA_UNIT,DMA_CH, TrnCpltIrq);
   DMA_SetTransferCnt(DMA_UNIT,DMA_CH,DMA_TRNCNT);
   DMA_SetDesAddress(DMA_UNIT,DMA_CH,(uint32_t)(ADC1_AIN10_Data));
   DMA_ChannelCmd(DMA_UNIT, DMA_CH,Enable); 
   PORT_Toggle(LED1_PORT,LED1_Pin);
   flag_ADC1_EOC = true;
}

void User_DMA_Init(void)
{
    stc_dma_config_t stcDmaCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    MEM_ZERO_STRUCT(stcDmaCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    
    stcDmaCfg.u16BlockSize = DMA_BLKSIZE;//
    stcDmaCfg.u16TransferCnt = DMA_TRNCNT;//
    
    stcDmaCfg.u32DesAddr = (uint32_t)(ADC1_AIN10_Data);//(&DMA0_Dre_Data[0]);//Target Address
    stcDmaCfg.u32SrcAddr = (uint32_t)(&(M4_ADC1->DR10));//USART2_DR_ADDRESS;//(uint32_t)(&DMA0_Src_data[0]);//Source Address
    
    /* Set repeat size. */
    stcDmaCfg.u16SrcRptSize = DMA_RPT_SIZE;
    stcDmaCfg.u16DesRptSize = DMA_RPT_SIZE;

    /* Disable linked list transfer. */
    stcDmaCfg.stcDmaChCfg.enLlpEn = Disable;     
    /* Enable repeat function. */
    stcDmaCfg.stcDmaChCfg.enSrcRptEn = Disable;
    stcDmaCfg.stcDmaChCfg.enDesRptEn = Disable;   
    /* Set source & destination address mode. */
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;//地址不变
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressIncrease;
    /* Enable interrup. */
    stcDmaCfg.stcDmaChCfg.enIntEn = Enable;
    /* Set data width 32bit. */
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma16Bit;

//    M4_MSTP->FCG0PC = 0xA5A50001;
//    M4_MSTP->FCG0_f.DMA1 = Reset;
//    M4_MSTP->FCG0PC = 0xA5A50000;
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA1, Enable);
    /* Enable DMA1. */
    DMA_Cmd(DMA_UNIT,Enable);   
    /* Initialize DMA. */
    DMA_InitChannel(DMA_UNIT, DMA_CH, &stcDmaCfg);
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(DMA_UNIT, DMA_CH,Enable);
    /* Clear DMA transfer complete interrupt flag. */
    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH,TrnCpltIrq);
    
    stcIrqRegiConf.enIntSrc = INT_DMA1_TC0;
    stcIrqRegiConf.enIRQn = DMA1_CH0_IRQn;
    stcIrqRegiConf.pfnCallback =  DMA1_CH0_Callback;   
    
    enIrqRegistration(&stcIrqRegiConf);
	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//Enable Interrupt

    
    /* Enable PTDIS(AOS) clock*/
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS,Enable);
    
    DMA_SetTriggerSrc(DMA_UNIT,DMA_CH,EVT_ADC1_EOCA);
       
//    M4_AOS->INT_SFTTRG_f.STRG = 1;
//    
//    while(Set != DMA_GetIrqFlag(DMA_UNIT,DMA_CH, TrnCpltIrq))
//    {
//        M4_AOS->INT_SFTTRG_f.STRG = 1;
//    }
}
void User_ADC_Init(void)
{
	uint8_t au8Adc1SaSampTime = 0x60;
	stc_adc_init_t stcAdcInit;
    stc_adc_ch_cfg_t  stcAdcBaseCFG;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t Port_CFG;
//    stc_adc_pga_cfg_t ADC_PGA_CFG;
    stc_adc_trg_cfg_t ADC_TRG_CFG;
    
    MEM_ZERO_STRUCT(stcAdcInit);
    MEM_ZERO_STRUCT(stcAdcBaseCFG);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(Port_CFG);
//    MEM_ZERO_STRUCT(ADC_PGA_CFG);
    MEM_ZERO_STRUCT(ADC_TRG_CFG);
    
//    CLK_SetPeriClkSource(ClkAdcSrcMpllp);//MPLLP 3分频56MHz
    PWC_Fcg3PeriphClockCmd(PWC_FCG3_PERIPH_ADC1 | PWC_FCG3_PERIPH_ADC2, Enable);
    stcAdcInit.enScanMode = AdcMode_SAOnce;
    stcAdcInit.enDataAlign = AdcDataAlign_Right;
    stcAdcInit.enResolution = AdcResolution_12Bit;
    stcAdcInit.enAutoClear = AdcClren_Disable;
//    stcAdcInit.enAverageCount = AdcAvcnt_4;
    
    
    ADC_Init(M4_ADC1, &stcAdcInit);//配置ADC
//    ADC_Init(M4_ADC2, &stcAdcInit);

//    ADC_PGA_CFG.enCtl = AdcPgaCtl_Invalid;
//    ADC_ConfigPga(&ADC_PGA_CFG);
    ADC_PgaCmd(Enable);
    ADC_ConfigPga(AdcPgaFactor_2,AdcPgaNegative_VSSA);
    
    Port_CFG.enPinMode = Pin_Mode_Ana;
    PORT_Init(PortC, Pin00, &Port_CFG);//config PC00 As ADC_IN10
    PORT_Init(PortA, Pin00, &Port_CFG);//config PA0 AS ADC_IN0
    stcAdcBaseCFG.u32Channel = ADC1_CH10;//|ADC1_CH0;
//    stcAdcBaseCFG.enAvgEnable = true;
//    stcAdcBaseCFG.pu8SampTime = &au8Adc1SaSampTime;
    stcAdcBaseCFG.u8Sequence = ADC_SEQ_A;//Must be setting, Default can nog convert data
    ADC_AddAdcChannel(M4_ADC1, &stcAdcBaseCFG);

    
	M4_ADC1->ISR_f.EOCAF  = 0;
    M4_ADC1->ICR_f.EOCAIEN = 1;
    
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS,Enable);
    
    ADC_TRG_CFG.enInTrg0 = EVT_TMR01_GCMB;//Timer01 CHB 溢出作为ADC转换触发条件
    ADC_TRG_CFG.enTrgSel = AdcTrgsel_TRGX0;
    ADC_TRG_CFG.u8Sequence = ADC_SEQ_A;
    ADC_ConfigTriggerSrc(M4_ADC1,&ADC_TRG_CFG);
    ADC_TriggerSrcCmd(M4_ADC1,ADC_SEQ_A,Enable);
    
    stcIrqRegiConf.enIntSrc = INT_ADC1_EOCA;    
	stcIrqRegiConf.pfnCallback = ADC_EOCA_CallBack;
	stcIrqRegiConf.enIRQn = ADC1_EOCA_IRQn;
	enIrqRegistration(&stcIrqRegiConf);
	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//Enable Interrupt
    
    Timer01_Init();
    User_DMA_Init();
	
}
void Testsignal_init(void)
{
    int i;
    for(i=0;i<FFT_SIZE;i++)
    {
        testsignal[i] = 100*arm_sin_f32(PI2*i*500.0/FS) + 300*arm_sin_f32(PI2*i*300.0/FS)  + 1000*arm_sin_f32(PI2*i*1000/FS);
        fft_result[i] = 0;
    }
}
void DatatoVoltage(uint16_t* data,float *Voltage)
{
    int i;
    for(i=0;i<FFT_SIZE;i++)
    {
        Voltage[2*i] = ((float)data[i]/4096)*3.3 - 1.5;//计算电压数据，并减去直流分量，实验用的信号发生器，峰峰值3V，直流分量1.5V，没有负电压
        Voltage[2*i+1] = 0;
    }
}

void FFT_Test(void)
{
   int i;
//    Testsignal_init();
   DatatoVoltage(ADC1_AIN10_Data,voltage);
   fft_test(voltage,fft_result);
    printf("%f",-3.345);
    printf("FFT:\r\n");
   for(i=0;i<FFT_SIZE;i++)
   {
       printf("%f\r\n",fft_result[i]);
   }
//   fft_test(testsignal,fft_result);
//   printf("Testsignal:\r\n");
//   for(i=0;i<FFT_SIZE;i++)
//   {
//       printf("%f\r\n",fft_result[i]/NPT);
//   }
}

void Test_FFT_Function(void)
{      
    if(flag_ADC1_EOC)
        {
            flag_ADC1_EOC = false;
            FFT_Test();
            TIMER0_Cmd(M4_TMR01,Tim0_ChannelB,Enable);
        }    
}
