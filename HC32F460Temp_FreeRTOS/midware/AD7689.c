#include "AD7689.h"
//CFG_CHANGE|INCC_GND|CHANNEL0|REF_EXT_TDIS|SEQ_DIS|NOTReadBackCFG
static void CNV_Callback(void)
{
	;
}
void AD7689_Init(void)
{
	stc_port_init_t Port_CFG;
	stc_exint_config_t stcEitCfg;
	stc_irq_regi_conf_t stcIrqRegiCfg;
	MEM_ZERO_STRUCT(Port_CFG);
	MEM_ZERO_STRUCT(stcEitCfg);
	MEM_ZERO_STRUCT(stcIrqRegiCfg);
	Hw_SPI3_Init();
	Port_CFG.enPinMode = Pin_Mode_Out;
	Port_CFG.enPinOType = Pin_OType_Od;//ODÊä³ö
	Port_CFG.enPullUp = Enable;
	Port_CFG.enExInt = Enable;
	PORT_Init(SPI3_NSS_PORT,SPI3_NSS_PIN,&Port_CFG);
	PORT_ResetBits(SPI3_NSS_PORT,SPI3_NSS_PIN);
	stcEitCfg.enExitCh = ExtiCh01;
    stcEitCfg.enExtiLvl = ExIntFallingEdge;
    stcEitCfg.enFilterEn = Enable;
    stcEitCfg.enFltClk = Pclk3Div64;
    EXINT_Init(&stcEitCfg);
	stcIrqRegiCfg.enIntSrc = INT_PORT_EIRQ3;
    stcIrqRegiCfg.enIRQn = Int000_IRQn;
    stcIrqRegiCfg.pfnCallback =  CNV_Callback;       
    enIrqRegistration(&stcIrqRegiCfg);    
    NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
}
static void waitforConvert(void)
{
	;
}
void AD7689_SOC(void)//start of convert
{
	PORT_SetBits(SPI3_NSS_PORT,SPI3_NSS_PIN);
}
//void 