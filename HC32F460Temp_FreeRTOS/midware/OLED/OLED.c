#include "hc32_ddl.h"
#include "OLED_FRONT.h"
#define VERSION_OLED    1//0旧版，1新版DEMO
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
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
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
	@brief			IIC写入命令
	@param			IIC_Command：写入的命令
	@retval			无
 */
void Write_IIC_Command(unsigned char IIC_Command)
{
    unsigned char temp;	//定义变量
    temp = IIC_Command;
    User_I2C3_Master_Write(OLED_ADDRESS,0x00,&temp,1);
}
/*
	@brief			IIC写入数据
	@param			IIC_Data：数据
	@retval			无
 */
void Write_IIC_Data(unsigned char IIC_Data)
{
    unsigned char temp;	//定义变量
    temp = IIC_Data;
    User_I2C3_Master_Write(OLED_ADDRESS,0x40,&temp,1);
	
}
void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
	if(cmd) 
	  {
       Write_IIC_Data(dat); //写入数据
	  }
	else {
       Write_IIC_Command(dat); //写入命令
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
	@brief			开显示
	@param			无
	@retval			无
 */ 
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //设置OLED电荷泵
	OLED_WR_Byte(0X14,OLED_CMD);  //使能，开
	OLED_WR_Byte(0XAF,OLED_CMD);  //开显示
}


/*
	@brief			关显示
	@param			无
	@retval			无
 */  
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //设置OLED电荷泵
	OLED_WR_Byte(0X10,OLED_CMD);  //失能，关
	OLED_WR_Byte(0XAE,OLED_CMD);  //关显示
}		   			 

/*
	@brief			清屏
	@param			无
	@retval			无
 */	  
void OLED_Clear(void)  
{  
	unsigned char i,n;		    //定义变量
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //从0~7页依次写入
		OLED_WR_Byte (0x00,OLED_CMD);      //列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //列高地址  
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); //写入 0 清屏
	}
}
/*
	@brief			画点
	@param			x：列的起始低地址与起始高地址；0x00~0x0f：设置起始列低地址（在页寻址模式）；0x10~0x1f：设置起始列高地址（在页寻址模式）
					y：起始页地址 0~7
	@retval			无
 */
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);	//写入页地址
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);	//写入列低地址
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD);  //写入列高地址
}   	  

/*
	@brief			显示一个字符
	@param			x：起始列
					y：起始页，SIZE = 16占两页；SIZE = 12占1页
					chr：字符
	@retval			无
 */
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' '; //获取字符的偏移量	
		if(x>Max_Column-1){x=0;y=y+2;} //如果列书超出了范围，就从下2页的第0列开始
		if(SIZE ==16) //字符大小如果为 16 = 8*16
			{
			OLED_Set_Pos(x,y);	//从x y 开始画点
			for(i=0;i<8;i++)  //循环8次 占8列
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA); //找出字符 c 的数组位置，先在第一页把列画完
			OLED_Set_Pos(x,y+1); //页数加1
			for(i=0;i<8;i++)  //循环8次
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA); //把第二页的列数画完
			}
			else {	//字符大小为 6 = 6*8
				OLED_Set_Pos(x,y+1); //一页就可以画完
				for(i=0;i<6;i++) //循环6次 ，占6列
				OLED_WR_Byte(F6x8[c][i],OLED_DATA); //把字符画完
			}
}


/*
	@brief			计算m^n
	@param			m：输入的一个数
					n：输入数的次方
	@retval			result：一个数的n次方
 */
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  


/*
	@brief			在指定的位置，显示一个指定长度大小的数
	@param			x：起始列
					y：起始页
					num：数字
					len：数字的长度
					size：数据的值
	@retval			无
 */		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2)
{         	
	unsigned char t,temp;  //定义变量
	unsigned char enshow=0;		//定义变量
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;//取出输入数的每个位，由高到低
		if(enshow==0&&t<(len-1)) //enshow：是否为第一个数；t<(len-1)：判断是否为最后一个数
		{
			if(temp==0) //如果该数为0 
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ');//显示 0 ；x+(size2/2)*t根据字体大小偏移的列数（8）
				continue; //跳过剩下语句，继续重复循环（避免重复显示）
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0'); //显示一个位；x+(size2/2)*t根据字体大小偏移的列数（8）
	}
} 


/*
	@brief			显示字符串
	@param			x：起始列
					y：起始页
					*chr：第一个字符首地址
	@retval			无
 */
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr)
{
	unsigned char j=0; //定义变量
	while (chr[j]!='\0') //如果不是最后一个字符
	{		
		OLED_ShowChar(x,y,chr[j]); //显示字符
			x+=8; //列数加8 ，一个字符的列数占8
		if(x>120){x=0;y+=2;} //如果x超过128，切换页，从该页的第一列显示
			j++; //下一个字符
	}
}
/*
	@brief			显示中文
	@param			x：起始列；一个字体占16列
					y：起始页；一个字体占两页
					no：字体的序列号
	@retval			无
 */
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)
{      			    
	unsigned char t,adder=0; //定义变量
	OLED_Set_Pos(x,y);	//从 x y 开始画点，先画第一页
    for(t=0;t<16;t++) //循环16次，画第一页的16列
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);//画no在数组位置的第一页16列的点
				adder+=1; //数组地址加1
     }	
		OLED_Set_Pos(x,y+1); //画第二列
    for(t=0;t<16;t++)	//循环16次，画第二页的16列
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);//画no在数组位置的第二页16列的点
				adder+=1;	//数组地址加1
      }					
}
/*
	@brief			OLED初始化函数
	@param			无
	@retval			无
 */				    
void OLED_Init(void)
{

	OLED_I2C3_init();
 
	Ddl_Delay1ms(200);	//延迟，由于单片机上电初始化比OLED快，所以必须加上延迟，等待OLED上电初始化完成

	OLED_WR_Byte(0xAE,OLED_CMD);//关闭显示
	
	OLED_WR_Byte(0x40,OLED_CMD);//---set low column address
	OLED_WR_Byte(0xB0,OLED_CMD);//---set high column address

	OLED_WR_Byte(0xC8,OLED_CMD);//-not offset

	OLED_WR_Byte(0x81,OLED_CMD);//设置对比度
	OLED_WR_Byte(0xff,OLED_CMD);

	OLED_WR_Byte(0xa1,OLED_CMD);//段重定向设置

	OLED_WR_Byte(0xa6,OLED_CMD);//
	
	OLED_WR_Byte(0xa8,OLED_CMD);//设置驱动路数
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
//	OLED_Set_Pos(0,0); 	 //画点
}  

