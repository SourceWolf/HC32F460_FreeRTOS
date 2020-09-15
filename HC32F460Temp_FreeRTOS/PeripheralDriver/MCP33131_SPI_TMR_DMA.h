#ifndef MCP33131_H
#define MCP33131_H
#define CFG_CHANGE		(0x0001<<13)

#define INCC_GND		(0x0007<<10)
#define INCC_COM		(0x0006<<10)
#define INCC_TEMP		(0x0003<<10)

#define	CHANNEL0		(0x0000<<7)
#define	CHANNEL1		(0x0001<<7)
#define	CHANNEL2		(0x0002<<7)
#define	CHANNEL3		(0x0003<<7)
#define	CHANNEL4		(0x0004<<7)
#define	CHANNEL5		(0x0005<<7)
#define	CHANNEL6		(0x0006<<7)
#define	CHANNEL7		(0x0007<<7)

#define	BW_FULL			(0x0001<<6)

#define	REF_2V5				(0<<3)
#define	REF_4096			(1<<3)
#define	REF_EXT_TEN			(2<<3)
#define	REF_EXT_BUF_TEN		(3<<3)
#define	REF_EXT_TDIS		(6<<3)
#define	REF_EXT_BUF_TDIS	(7<<3)

#define	SEQ_DIS					(0<<1)
#define	SEQ_SCANWhileUpdateCFG	(1<<1)
#define	SEQ_SCANPassAndT		(2<<1)
#define	SEQ_SCANPass			(3<<1)

#define	NOTReadBackCFG				(1)
//------------------------end-------------------------------//

#define ADCH0	0
#define ADCH1	1
#define ADCH2	2
#define ADCH3	3
#define ADCH4	4
#define ADCH5	5
#define ADCH6	6
#define ADCH7	7
#define ADTEMP	8
void Hw_TimerA3_Init(void);
void Hw_SPI3_Init(void);
void Hw_SPI3_DMA_RX_Init(void);
void Hw_SPI3_DMA_TX_Init(void);
#endif
