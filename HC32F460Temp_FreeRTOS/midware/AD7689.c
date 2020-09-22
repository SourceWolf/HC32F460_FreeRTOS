#include "AD7689.h"
//CFG_CHANGE|INCC_GND|CHANNEL0|REF_EXT_TDIS|SEQ_DIS|NOTReadBackCFG
typedef union
{
    uint16_t cfg16;
    uint8_t cfg8[2];
}adc_cfg_t;
typedef union
{
    uint16_t dat16;
    uint8_t dat8[2];
}adc_data_t;
adc_data_t adc_dat;
static adc_cfg_t adc_cfg;
static void CNV_Callback(void)
{
	PORT_ResetBits(SPI3_NSS_PORT,SPI3_NSS_PIN);
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
//    PORT_ResetBits(SPI3_NSS_PORT,SPI3_NSS_PIN);
    adc_cfg.cfg16 = (CFG_CHANGE|INCC_GND|CHANNEL0|REF_EXT_TDIS|SEQ_DIS|NOTReadBackCFG);
    adc_cfg.cfg16 = adc_cfg.cfg16<<2;
//    SPI_SendData8(SPI3_UNIT,adc_cfg.cfg8[0]);
//    SPI_SendData8(SPI3_UNIT,adc_cfg.cfg8[1]);
//    PORT_SetBits(SPI3_NSS_PORT,SPI3_NSS_PIN);
    SPI_TX_8bit(adc_dat.dat8,adc_cfg.cfg8,2);
}
static void waitforConvert(void)
{
	;
}
void ADC_ReadwhileWrite_16bit(uint8_t *rxdata,uint8_t *data)
{
	PORT_ResetBits(SPI3_NSS_PORT,SPI3_NSS_PIN);
	while(SPI3_UNIT->SR_f.TDEF == 0);
	SPI_SendData8(SPI3_UNIT,data[1]);
	while(SPI3_UNIT->SR_f.RDFF == 0);
	rxdata[1] = SPI_ReceiveData8(SPI3_UNIT);
    while(SPI3_UNIT->SR_f.TDEF == 0);
	SPI_SendData8(SPI3_UNIT,data[0]);
	while(SPI3_UNIT->SR_f.RDFF == 0);
	rxdata[0] = SPI_ReceiveData8(SPI3_UNIT);
	while(SPI3_UNIT->SR_f.IDLNF ==1);
	PORT_SetBits(SPI3_NSS_PORT,SPI3_NSS_PIN);
}
void AD7689_SOC(void)//start of convert
{
	PORT_SetBits(SPI3_NSS_PORT,SPI3_NSS_PIN); 
    Ddl_Delay1us(5);
    PORT_ResetBits(SPI3_NSS_PORT,SPI3_NSS_PIN);
    Ddl_Delay1us(300);
    PORT_SetBits(SPI3_NSS_PORT,SPI3_NSS_PIN);
//    Ddl_Delay1ms(1);
    ADC_ReadwhileWrite_16bit(adc_dat.dat8,adc_cfg.cfg8);
}
//void 