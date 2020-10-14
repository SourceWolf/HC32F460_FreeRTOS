/*-----------------------------------------------------------------------------------------------
 *-----------------------------------------------------------------------------------------------
 *
 *									����뵼�����޹�˾HDSC
 *							          
 *
 *-----------------------------------------------------------------------------------------------

 *-----------------------------------------------------------------------------------------------
 * �� �� ��:ŷ����
 * ��	 ��:
 * ��	 ��:����AT24C02EEPROM��д
 * оƬƽ̨�� HC32F460
 *-----------------------------------------------------------------------------------------------
 *-----------------------------------------------------------------------------------------------
 */ 

#include "bsp_i2c_gpio.h"

volatile unsigned char eedata[5];
volatile unsigned char eedata_write[5];
volatile unsigned char I2C_Read_Data[2];
/******************************************************************************
������Set_SCL_DIR()
���ܣ�����SCLΪ���
˵����
******************************************************************************/
void Set_SCL_DIR(void)
{
    PORT_Unlock();
	SCL_DIR = 1;//ʱ��������Ϊ���
    PORT_Lock();
}

/******************************************************************************
������Set_SDA_DIR()
���ܣ�����SDA�˿��䷽��
˵����1Ϊ�����0Ϊ����
******************************************************************************/
void Set_SDA_DIR(unsigned char dir)
{
    PORT_Unlock();
	if(dir)
		{
			SDA_DIR = 1;//DDRX |= 0xXX;//����������Ϊ���
		}
	else
		{
			SDA_DIR = 0;//DDRX &= ~0xXX;//����������Ϊ����
		}
	PORT_Lock();
}
/******************************************************************************
������Set_SCL_pin()
���ܣ�����SDA�˿��������
˵����
******************************************************************************/
void Set_SCL_pin(bool value)
{
    SCL_Pin_out = value;
//	if(value)
//		{
//			PORTA |= (0x01<<4);//PORTX |= 0xXX;//����������Ϊ���
//		}
//	else
//		{
//			PORTA &= ~(0x01<<4);//PORTX &= ~0xXX;//����������Ϊ����
//		}
	
}
/******************************************************************************
������Set_SDA_pin()
���ܣ�����SDA�˿��������
˵����
******************************************************************************/
void Set_SDA_pin(bool value)
{
    SDA_Pin_out = value;
//	if(value)
//		{
//			PORTA |= (0x01<<6);//PORTX |= 0xXX;//����������Ϊ���
//		}
//	else
//		{
//			PORTA &= ~(0x01<<6);//PORTX &= ~0xXX;//����������Ϊ����
//		}
	
}
/******************************************************************************
������Get_SDA_pin()
���ܣ�����SDA�˿��������
˵����
******************************************************************************/
unsigned char Get_SDA_pin(void)
{
	if(SDA_Pin_in)
	{
		return 1;
	}
	else
	{
		return 0;
	}		
}
/******************************************************************************
������NOP10_Delay()
���ܣ���ʱ
˵����
******************************************************************************/
void NOP10_Delay(void)
{
    Ddl_Delay1us(2);
//	asm("Nop");asm("Nop");asm("Nop");asm("Nop");asm("Nop");asm("Nop");
//	asm("Nop");asm("Nop");asm("Nop");asm("Nop");asm("Nop");asm("Nop");
	//asm("Nop");asm("Nop");asm("Nop");asm("Nop");asm("Nop");asm("Nop");
	//asm("Nop");asm("Nop");asm("Nop");asm("Nop");asm("Nop");asm("Nop");
}

/******************************************************************************
������Gpio_I2C_Init()
���ܣ�I2C���߳�ʼ����ʹ���ߴ��ڿ���״̬
˵����ʹ��ģ��I2Cָ��ǰ��Ӧ��ִ��һ�α�����
******************************************************************************/
void Gpio_I2C_Init(void)
{
     Set_SCL_DIR();
     Set_SDA_DIR(1);

     Set_SCL_pin(1);
     NOP10_Delay ();//6us;
     Set_SDA_pin(1);
     NOP10_Delay ();//6us;
}
/******************************************************************************
������I2C_Start()
���ܣ�����I2C���ߵ���ʼ����
˵����SCL���ڸߵ�ƽ�ڼ䣬��SDA�����½���ʱ����I2C����
       ������Ҳ���������ظ���ʼ����
******************************************************************************/
void I2C_Start(void)
{

     Set_SCL_DIR(); //���
     Set_SDA_DIR(1); //���

     Set_SDA_pin(1);     NOP10_Delay ();//6us;
     Set_SCL_pin(1);     NOP10_Delay ();//6us;
     Set_SDA_pin(0);     NOP10_Delay ();//6us;
     Set_SCL_pin(0);     NOP10_Delay ();//6us;
}
///******************************************************************************
//������void I2C_Write(unsigned char dat)
//���ܣ���I2C����д1���ֽڵ�����
//������dat��Ҫд�������ϵ����� 
//******************************************************************************/
unsigned char I2C_Write(unsigned char dat)
{
	unsigned char  Ack;
    unsigned char b = 8;
    volatile unsigned char i2c_fault;
      Set_SDA_DIR(1);//���
     do
     {
         if(dat & 0x80) Set_SDA_pin(1);
         else   Set_SDA_pin(0);     
         dat = dat<<1;
         NOP10_Delay ();//6us;
         Set_SCL_pin(1);     NOP10_Delay ();//6us;
         Set_SCL_pin(0);     NOP10_Delay ();//6us;
     } while( --b != 0 );
     Ack = I2C_GetAck();
	 if(Ack==1)
	 {
		 i2c_fault =1;//NAK
	 }
	 else i2c_fault =0;
     return i2c_fault;
}
///******************************************************************************
//������unsigned char I2C_Read()
//���ܣ��ӻ���ȡ1���ֽڵ�����
//���أ���ȡ��1���ֽ�����
//******************************************************************************/
unsigned char I2C_Read(unsigned char  Ack)
{
     unsigned char dat = 0;
     unsigned char t = 8;
	 unsigned char Bits;
     Set_SDA_DIR(1);
	 Set_SDA_pin(1);
     Set_SDA_DIR(0); //�ڶ�ȡ����֮ǰ��Ҫ��SDA���ߣ�ʹ֮��������״̬

     do
     {
//	     SdaPin = 1;
         Set_SCL_pin(1);    NOP10_Delay ();//6us;
         dat <<= 1;
		 Bits = Get_SDA_pin();NOP10_Delay ();//6us;
         if (Bits) dat++;
         Set_SCL_pin(0);     NOP10_Delay ();//6us;
     } while ( --t != 0 );
  
 //#########����Ӧ��ӻ�##################

     Set_SDA_DIR(1); //���

     if(Ack)
		{ Set_SDA_pin(0);}
     else   
        {Set_SDA_pin(1);}
     NOP10_Delay ();//6us;
     Set_SCL_pin(1);     NOP10_Delay ();//6us;
     Set_SCL_pin(0);     NOP10_Delay ();//6us;
 //#######################################/
     return dat;
}
///******************************************************************************
//������unsigned char  I2C_GetAck()
//���ܣ���ȡ�ӻ�Ӧ��λ��Ӧ����Ӧ�𣩣������жϣ��ӻ��Ƿ�ɹ�������������
//���أ�0���ӻ�Ӧ��
//       1���ӻ���Ӧ��
//˵�����ӻ����յ�ÿһ���ֽں�Ҫ����Ӧ��λ����������յ���Ӧ����Ӧ����ֹ����
//******************************************************************************/
unsigned char  I2C_GetAck(void)
{
     unsigned char Ack;
    
     Set_SDA_DIR(0); //����
	 Set_SDA_pin(1);//����
     NOP10_Delay ();//6us;
     Set_SCL_pin(1);     NOP10_Delay ();//6us;
     Ack = Get_SDA_pin();NOP10_Delay ();
     Set_SCL_pin(0);     NOP10_Delay ();//6us;

     Set_SDA_DIR(1); //���
     return Ack;
}
/******************************************************************************
������void I2C_PutAck(unsigned char  Ack)
���ܣ���������Ӧ��λ��Ӧ����Ӧ�𣩣�����֪ͨ�ӻ��������Ƿ�ɹ����մӻ�����
������Ack = 0������Ӧ��
       Ack = 1��������Ӧ��
˵�����������յ�ÿһ���ֽں�Ҫ����Ӧ�����յ����һ���ֽ�ʱ��Ӧ��������Ӧ��
******************************************************************************/
void I2C_PutAck(unsigned char  Ack)
{
     
     Set_SDA_DIR(1); //���
 
     if(Ack) Set_SDA_pin(1);
     else   Set_SDA_pin(0);
                      NOP10_Delay ();//6us;
     Set_SCL_pin(1);  NOP10_Delay ();//6us;
     Set_SCL_pin(0);  NOP10_Delay ();//6us;
}
/******************************************************************************
������I2C_Stop()
���ܣ�����I2C���ߵ�ֹͣ����
˵����SCL���ڸߵ�ƽ�ڼ䣬��SDA����������ʱֹͣI2C����
******************************************************************************/
void I2C_Stop(void)
{
     Set_SDA_pin(0);     NOP10_Delay ();//6us;
     Set_SCL_pin(1);     NOP10_Delay ();//6us;
     Set_SDA_pin(1);     NOP10_Delay ();//6us;
}

/******************************************************************************
������I2C_Send_Command(uint8_t DeviceAddr, uint8_t address, uint8_t *data, uint8_t len)
���ܣ�I2C����
˵����
******************************************************************************/
unsigned char I2C_Send_Command(uint8_t DeviceAddr, uint8_t address, uint8_t *data, uint8_t len)
{
	unsigned char count;
	volatile unsigned char i2c_fault;
	I2C_Start();
	i2c_fault = I2C_Write(DeviceAddr<<1);
	if(i2c_fault)
    {
        I2C_Stop();
        return 1;
    }
//	i2c_fault = I2C_Write(address>>8);
//    if(i2c_fault)
//    {
//        I2C_Stop();
//        return 1;
//    }
	i2c_fault = I2C_Write(address);
    if(i2c_fault)
    {
        I2C_Stop();
        return 1;
    }
	
	//	I2C_master_write(0x03);
	for(count=0;count<len;count++)
	{
		i2c_fault = I2C_Write(data[count]);
        if(i2c_fault)
        {
            I2C_Stop();
            return 1;
        }
	}
	I2C_Stop();
	return 0;

}
/******************************************************************************
������I2C_Read_Command(uint8_t DeviceAddr, uint8_t address, uint8_t *rxbuf, uint8_t len)
���ܣ�I2C����
˵����
******************************************************************************/
unsigned char I2C_Read_Command(uint8_t DeviceAddr, uint8_t address, uint8_t *rxbuf, uint8_t len)
{
    unsigned char count;
    volatile unsigned char i2c_fault;
	I2C_Start();
	
	i2c_fault = I2C_Write(DeviceAddr<<1);
	if(i2c_fault)
    {
        I2C_Stop();
        return 1;
    }
//	i2c_fault = I2C_Write(address>>8);
//    if(i2c_fault)
//    {
//        I2C_Stop();
//        return 1;
//    }
	i2c_fault = I2C_Write(address);
	if(i2c_fault)
    {
        I2C_Stop();
        return 1;
    }
	I2C_Start();
	
	i2c_fault = I2C_Write((DeviceAddr<<1)|0x01);
	if(i2c_fault)
    {
        I2C_Stop();
        return 1;
    }	
	for(count=0;count<(len-1);count++)
	{
		rxbuf[count]=I2C_Read(1);
	}
	rxbuf[count]=I2C_Read(0);	// "0" is no ack. "1" is ack
	
	I2C_Stop();
    return 0;
}
