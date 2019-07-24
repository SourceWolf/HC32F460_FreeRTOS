#ifndef UART_DMA_H
#define UART_DMA_H
#include "hc32_ddl.h"

#define USART_CH   M4_USART3

#define STATUS_UART_OT     0x02
#define STATUS_UART_RX      0x01


#define USART_BAUDRATE      (56000)

#define USART_RX_PORT       PortE
#define USART_RX_PIN        Pin04
#define USART_RX_FUNC       Func_Usart3_Rx

#define USART_TX_PORT       PortE
#define USART_TX_PIN        Pin05
#define USART_TX_FUNC       Func_Usart3_Tx

#define USART_RI_NUM        INT_USART3_RI
#define USART_EI_NUM        INT_USART3_EI
#define USART_TI_NUM        INT_USART3_TI

#define USART_CLK           PWC_FCG1_PERIPH_USART3

#define USART_RX_IRQn       Int022_IRQn
#define USART_ER_IRQn       Int023_IRQn
#define USART_RTO_IRQn     Int024_IRQn
#define TIMER02_CHA_IRQn    Int025_IRQn

#define UART3_DMA2_UNIT                (M4_DMA2)
#define UART3_DMA_CH                  (DmaCh0)
#define UART3_DMA_CLK                 PWC_FCG0_PERIPH_DMA2
#define UART3_DMA_TRNCNT              (74)//�������
#define DMA_BLKSIZE             (1u)
#define UART3_DMA_RPT_SIZE            UART3_DMA_TRNCNT
#define DMA_INT_SRC             INT_DMA2_BTC0
#define DMA_Trg_Src             EVT_USART3_RI
//#define DMA2_CH0_IRQn           Int009_IRQn

extern volatile uint8_t flag_Uart_status;
extern uint8_t UART_RXbuff[UART3_DMA_TRNCNT];
void hw_dma_init(void);
void hwdmx_uartInit(void);
void UART_RTO_Timer_Init(void);
void Test_UART_TX(void);

#endif

