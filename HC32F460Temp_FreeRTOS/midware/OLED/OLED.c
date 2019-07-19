#include "hc32_ddl.h"
#include "OLED_FRONT.h"
#define VERSION_OLED    1//0�ɰ棬1�°�DEMO
#if VERSION_OLED
#define OLED_I2C    M4_I2C2
#define OLED_SCL_PORT   PortD
#define OLED_SCL_PIN    Pin00
#define OLED_SDA_PORT   PortD
#define OLED_SDA_PIN    Pin01
#else
#define OLED_I2C    M4_I2C3
#define OLED_SCL_PORT   PortB
#define OLED_SCL_PIN    Pin06
#define OLED_SDA_PORT   PortB
#define OLED_SDA_PIN    Pin07
#endif 
#define OLED_ADDRESS  0x3C//0x78
#define GENERATE_START                  0x00
#define GENERATE_RESTART                0x01
#define TIMEOUT                         ((uint32_t)0x10000)
#define I2C_RET_OK                      0
#define I2C_RET_ERROR                   1

#define OLED_ADDRESS_W                    0x00
#define OLED_ADDRESS_R                    0x01



/*definition--------------------------------------------*/
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����
#define OLED_MODE 0

#define SIZE 16
#define Max_Column	128
#define Max_Row		64 
#define X_WIDTH 	128
#define Y_WIDTH 	64
/**
 ******************************************************************************
 ** \brief  Send start or restart condition
 **
 ** \param  none
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
static uint8_t OLED_StartOrRestart(uint8_t u8Start)
{
    uint32_t u32TimeOut = TIMEOUT;
    
    /* generate start or restart signal */
    if(GENERATE_START == u8Start)
    {
        /* Wait I2C bus idle */
        while(Set == I2C_GetStatus(OLED_I2C, I2C_SR_BUSY))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
      
        I2C_GenerateStart(OLED_I2C , Enable);
    }
    else
    {
        /* Clear start status flag */
        I2C_ClearStatus(OLED_I2C, I2C_CLR_STARTFCLR);
        /* Send restart condition */
        I2C_GenerateReStart(OLED_I2C , Enable);
    }

    /* Judge if start success*/
    u32TimeOut = TIMEOUT;
    while((Reset == I2C_GetStatus(OLED_I2C, I2C_SR_BUSY)) ||
            (Reset == I2C_GetStatus(OLED_I2C, I2C_SR_STARTF)))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
    
    return I2C_RET_OK;
}
/**
 ******************************************************************************
 ** \brief  Send e2prom device address
 **
 ** \param  u16Adr  The slave address
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
static uint8_t OLED_SendAdr(uint8_t u8Adr)
{
    uint32_t u32TimeOut = TIMEOUT;

    /* Wait tx buffer empty */
    while(Reset == I2C_GetStatus(OLED_I2C, I2C_SR_TEMPTYF))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
    
    /* Send I2C address */
    I2C_SendData(OLED_I2C, u8Adr);
   
    if(OLED_ADDRESS_W == (u8Adr & 0x01))
    {
        /* If in master transfer process, Need wait transfer end*/
        uint32_t u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(OLED_I2C, I2C_SR_TENDF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
        
        /* Check ACK */
        u32TimeOut = TIMEOUT;
        while(Set == I2C_GetStatus(OLED_I2C, I2C_SR_NACKDETECTF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
    }
    
    return I2C_RET_OK;
}


/**
 ******************************************************************************
 ** \brief  Send data to e2prom
 **
 ** \param  pTxData  Pointer to the data buffer
 ** \param  u32Size  Data size
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
static uint8_t OLED_WriteData(uint8_t *pTxData, uint32_t u32Size)
{
    uint32_t u32TimeOut = TIMEOUT;
    
    while(u32Size--)
    {
        /* Wait tx buffer empty */
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(OLED_I2C, I2C_SR_TEMPTYF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
        
        /* Send one byte data */
        I2C_SendData(OLED_I2C, *pTxData++);

        /* Wait transfer end*/
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(OLED_I2C, I2C_SR_TENDF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
        
        /* Check ACK */
        u32TimeOut = TIMEOUT;
        while(Set == I2C_GetStatus(OLED_I2C, I2C_SR_NACKDETECTF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
    }
    
    return I2C_RET_OK;
}
/**
 ******************************************************************************
 ** \brief  Write address and receive data from e2prom
 **
 ** \param  u8Adr    Device address and R/W bit
 ** \param  pTxData  Pointer to the data buffer
 ** \param  u32Size  Data size
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
static uint8_t OLED_SendAdrRevData(uint8_t u8Adr, uint8_t *pRxData, uint32_t u32Size)
{
    uint32_t u32TimeOut = TIMEOUT;

    /* Wait tx buffer empty */
    while(Reset == I2C_GetStatus(OLED_I2C, I2C_SR_TEMPTYF))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
    
    for(uint32_t i=0; i<u32Size; i++)
    {
        /* if the last byte receive, need config NACK*/
        if(i == (u32Size - 1))
        {
            I2C_NackConfig(OLED_I2C, Enable);
        }
        
        /* if first byte receive, need send adr*/
        if(0 == i)
        {
            I2C_SendData(OLED_I2C, u8Adr);
        }
    
        /* Wait receive full flag*/
        u32TimeOut = TIMEOUT;
        while(Reset == I2C_GetStatus(OLED_I2C, I2C_SR_RFULLF))
        {
            if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
        }
        
        /* read data from register*/
        *pRxData++ = I2C_ReadData(OLED_I2C);
        
    }
    
    return I2C_RET_OK;  
}

/**
 ******************************************************************************
 ** \brief  General stop condition to e2prom
 **
 ** \param  None
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
uint8_t OLED_Stop(void)
{
    uint32_t u32TimeOut;
    
    /* Wait I2C bus busy */
    u32TimeOut = TIMEOUT;
    while(Reset == I2C_GetStatus(OLED_I2C, I2C_SR_BUSY))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
  
    I2C_GenerateStop(OLED_I2C, Enable);
    
    /* Wait STOPF */
    u32TimeOut = TIMEOUT;
    while(Reset == I2C_GetStatus(OLED_I2C, I2C_SR_STOPF))
    {
        if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
    }
    
    return I2C_RET_OK;
}
/**
 ******************************************************************************
 ** \brief  Write address and receive data from e2prom
 **
 ** \param  DeviceAddr    Device address 0xFE, The last bit R/W ignore;
 ** \param  DataAddr      Data Address
 ** \param  RecData       Pointer to the data buffer
 **
 ** \return Process result
 **         - I2C_RET_ERROR  Send start failed
 **         - I2C_RET_OK     Send start success
 ******************************************************************************/
uint8_t User_I2C3_Master_Write(uint8_t DeviceAddr, uint8_t DataAddr,uint8_t *TxData, uint8_t Data_len)
{
    uint8_t status = I2C_RET_OK;
    status = OLED_StartOrRestart(GENERATE_START);
    if(status != I2C_RET_OK)
        {
            return status;
        }
    status = OLED_SendAdr((DeviceAddr<<1)&0xFE);
    if(status != I2C_RET_OK)
        {
            return status;
        }
    status = OLED_SendAdr(DataAddr);
    if(status != I2C_RET_OK)
        {
            return status;
        }
    status = OLED_WriteData(TxData, Data_len);
    if(status != I2C_RET_OK)
        {
            return status;
        }
    status = OLED_Stop();
    if(status != I2C_RET_OK)
        {
            return status;
        }
    return I2C_RET_OK;
}

uint8_t User_I2C3_Master_Read(uint8_t DeviceAddr, uint8_t DataAddr,uint8_t *RxData, uint8_t Data_len)
{
    uint8_t i,status = I2C_RET_OK;
    uint32_t u32TimeOut = TIMEOUT;
    status = OLED_StartOrRestart(GENERATE_START);
    if(status != I2C_RET_OK)
        {
            return status;
        }
    status = OLED_SendAdr((DeviceAddr<<1)&0xFE);
    if(status != I2C_RET_OK)
        {
            return status;
        }
    status = OLED_SendAdr(DataAddr);
    if(status != I2C_RET_OK)
        {
            return status;
        }
    status = OLED_StartOrRestart(GENERATE_RESTART);
    if(status != I2C_RET_OK)
        {
            return status;
        }
     status = OLED_SendAdr((DeviceAddr<<1)|0x01);
    if(status != I2C_RET_OK)
        {
            return status;
        }
    for(i=0; i<Data_len; i++)
        {
            /* if the last byte receive, need config NACK*/
            if(i == (Data_len - 1))
            {
                I2C_NackConfig(OLED_I2C, Enable);
            }        
            while(Reset == I2C_GetStatus(OLED_I2C, I2C_SR_RFULLF))
                {
                    if(0 == (u32TimeOut--)) return I2C_RET_ERROR;
                }        
            /* read data from register*/
            RxData[i] = I2C_ReadData(OLED_I2C);
        }
     status = OLED_Stop();
    if(status != I2C_RET_OK)
        {
            return status;
        }
    return I2C_RET_OK;
    
}

/*
	@brief			IICд������
	@param			IIC_Command��д�������
	@retval			��
 */
void Write_IIC_Command(unsigned char IIC_Command)
{
    unsigned char temp;	//�������
    temp = IIC_Command;
    User_I2C3_Master_Write(OLED_ADDRESS,0x00,&temp,1);
}
/*
	@brief			IICд������
	@param			IIC_Data������
	@retval			��
 */
void Write_IIC_Data(unsigned char IIC_Data)
{
    unsigned char temp;	//�������
    temp = IIC_Data;
    User_I2C3_Master_Write(OLED_ADDRESS,0x40,&temp,1);
	
}
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
	if(cmd) 
	  {
       Write_IIC_Data(dat); //д������
	  }
	else {
       Write_IIC_Command(dat); //д������
	}
}

void OLED_I2C3_init(void)
{
    stc_i2c_init_t stcI2cInit;
    
    
#if VERSION_OLED
    PORT_SetFunc(OLED_SCL_PORT, OLED_SCL_PIN, Func_I2c2_Scl, Disable);
    PORT_SetFunc(OLED_SDA_PORT, OLED_SDA_PIN, Func_I2c2_Sda, Disable);
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_I2C2,Enable);
#else
    PORT_SetFunc(OLED_SCL_PORT, OLED_SCL_PIN, Func_I2c3_Scl, Disable);
    PORT_SetFunc(OLED_SDA_PORT, OLED_SDA_PIN, Func_I2c3_Sda, Disable);
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_PERIPH_I2C3,Enable);
#endif    
        
    
    I2C_DeInit(OLED_I2C);
    
    MEM_ZERO_STRUCT(stcI2cInit);
    stcI2cInit.enI2cMode = I2cMaster;
    stcI2cInit.u32Baudrate = 200000;
    I2C_Init(OLED_I2C, &stcI2cInit);
    
    I2C_Cmd(OLED_I2C, Enable);  
}
/*
	@brief			����ʾ
	@param			��
	@retval			��
 */ 
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //����OLED��ɱ�
	OLED_WR_Byte(0X14,OLED_CMD);  //ʹ�ܣ���
	OLED_WR_Byte(0XAF,OLED_CMD);  //����ʾ
}


/*
	@brief			����ʾ
	@param			��
	@retval			��
 */  
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //����OLED��ɱ�
	OLED_WR_Byte(0X10,OLED_CMD);  //ʧ�ܣ���
	OLED_WR_Byte(0XAE,OLED_CMD);  //����ʾ
}		   			 

/*
	@brief			����
	@param			��
	@retval			��
 */	  
void OLED_Clear(void)  
{  
	unsigned char i,n;		    //�������
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //��0~7ҳ����д��
		OLED_WR_Byte (0x00,OLED_CMD);      //�е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //�иߵ�ַ  
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); //д�� 0 ����
	}
}
/*
	@brief			����
	@param			x���е���ʼ�͵�ַ����ʼ�ߵ�ַ��0x00~0x0f��������ʼ�е͵�ַ����ҳѰַģʽ����0x10~0x1f��������ʼ�иߵ�ַ����ҳѰַģʽ��
					y����ʼҳ��ַ 0~7
	@retval			��
 */
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);	//д��ҳ��ַ
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);	//д���е͵�ַ
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD);  //д���иߵ�ַ
}   	  

/*
	@brief			��ʾһ���ַ�
	@param			x����ʼ��
					y����ʼҳ��SIZE = 16ռ��ҳ��SIZE = 12ռ1ҳ
					chr���ַ�
	@retval			��
 */
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' '; //��ȡ�ַ���ƫ����	
		if(x>Max_Column-1){x=0;y=y+2;} //������鳬���˷�Χ���ʹ���2ҳ�ĵ�0�п�ʼ
		if(SIZE ==16) //�ַ���С���Ϊ 16 = 8*16
			{
			OLED_Set_Pos(x,y);	//��x y ��ʼ����
			for(i=0;i<8;i++)  //ѭ��8�� ռ8��
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA); //�ҳ��ַ� c ������λ�ã����ڵ�һҳ���л���
			OLED_Set_Pos(x,y+1); //ҳ����1
			for(i=0;i<8;i++)  //ѭ��8��
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA); //�ѵڶ�ҳ����������
			}
			else {	//�ַ���СΪ 6 = 6*8
				OLED_Set_Pos(x,y+1); //һҳ�Ϳ��Ի���
				for(i=0;i<6;i++) //ѭ��6�� ��ռ6��
				OLED_WR_Byte(F6x8[c][i],OLED_DATA); //���ַ�����
			}
}


/*
	@brief			����m^n
	@param			m�������һ����
					n���������Ĵη�
	@retval			result��һ������n�η�
 */
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/*
	@brief			��ָ����λ�ã���ʾһ��ָ�����ȴ�С����
	@param			x����ʼ��
					y����ʼҳ
					num������
					len�����ֵĳ���
					size�����ݵ�ֵ
	@retval			��
 */		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2)
{         	
	unsigned char t,temp;  //�������
	unsigned char enshow=0;		//�������
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;//ȡ����������ÿ��λ���ɸߵ���
		if(enshow==0&&t<(len-1)) //enshow���Ƿ�Ϊ��һ������t<(len-1)���ж��Ƿ�Ϊ���һ����
		{
			if(temp==0) //�������Ϊ0 
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ');//��ʾ 0 ��x+(size2/2)*t���������Сƫ�Ƶ�������8��
				continue; //����ʣ����䣬�����ظ�ѭ���������ظ���ʾ��
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0'); //��ʾһ��λ��x+(size2/2)*t���������Сƫ�Ƶ�������8��
	}
} 


/*
	@brief			��ʾ�ַ���
	@param			x����ʼ��
					y����ʼҳ
					*chr����һ���ַ��׵�ַ
	@retval			��
 */
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned char j=0; //�������
	while (chr[j]!='\0') //����������һ���ַ�
	{		
		OLED_ShowChar(x,y,chr[j]); //��ʾ�ַ�
			x+=8; //������8 ��һ���ַ�������ռ8
		if(x>120){x=0;y+=2;} //���x����128���л�ҳ���Ӹ�ҳ�ĵ�һ����ʾ
			j++; //��һ���ַ�
	}
}
/*
	@brief			��ʾ����
	@param			x����ʼ�У�һ������ռ16��
					y����ʼҳ��һ������ռ��ҳ
					no����������к�
	@retval			��
 */
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)
{      			    
	unsigned char t,adder=0; //�������
	OLED_Set_Pos(x,y);	//�� x y ��ʼ���㣬�Ȼ���һҳ
    for(t=0;t<16;t++) //ѭ��16�Σ�����һҳ��16��
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);//��no������λ�õĵ�һҳ16�еĵ�
				adder+=1; //�����ַ��1
     }	
		OLED_Set_Pos(x,y+1); //���ڶ���
    for(t=0;t<16;t++)	//ѭ��16�Σ����ڶ�ҳ��16��
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);//��no������λ�õĵڶ�ҳ16�еĵ�
				adder+=1;	//�����ַ��1
      }					
}
/*
	@brief			OLED��ʼ������
	@param			��
	@retval			��
 */				    
void OLED_Init(void)
{

	OLED_I2C3_init();
 
	Ddl_Delay1ms(200);	//�ӳ٣����ڵ�Ƭ���ϵ��ʼ����OLED�죬���Ա�������ӳ٣��ȴ�OLED�ϵ��ʼ�����

	OLED_WR_Byte(0xAE,OLED_CMD);//�ر���ʾ
	
	OLED_WR_Byte(0x40,OLED_CMD);//---set low column address
	OLED_WR_Byte(0xB0,OLED_CMD);//---set high column address

	OLED_WR_Byte(0xC8,OLED_CMD);//-not offset

	OLED_WR_Byte(0x81,OLED_CMD);//���öԱȶ�
	OLED_WR_Byte(0xff,OLED_CMD);

	OLED_WR_Byte(0xa1,OLED_CMD);//���ض�������

	OLED_WR_Byte(0xa6,OLED_CMD);//
	
	OLED_WR_Byte(0xa8,OLED_CMD);//��������·��
	OLED_WR_Byte(0x1f,OLED_CMD);
	
	OLED_WR_Byte(0xd3,OLED_CMD);
	OLED_WR_Byte(0x00,OLED_CMD);
	
	OLED_WR_Byte(0xd5,OLED_CMD);
	OLED_WR_Byte(0xf0,OLED_CMD);
	
	OLED_WR_Byte(0xd9,OLED_CMD);
	OLED_WR_Byte(0x22,OLED_CMD);
	
	OLED_WR_Byte(0xda,OLED_CMD);
	OLED_WR_Byte(0x02,OLED_CMD);
	
	OLED_WR_Byte(0xdb,OLED_CMD);
	OLED_WR_Byte(0x49,OLED_CMD);
	
	OLED_WR_Byte(0x8d,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);
	
	OLED_WR_Byte(0xaf,OLED_CMD);
    OLED_Display_On();
    OLED_Clear();
//	OLED_Set_Pos(0,0); 	 //����
}  

