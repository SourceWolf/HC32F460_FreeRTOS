#include "NRF24L01.h"
#include "User_SPI.h"
#define NRF_CE_PORT	PortC
#define NRF_CE_Pin	Pin13
#define NRF_IRQ_PORT	PortH
#define NRF_IRQ_Pin	Pin02
#define NRF_CS_PORT	PortE
#define NRF_CS_Pin	Pin01
void NRF24L01_Init(void)
{
	stc_port_init_t Port_CFG;
	MEM_ZERO_STRUCT(Port_CFG);
	Port_CFG.enPinMode = Pin_Mode_Out;
	PORT_Init(NRF_CE_PORT, NRF_CE_Pin, &Port_CFG);
	PORT_Init(NRF_CS_PORT, NRF_CS_Pin, &Port_CFG);
	Port_CFG.enPinMode = Pin_Mode_In;
	PORT_Init(NRF_IRQ_PORT, NRF_IRQ_Pin, &Port_CFG);
	User_SPI_Init();
}
void NRF_CS_Set(void)
{
	PORT_SetBits(NRF_CS_PORT, NRF_CS_Pin);
}
void NRF_CS_Clr(void)
{
	PORT_ResetBits(NRF_CS_PORT, NRF_CS_Pin);
}
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,u8_ctr;	    
 	NRF_CS_Clr();          //ʹ��SPI����
  	status = SPIx_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(u8_ctr=0; u8_ctr<len; u8_ctr++)SPIx_ReadWriteByte(*pBuf++); //д������	 
  	NRF_CS_Set();       //�ر�SPI����
  	return status;          //���ض�����״ֵ̬
}
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
	uint8_t status,u8_ctr;	       
  	NRF_CS_Clr();           //ʹ��SPI����
  	status=SPIx_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)
	{
		pBuf[u8_ctr]=SPIx_ReadWriteByte(0XFF);
	}//��������
  	NRF_CS_Set();        //�ر�SPI����
  	return status;        //���ض�����״ֵ̬
}
void NRF24L01_Read_Reg(uint8_t reg, uint8_t *buf, uint8_t len)
{
	NRF_CS_Clr();
	SPI_Writedata(reg);
	SPI_Writedata(0xF0);
	buf[0] = SPI_ReadData();
	NRF_CS_Set();
}
bool NRF24L01_check(void)
{
	uint8_t buf[5];
	uint8_t i;
	for(i = 0; i<5; i++)
	{
		buf[i] = 0xA5;
	}
	NRF24L01_Write_Buf(WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf[i]!=0XA5)break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
	
}
