#include "hc32_ddl.h"
#include "System_InterruptCFG_Def.h"
#include "Hw_Uart3_DMA.h"
uint8_t UART_RXbuff[UART3_DMA_TRNCNT];
void USART_RX_TimerOut_Callback(void)
{
    USART3_UNIT->CR1_f.CRTOF = 1;
    M4_TMR02->CNTBR_f.CNTB = 0;
//    TIMER0_Cmd(M4_TMR02, Tim0_ChannelA, Disable);
//    USART_CH->DR_f.RDR;
    //--------------------��ʱ��DMA��ַ����-------------------------------//
//    DMA_Cmd(UART3_DMA2_UNIT,Disable);
//    DMA_Cmd(UART3_DMA2_UNIT,Enable); 
    DMA_ChannelCmd(UART3_DMA2_UNIT, UART3_DMA_CH,Disable);
    DMA_ClearIrqFlag(UART3_DMA2_UNIT,UART3_DMA_CH, TrnCpltIrq);
    DMA_SetTransferCnt(UART3_DMA2_UNIT,UART3_DMA_CH,UART3_DMA_TRNCNT);
    DMA_SetDesAddress(UART3_DMA2_UNIT,UART3_DMA_CH,(uint32_t)(UART_RXbuff));
    DMA_ChannelCmd(UART3_DMA2_UNIT, UART3_DMA_CH,Enable);
}
void USART_RX_Callback(void)
{
    uint8_t data;
    data = USART3_UNIT->DR_f.RDR;
}
void USART_RX_ERROR_Callback(void)
{
    if(USART3_UNIT->SR_f.ORE == 1)
    {
        USART3_UNIT->CR1_f.CORE = 1;       
    }  
    if(USART3_UNIT->SR_f.FE == 1)//֡����
    {
        USART3_UNIT->CR1_f.CFE = 1;
    }
    if(USART3_UNIT->SR_f.PE == 1)//У�����
    {
        USART3_UNIT->CR1_f.CPE = 1;
    }
    DMA_ChannelCmd(UART3_DMA2_UNIT, UART3_DMA_CH,Disable);
    DMA_ClearIrqFlag(UART3_DMA2_UNIT,UART3_DMA_CH, TrnCpltIrq);
    DMA_SetTransferCnt(UART3_DMA2_UNIT,UART3_DMA_CH,UART3_DMA_TRNCNT);
    DMA_SetDesAddress(UART3_DMA2_UNIT,UART3_DMA_CH,(uint32_t)(UART_RXbuff));
    DMA_ChannelCmd(UART3_DMA2_UNIT, UART3_DMA_CH,Enable);
}
void hwdmx_uartInit(void)
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

    PWC_Fcg1PeriphClockCmd(USART3_CLK,Enable);
    Port_CFG.enPinMode = Pin_Mode_Out;
	PORT_Init(USART3_TX_PORT, USART3_TX_PIN, &Port_CFG);
    Port_CFG.enPinMode = Pin_Mode_In;
    PORT_Init(USART3_TX_PORT, USART3_TX_PIN, &Port_CFG);
    
    PORT_SetFunc(USART3_RX_PORT, USART3_RX_PIN, USART3_RX_FUNC, Disable);
    PORT_SetFunc(USART3_TX_PORT, USART3_TX_PIN, USART3_TX_FUNC, Disable);
    
//    USART_CH->CR1_f.OVER8 = 1;//˫��������
    USART_UART_Init(USART3_UNIT,&stcUsartConf); //��ʼ������ 
		
    USART_SetBaudrate(USART3_UNIT, 500000);
		
    USART_FuncCmd(USART3_UNIT, UsartTx, Enable);
	USART_FuncCmd(USART3_UNIT, UsartRx, Enable);
    USART_FuncCmd(USART3_UNIT, UsartTimeOut, Enable);
    USART_FuncCmd(USART3_UNIT, UsartTimeOutInt, Enable);
    USART_FuncCmd(USART3_UNIT, UsartRxInt, Enable);
    
    stcIrqRegiConf.enIntSrc = USART3_RI_NUM;
    stcIrqRegiConf.enIRQn = USART3_RX_IRQn;
    stcIrqRegiConf.pfnCallback = USART_RX_Callback;
    enIrqRegistration(&stcIrqRegiConf);//�����ж�����������
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_02);
	NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
//    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//
    
    stcIrqRegiConf.enIntSrc = USART3_EI_NUM;
    stcIrqRegiConf.enIRQn = USART3_ER_IRQn;
    stcIrqRegiConf.pfnCallback = USART_RX_ERROR_Callback;
    enIrqRegistration(&stcIrqRegiConf);//�����ж�����������
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_03);
	NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//
    
    stcIrqRegiConf.enIntSrc = INT_USART3_RTO;
    stcIrqRegiConf.enIRQn = USART3_RTO_IRQn;
    stcIrqRegiConf.pfnCallback = USART_RX_TimerOut_Callback;
    enIrqRegistration(&stcIrqRegiConf);//�����ж�����������
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);
    hw_dma_init();
    UART_RTO_Timer_Init();
}
void Test_UART_TX(void)
{
    while(USART3_UNIT->SR_f.TC == 0);
    USART_SendData(USART3_UNIT, 0x55);
}

void UART_DMA_Callback(void)
{
//    DMA_Cmd(UART3_DMA2_UNIT,Disable);
//    DMA_Cmd(UART3_DMA2_UNIT,Enable); 
//    DMA_ChannelCmd(UART3_DMA2_UNIT, UART3_DMA_CH,Disable);
//    DMA_ClearIrqFlag(UART3_DMA2_UNIT,UART3_DMA_CH, TrnCpltIrq);
//    DMA_SetTransferCnt(UART3_DMA2_UNIT,UART3_DMA_CH,UART3_DMA_TRNCNT);
//    DMA_SetDesAddress(UART3_DMA2_UNIT,UART3_DMA_CH,(uint32_t)(&UART_RXbuff[0]));
//    DMA_ChannelCmd(UART3_DMA2_UNIT, UART3_DMA_CH,Enable);     
    TIMER0_Cmd(M4_TMR02, Tim0_ChannelA, Disable);
    M4_TMR02->CNTBR_f.CNTB = 0;
}
void hw_dma_init(void)
{
    stc_dma_config_t stcDmaCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    MEM_ZERO_STRUCT(stcDmaCfg);
    MEM_ZERO_STRUCT(stcIrqRegiConf);
    
    stcDmaCfg.u16BlockSize = UART3_DMA_BLKSIZE;//
    stcDmaCfg.u16TransferCnt = UART3_DMA_TRNCNT;//
    
    stcDmaCfg.u32DesAddr = (uint32_t)(&UART_RXbuff[0]);//(&DMA0_Dre_Data[0]);//Target Address
    stcDmaCfg.u32SrcAddr = ((uint32_t)(&USART3_UNIT->DR)+2);//USART2_DR_ADDRESS;//(uint32_t)(&DMA0_Src_data[0]);//Source Address
    
    /* Set repeat size. */
    stcDmaCfg.u16SrcRptSize = 0;
    stcDmaCfg.u16DesRptSize = UART3_DMA_TRNCNT;

    /* Disable linked list transfer. */
    stcDmaCfg.stcDmaChCfg.enLlpEn = Disable;     
    /* Enable repeat function. */
    stcDmaCfg.stcDmaChCfg.enSrcRptEn = Disable;
    stcDmaCfg.stcDmaChCfg.enDesRptEn = Disable;   
    /* Set source & destination address mode. */
    stcDmaCfg.stcDmaChCfg.enSrcInc = AddressFix;//��ַ����
    stcDmaCfg.stcDmaChCfg.enDesInc = AddressIncrease;
    /* Enable interrup. */
    stcDmaCfg.stcDmaChCfg.enIntEn = Enable;
    /* Set data width 32bit. */
    stcDmaCfg.stcDmaChCfg.enTrnWidth = Dma8Bit;

    PWC_Fcg0PeriphClockCmd(UART3_DMA_CLK, Enable);
    
    
    /* Enable DMA1. */
    DMA_Cmd(UART3_DMA2_UNIT,Enable);   
    /* Initialize DMA. */
    DMA_InitChannel(UART3_DMA2_UNIT, UART3_DMA_CH, &stcDmaCfg);
    /* Enable DMA1 channel0. */
    DMA_ChannelCmd(UART3_DMA2_UNIT, UART3_DMA_CH,Enable);
    /* Clear DMA transfer complete interrupt flag. */
    DMA_ClearIrqFlag(UART3_DMA2_UNIT, UART3_DMA_CH,TrnCpltIrq);
    
    stcIrqRegiConf.enIntSrc = INT_DMA2_TC0;
    stcIrqRegiConf.enIRQn = DMA2_CH0_IRQn;
    stcIrqRegiConf.pfnCallback =  UART_DMA_Callback;   
    
    enIrqRegistration(&stcIrqRegiConf);
	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//Enable Interrupt

    
    /* Enable PTDIS(AOS) clock*/
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS,Enable);
    
    DMA_SetTriggerSrc(UART3_DMA2_UNIT,UART3_DMA_CH,UART3_DMA_Trg_Src);
    
}
void RTO_Timer_Callback(void)
{
    TIMER0_Cmd(M4_TMR02, Tim0_ChannelA, Disable);
}
void UART_RTO_Timer_Init(void)
{
	stc_tim0_base_init_t stcTimerCfg;
    stc_irq_regi_conf_t stcIrqRegiConf;
    stc_tim0_trigger_init_t Triger_Init;
	
	MEM_ZERO_STRUCT(stcTimerCfg);
	MEM_ZERO_STRUCT(stcIrqRegiConf);
	
	/*Write-off protection*/
    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM02,Enable);//ʱ��ʹ��
    
    Triger_Init.Tim0_InTrigClear = Enable;
    Triger_Init.Tim0_InTrigStart = Enable;
    TIMER0_HardTriggerInit(M4_TMR02, Tim0_ChannelA, &Triger_Init);//��������RTO����
	
	stcTimerCfg.Tim0_CounterMode = Tim0_Sync;//ͬ������
	stcTimerCfg.Tim0_SyncClockSource = Tim0_Pclk1;//ʱ��ԴPCLK1
	stcTimerCfg.Tim0_ClockDivision = Tim0_ClkDiv1024;//512��Ƶ/////---------------------------UART4 ���ڳ�ʱʱ����Timer0 CHB����ʱ�������
	stcTimerCfg.Tim0_CmpValue = 65535-1;
    
    TIMER0_BaseInit(M4_TMR02, Tim0_ChannelA, &stcTimerCfg);
    TIMER0_IntCmd(M4_TMR02,Tim0_ChannelA,Enable);//TIMER0_IntCmd(M4_TMR01,Tim0_ChannelA,Enable);
	
	stcIrqRegiConf.enIntSrc = INT_TMR02_GCMA;//INT_TMR01_GCMA
	stcIrqRegiConf.pfnCallback = RTO_Timer_Callback;
	stcIrqRegiConf.enIRQn = TIMER02_CHA_IRQn;
	enIrqRegistration(&stcIrqRegiConf);
	
	NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);//ʹ���ж�
	
//    TIMER0_Cmd(M4_TMR02, Tim0_ChannelA, Enable);
}
