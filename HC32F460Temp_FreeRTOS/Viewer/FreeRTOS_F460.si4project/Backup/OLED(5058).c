#include "hc32_ddl.h"
#include "OLED_FRONT.h"
#include "Hw_I2C.h"
#include "OLED.h"
#include <math.h>
#include <arm_math.h>
#define VERSION_OLED    1//0旧版，1新版DEMO
#define MAX_Y_POS 31
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
#define I2C_Baudrate 400000
#define OLED_ADDRESS  0x3C//0x78
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
uint8_t display_data[4][128];

/*
	@brief			IIC写入命令
	@param			IIC_Command：写入的命令
	@retval			无
 */
void Write_IIC_Command(unsigned char IIC_Command)
{
	en_result_t status;
	uint8_t trytime = 0;
    unsigned char temp;	//定义变量
    temp = IIC_Command;
	do
	{
		trytime++;
		status = I2C_Write_data(OLED_I2C,OLED_ADDRESS,0x00,&temp,1);
		if(status != I2C_RET_OK)
		{
			Ddl_Delay1us(10);
		}
		else
		{
			break;
		}
	}while(trytime<3);	
}
/*
	@brief			IIC写入数据
	@param			IIC_Data：数据
	@retval			无
 */
void Write_IIC_Data(unsigned char IIC_Data)
{
	en_result_t status;
	uint8_t trytime = 0;
    unsigned char temp;	//定义变量
    temp = IIC_Data;
	do
	{
		trytime++;
		status = I2C_Write_data(OLED_I2C,OLED_ADDRESS,0x40,&temp,1);
		if(status != I2C_RET_OK)
		{
			Ddl_Delay1us(10);
		}
		else
		{
			break;
		}
	}while(trytime<3);	
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
#if VERSION_OLED
    PORT_SetFunc(OLED_SCL_PORT, OLED_SCL_PIN, Func_I2c2_Scl, Disable);
    PORT_SetFunc(OLED_SDA_PORT, OLED_SDA_PIN, Func_I2c2_Sda, Disable);
#else
    PORT_SetFunc(OLED_SCL_PORT, OLED_SCL_PIN, Func_I2c3_Scl, Disable);
    PORT_SetFunc(OLED_SDA_PORT, OLED_SDA_PIN, Func_I2c3_Sda, Disable);
#endif    
    HW_I2C_Init(OLED_I2C,I2C_Baudrate);             
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
void OLED_Refresh(void)
{
	unsigned char i,n;	
	for(i=0;i<4;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //从0~7页依次写入
		OLED_WR_Byte (0x00,OLED_CMD);      //列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //列高地址  
		for(n=0;n<128;n++)OLED_WR_Byte(display_data[i][n],OLED_DATA); //写入 0 清屏
	}
}
void OLED_Clear(void)  
{  
	unsigned char i,n;		    //定义变量
	for(i=0;i<4;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //从0~7页依次写入
		OLED_WR_Byte (0x00,OLED_CMD);      //列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //列高地址  
		for(n=0;n<128;n++) //写入 0 清屏
		{
			OLED_WR_Byte(0,OLED_DATA);
			display_data[i][n] = 0;
		}		
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
void OLED_Set_Point(unsigned char x,unsigned char y)
{
	volatile uint8_t temp,line;
	if(y>MAX_Y_POS)//超过屏高
	{
		return;
	}
	line = 3 - (y>>3);//行号
	temp = y-(3-line)*8;//byte位置
	temp = 0x80>>temp;
	display_data[line][x] |= temp;
}
void OLED_Clr_Point(unsigned char x,unsigned char y)
{
	volatile uint8_t temp,line;
	if(y>MAX_Y_POS)//超过屏高
	{
		return;
	}
	line = 3 - (y>>3);//行号
	temp = y-(3-line)*8;//byte位置
	temp = 0x80>>temp;
	display_data[line][x] &= ~temp;
}
void OLED_Draw_line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2)
{
	volatile unsigned char i,dx,dy;
	if(x2>x1)
	{
		dx = x2-x1;
		if(y2>y1)
		{
			dy = y2-y1;
			if(dx>dy)
			{
				for(i=x1;i<=x2;i++)
				{
					OLED_Set_Point(i,y1+(i-x1)*dy/dx);
				}
			}
			else
			{
				for(i=y1;i<=y2;i++)
				{
					OLED_Set_Point(x1+(i-y1)*dx/dy,i);
				}
			}	
		}
		else
		{
			dy = y1-y2;
			if(dx>dy)
			{
				for(i=x1;i<=x2;i++)
				{
					OLED_Set_Point(i,y1-(i-x1)*dy/dx);
				}
			}
			else
			{
				for(i=y1;i>=y2;i--)
				{
					OLED_Set_Point(x1+(y1-i)*dx/dy,i);
					if(i == 0)break;
				}
			}
		}
	}
	else
	{
		dx = x1-x2;
		if(y2>y1)
		{
			dy = y2-y1;
			if(dx>dy)
			{
				for(i=x1;i>=x2;i--)
				{
					OLED_Set_Point(i,y1+(x1-i)*dy/dx);
					if(i == 0)break;
				}
			}
			else
			{
				for(i=y1;i<=y2;i++)
				{
					OLED_Set_Point(x1-(i-y1)*dx/dy,i);
				}
			}
		}
		else
		{
			dy = y1-y2;
			if(dx>dy)
			{
				for(i=x1;i>=x2;i--)
				{
					OLED_Set_Point(i,y1+(i-x1)*dy/dx);
					if(i == 0)break;
				}
			}
			else
			{
				for(i=y1;i>=y2;i--)
				{
					OLED_Set_Point(x1+(i-y1)*dx/dy,i);
					if(i == 0)break;
				}
			}
		}
	}	
	
}
void TestDrawline(void)
{
	OLED_Draw_line(127,31,0,0);
	OLED_Draw_line(17,31,0,0);
	OLED_Draw_line(127,0,0,31);
	OLED_Draw_line(17,0,0,31);
	OLED_Draw_line(0,0,12,38);
	OLED_Draw_line(0,0,64,38);
	OLED_Draw_line(0,38,64,0);
	OLED_Draw_line(0,38,12,0);
}
void showsin(void)
{
	for(int i = 0;i<127;i++)
	{
		OLED_Set_Point(i,16+16*arm_sin_f32((float)i*0.1));
	}
}
void move_left(void)
{
	uint8_t temp;
	for(int j=0;j<4;j++)
	{
		temp = display_data[j][0];
		for(int i=0;i<127;i++)
		{
			display_data[j][i] = display_data[j][i+1];
		}
		display_data[j][127] = temp;
	}	
}
void insertdisplaydata(unsigned char data)
{
//	OLED_unShowChar2(0,0,'A');
#if 0
	for(int j=0;j<4;j++)
	{
		for(int i=0;i<127;i++)
		{
			display_data[j][127-i] = display_data[j][126-i];
		}
	}
	display_data[0][0] = 0;
	display_data[1][0] = 0;
	display_data[2][0] = 0;
	display_data[3][0] = 0;
	OLED_Set_Point(0,data);	
#else
	for(int j=0;j<4;j++)
	{
		for(int i=0;i<127;i++)
		{
			display_data[j][i] = display_data[j][i+1];
		}
	}
	display_data[0][127] = 0;
	display_data[1][127] = 0;
	display_data[2][127] = 0;
	display_data[3][127] = 0;
	OLED_Set_Point(127,data);	
#endif
//	OLED_ShowChar2(0,0,'A');
}
void OLED_ShowChar2(unsigned char x,unsigned char y,unsigned char chr)
{
	unsigned char c=0,i=0,j=0,pos,temp;	
	c=chr-' '; //获取字符的偏移量	
	if(x>Max_Column-1){x=0;y=y+1;} //如果列书超出了范围，就从下1行的第0列开始
	if(SIZE ==16) //字符大小如果为 16 = 8*16
	{
		for(i=0;i<8;i++)
		{
			temp = F8X16[c*16+8+i];
			pos = 0x80;
			for(j=0;j<8;j++)
			{
				if(temp&pos)
				{
					OLED_Set_Point(x+i,y+j);
				}
				else
				{
					OLED_Clr_Point(x+i,y+j);
				}
				pos = pos>>1;
			}	
			temp = F8X16[c*16+i];
			pos = 0x80;
			for(j=0;j<8;j++)
			{
				if(temp&pos)
				{
					OLED_Set_Point(x+i,y+8+j);
				}
				else
				{
					OLED_Clr_Point(x+i,y+8+j);
				}
				pos = pos>>1;
			}
		}	
	}

}
void OLED_unShowChar2(unsigned char x,unsigned char y,unsigned char chr)
{
	unsigned char c=0,i=0,j=0,pos,temp;	
	c=chr-' '; //获取字符的偏移量	
	if(x>Max_Column-1){x=0;y=y+1;} //如果列书超出了范围，就从下1行的第0列开始
	if(SIZE ==16) //字符大小如果为 16 = 8*16
	{
		for(i=0;i<8;i++)
		{
			temp = F8X16[c*16+8+i];
			pos = 0x80;
			for(j=0;j<8;j++)
			{
				if(temp&pos)
				{
					OLED_Clr_Point(x+i,y+j);
				}
				else
				{
					OLED_Clr_Point(x+i,y+j);
				}
				pos = pos>>1;
			}	
			temp = F8X16[c*16+i];
			pos = 0x80;
			for(j=0;j<8;j++)
			{
				if(temp&pos)
				{
					OLED_Clr_Point(x+i,y+8+j);
				}
				else
				{
					OLED_Clr_Point(x+i,y+8+j);
				}
				pos = pos>>1;
			}
		}	
	}
}
