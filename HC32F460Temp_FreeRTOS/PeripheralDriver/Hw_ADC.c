#include "hc32_ddl.h"

uint16_t ADC1_AIN10_Data;
#define ADC1_EOCA_IRQn          Int001_IRQn
#define ADC2_EOCA_IRQn          Int002_IRQn
void Get_ADC2_Data(uint16_t *data)
{
    *data = M4_ADC1->DR6;
}
void Get_ADC1_Data(uint16_t *data)
{
    *data = M4_ADC1->DR6;
}
void ADC1_EOCA_CallBack(void)
{
    Get_ADC1_Data(&ADC1_AIN10_Data);
}
void ADC2_EOCA_CallBack(void)
{
    Get_ADC1_Data(&ADC1_AIN10_Data);
}
void ADC1_Start_convert(void)
{
    M4_ADC1->STR = 1;
}
void ADC2_Start_convert(void)
{
    M4_ADC2->STR = 1;
}
void Hw_ADC_Init(void)
{
	uint8_t au8Adc1SaSampTime = 0x60;
	stc_adc_init_t stcAdcInit;
    stc_adc_ch_cfg_t  stcAdcBaseCFG;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t Port_CFG;
    
    MEM_ZERO_STRUCT(stcAdcInit);
    MEM_ZERO_STRUCT(stcAdcBaseCFG);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(Port_CFG);
    
//    CLK_SetPeriClkSource(ClkAdcSrcMpllp);//MPLLP 3分频56MHz
    PWC_Fcg3PeriphClockCmd(PWC_FCG3_PERIPH_ADC1 | PWC_FCG3_PERIPH_ADC2, Enable);
    stcAdcInit.enScanMode = AdcMode_SAOnce;
    stcAdcInit.enDataAlign = AdcDataAlign_Right;
    stcAdcInit.enResolution = AdcResolution_12Bit;
    stcAdcInit.enAutoClear = AdcClren_Enable;
//    stcAdcInit.enAverageCount = AdcAvcnt_4;
    
    
    ADC_Init(M4_ADC1, &stcAdcInit);//配置ADC
    ADC_Init(M4_ADC2, &stcAdcInit);
    
//    ADC_PgaCmd(Enable);
//    ADC_ConfigPga(AdcPgaFactor_2,AdcPgaNegative_VSSA);

    Port_CFG.enPinMode = Pin_Mode_Ana;
    PORT_Init(PortA, Pin06, &Port_CFG);//config PA06 As ADC_IN6
    PORT_Init(PortB, Pin01, &Port_CFG);//config PB01 As ADC_IN9
    
    stcAdcBaseCFG.u32Channel = ADC1_CH6;
//    stcAdcBaseCFG.enAvgEnable = true;
    stcAdcBaseCFG.pu8SampTime = &au8Adc1SaSampTime;
    stcAdcBaseCFG.u8Sequence = ADC_SEQ_A;//Must be setting, Default can nog convert data
    ADC_AddAdcChannel(M4_ADC1, &stcAdcBaseCFG);
    
    stcAdcBaseCFG.u32Channel = ADC2_CH5;
    ADC_AddAdcChannel(M4_ADC2, &stcAdcBaseCFG);

    stcIrqRegiConf.enIntSrc = INT_ADC1_EOCA;
	stcIrqRegiConf.pfnCallback = ADC1_EOCA_CallBack;
	stcIrqRegiConf.enIRQn = ADC1_EOCA_IRQn;
	enIrqRegistration(&stcIrqRegiConf);
    
    stcIrqRegiConf.enIntSrc = INT_ADC2_EOCA;
	stcIrqRegiConf.pfnCallback = ADC2_EOCA_CallBack;
	stcIrqRegiConf.enIRQn = ADC2_EOCA_IRQn;
	enIrqRegistration(&stcIrqRegiConf);
    
	M4_ADC1->ISR_f.EOCAF  = 0;
    M4_ADC1->ICR_f.EOCAIEN = 1;
    M4_ADC2->ISR_f.EOCAF  = 0;
    M4_ADC2->ICR_f.EOCAIEN = 1;
    
//	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//Enable Interrupt
	
}

