#include "hc32_ddl.h"
#include "OLED_FRONT.h"
#include "Hw_I2C.h"
#include "OLED.h"
#include <math.h>
#include <arm_math.h>
uint8_t display_data[Max_Data_Row][Max_Column];

/*
	@brief			IICд������
	@param			IIC_Command��д�������
	@retval			��
 */
void Write_IIC_Command(unsigned char IIC_Command)
{
	en_result_t status;
	uint8_t trytime = 0;
    unsigned char temp;	//�������
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
	@brief			IICд������
	@param			IIC_Data������
	@retval			��
 */
void Write_IIC_Data(unsigned char IIC_Data)
{
	en_result_t status;
	uint8_t trytime = 0;
    unsigned char temp;	//�������
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
       Write_IIC_Data(dat); //д������
	  }
	else {
       Write_IIC_Command(dat); //д������
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
void OLED_Refresh(void)
{
	unsigned char i,n;	
	for(i=0;i<Max_Row/8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //��0~7ҳ����д��
		OLED_WR_Byte (0x00,OLED_CMD);      //�е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //�иߵ�ַ  
		for(n=0;n<128;n++)OLED_WR_Byte(display_data[i][n],OLED_DATA); //д�� 0 ����
	}
}
/*
	@brief			����
	@param			��
	@retval			��
 */	
void OLED_Clear(void)  
{  
	unsigned char i,n;		    //�������
	for(i=0;i<Max_Row/8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //��0~7ҳ����д��
		OLED_WR_Byte (0x00,OLED_CMD);      //�е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //�иߵ�ַ  
		for(n=0;n<128;n++) //д�� 0 ����
		{
			OLED_WR_Byte(0,OLED_DATA);
			display_data[i][n] = 0;
		}		
	}
}
void OLED_Set_Point(unsigned char x,unsigned char y)
{
	volatile uint8_t temp,line;
	if(y>=Max_Row)//��������
	{
		return;
	}
	if(x>=Max_Column)//��������
	{
		return;
	}
	line = (Max_Data_Row - 1) - (y>>3);//�к�
	temp = y-((Max_Data_Row - 1)-line)*8;//byteλ��
	temp = 0x80>>temp;
	display_data[line][x] |= temp;
}
void OLED_Clr_Point(unsigned char x,unsigned char y)
{
	volatile uint8_t temp,line;
	if(y>=Max_Row)//��������
	{
		return;
	}
	if(x>=Max_Column)//��������
	{
		return;
	}
	line = (Max_Data_Row - 1) - (y>>3);//�к�
	temp = y-((Max_Data_Row - 1)-line)*8;//byteλ��
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
	OLED_Draw_line(0,0,12,31);
	OLED_Draw_line(0,0,64,31);
	OLED_Draw_line(0,63,64,0);
	OLED_Draw_line(0,63,12,0);
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
	c=chr-' '; //��ȡ�ַ���ƫ����	
	if(x>Max_Column-1){x=0;y=y+1;} //������鳬���˷�Χ���ʹ���1�еĵ�0�п�ʼ
	if(SIZE ==16) //�ַ���С���Ϊ 16 = 8*16
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
	c=chr-' '; //��ȡ�ַ���ƫ����	
	if(x>Max_Column-1){x=0;y=y+1;} //������鳬���˷�Χ���ʹ���1�еĵ�0�п�ʼ
	if(SIZE ==16) //�ַ���С���Ϊ 16 = 8*16
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

void showsin(void)
{
	for(int i = 0;i<127;i++)
	{
		OLED_Set_Point(i,16+16*arm_sin_f32((float)i*0.1));
	}
}

