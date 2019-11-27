#include "hc32_ddl.h"
#include "Hw_Uart1.h"
uint8_t UART1_RXbuff[256];
static void USART1_RX_Callback(void)
{
    uint8_t data;
    data = USART1_UNIT->DR_f.RDR;
    while(USART1_UNIT->SR_f.TC == 0);
    USART_SendData(USART1_UNIT, data);
}
static void USART1_RX_ERROR_Callback(void)
{
    if(USART1_UNIT->SR_f.ORE == 1)
    {
        USART1_UNIT->CR1_f.CORE = 1;       
    }  
    if(USART1_UNIT->SR_f.FE == 1)//帧错误
    {
        USART1_UNIT->CR1_f.CFE = 1;
    }
    if(USART1_UNIT->SR_f.PE == 1)//校验错误
    {
        USART1_UNIT->CR1_f.CPE = 1;
    }
}
void Hw_Uart1_Init(void)
{
  	stc_usart_uart_init_t stcUsartConf;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t Port_CFG;
  	MEM_ZERO_STRUCT(stcUsartConf);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(Port_CFG);
        
	  stcUsartConf.enClkMode = UsartIntClkCkNoOutput;//internal Clk
	  stcUsartConf.enDataLength = UsartDataBits8;//8 bit data
	  stcUsartConf.enDirection = UsartDataLsbFirst;//小端数据
    stcUsartConf.enParity = UsartParityNone;//无奇偶校验
    stcUsartConf.enStopBit = UsartOneStopBit;//1个停止位
	  stcUsartConf.enSampleMode = UsartSamleBit8;
    stcUsartConf.enDetectMode = UsartStartBitFallEdge;//RX起始位为下降沿

    PWC_Fcg1PeriphClockCmd(USART1_CLK,Enable);
    Port_CFG.enPinMode = Pin_Mode_Out;
	PORT_Init(USART1_TX_PORT, USART1_TX_PIN, &Port_CFG);
    Port_CFG.enPinMode = Pin_Mode_In;
    PORT_Init(USART1_TX_PORT, USART1_TX_PIN, &Port_CFG);
    
    PORT_SetFunc(USART1_RX_PORT, USART1_RX_PIN, USART1_RX_FUNC, Disable);
    PORT_SetFunc(USART1_TX_PORT, USART1_TX_PIN, USART1_TX_FUNC, Disable);
    
    USART_UART_Init(USART1_UNIT,&stcUsartConf); //初始化串口 
		
    USART_SetBaudrate(USART1_UNIT, USART1_BAUDRATE);
		
    USART_FuncCmd(USART1_UNIT, UsartTx, Enable);
	USART_FuncCmd(USART1_UNIT, UsartRx, Enable);
    USART_FuncCmd(USART1_UNIT, UsartRxInt, Enable);
    
    stcIrqRegiConf.enIntSrc = USART1_RI_NUM;
    stcIrqRegiConf.enIRQn = USART1_RX_IRQn;
    stcIrqRegiConf.pfnCallback = USART1_RX_Callback;
    enIrqRegistration(&stcIrqRegiConf);//配置中断向量及函数
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_02);
	NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//
    
    stcIrqRegiConf.enIntSrc = USART1_EI_NUM;
    stcIrqRegiConf.enIRQn = USART1_ER_IRQn;
    stcIrqRegiConf.pfnCallback = USART1_RX_ERROR_Callback;
    enIrqRegistration(&stcIrqRegiConf);//配置中断向量及函数
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_03);
	NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//
    
}
void Test_UART1_TX(void)
{
    while(USART1_UNIT->SR_f.TC == 0);
    USART_SendData(USART1_UNIT, 0x55);
}
