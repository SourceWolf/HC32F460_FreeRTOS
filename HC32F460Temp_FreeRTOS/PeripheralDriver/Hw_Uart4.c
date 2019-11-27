#include "hc32_ddl.h"
#include "Hw_Uart4.h"
uint8_t UART4_RXbuff[256];
static void USART4_RX_Callback(void)
{
    uint8_t data;
    data = USART4_UNIT->DR_f.RDR;
    while(USART4_UNIT->SR_f.TC == 0);
    USART_SendData(USART4_UNIT, data);
}
static void USART4_RX_ERROR_Callback(void)
{
    if(USART4_UNIT->SR_f.ORE == 1)
    {
        USART4_UNIT->CR1_f.CORE = 1;       
    }  
    if(USART4_UNIT->SR_f.FE == 1)//֡����
    {
        USART4_UNIT->CR1_f.CFE = 1;
    }
    if(USART4_UNIT->SR_f.PE == 1)//У�����
    {
        USART4_UNIT->CR1_f.CPE = 1;
    }
}
void Hw_Uart4_Init(void)
{
  	stc_usart_uart_init_t stcUsartConf;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_port_init_t Port_CFG;
  	MEM_ZERO_STRUCT(stcUsartConf);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    MEM_ZERO_STRUCT(Port_CFG);
        
	  stcUsartConf.enClkMode = UsartIntClkCkNoOutput;//internal Clk
	  stcUsartConf.enDataLength = UsartDataBits8;//8 bit data
	  stcUsartConf.enDirection = UsartDataLsbFirst;//С������
    stcUsartConf.enParity = UsartParityNone;//����żУ��
    stcUsartConf.enStopBit = UsartOneStopBit;//1��ֹͣλ
	  stcUsartConf.enSampleMode = UsartSamleBit8;
    stcUsartConf.enDetectMode = UsartStartBitFallEdge;//RX��ʼλΪ�½���

    PWC_Fcg1PeriphClockCmd(USART4_CLK,Enable);
    Port_CFG.enPinMode = Pin_Mode_Out;
	PORT_Init(USART4_TX_PORT, USART4_TX_PIN, &Port_CFG);
    Port_CFG.enPinMode = Pin_Mode_In;
    PORT_Init(USART4_TX_PORT, USART4_TX_PIN, &Port_CFG);
    
    PORT_SetFunc(USART4_RX_PORT, USART4_RX_PIN, USART4_RX_FUNC, Disable);
    PORT_SetFunc(USART4_TX_PORT, USART4_TX_PIN, USART4_TX_FUNC, Disable);
    
    USART_UART_Init(USART4_UNIT,&stcUsartConf); //��ʼ������ 
		
    USART_SetBaudrate(USART4_UNIT, USART4_BAUDRATE);
		
    USART_FuncCmd(USART4_UNIT, UsartTx, Enable);
	USART_FuncCmd(USART4_UNIT, UsartRx, Enable);
    USART_FuncCmd(USART4_UNIT, UsartRxInt, Enable);
    
    stcIrqRegiConf.enIntSrc = USART4_RI_NUM;
    stcIrqRegiConf.enIRQn = USART4_RX_IRQn;
    stcIrqRegiConf.pfnCallback = USART4_RX_Callback;
    enIrqRegistration(&stcIrqRegiConf);//�����ж�����������
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_02);
	NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//
    
    stcIrqRegiConf.enIntSrc = USART4_EI_NUM;
    stcIrqRegiConf.enIRQn = USART4_ER_IRQn;
    stcIrqRegiConf.pfnCallback = USART4_RX_ERROR_Callback;
    enIrqRegistration(&stcIrqRegiConf);//�����ж�����������
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_03);
	NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//
    
}
void Test_UART4_TX(void)
{
    while(USART4_UNIT->SR_f.TC == 0);
    USART_SendData(USART4_UNIT, 0x55);
}
