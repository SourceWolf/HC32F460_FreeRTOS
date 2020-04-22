/*
 * OLED_96.h
 *
 * Created: 2015/1/7 16:14:39
 *  Author: dell1
 */ 


#ifndef OLED_96_H_
#define OLED_96_H_
//#include <asf.h>
#include "hc32_ddl.h"
#define OLED_CMD  0  //д����
#define OLED_DATA 1  //д����
#define OLED_MODE 0

//#define OLED_CS       PIN_PB14
//#define OLED_RST      PIN_PB12
//#define OLED_DC       PIN_PB13
//#define OLED_SCL      PIN_PB10
//#define OLED_SDA      PIN_PB11
#define OLED_CS_PORT	PortE
#define OLED_MOSI_PORT	PortE
#define OLED_CLK_PORT	PortE
#define OLED_RST_PORT	PortE
#define OLED_DC_PORT	PortE
#define OLED_CS_PIN		Pin01
#define OLED_MOSI_PIN	Pin02
#define OLED_CLK_PIN	Pin00
#define OLED_RST_PIN	Pin03
#define OLED_DC_PIN		Pin04

#define SPI_DataLength 8
/************************************************************************/
/*OLED��������
*OLED_CS_SET((x))   Ƭѡ��x=0,��Ч��x=1,��Ч��
*OLED_RST_SET((x))  ��λ��x=0,��Ч��x=1,��Ч��
*OLED_DC_SET((x))   �������ݿ��ƣ�x=0,��Ч��x=1,��Ч��
*OLED_SCL_SET((x))  SPIʱ�ӣ�x=0,�͵�ƽ��x=1,�ߵ�ƽ��
*OLED_SDA_SET((x))  SPI���ݣ�x=0,�͵�ƽ��x=1,�ߵ�ƽ��
*/
/************************************************************************/
//#define OLED_CS_SET(x)   (port_pin_set_output_level(OLED_CS,(x)))
//#define OLED_RST_SET(x)  (port_pin_set_output_level(OLED_RST,(x)))
//#define OLED_DC_SET(x)   (port_pin_set_output_level(OLED_DC,(x)))
//#define OLED_SCL_SET(x)  (port_pin_set_output_level(OLED_SCL,(x)))
//#define OLED_SDA_SET(x)  (port_pin_set_output_level(OLED_SDA,(x)))

/************************************************************************/
/*                           OLED����������                             */
/************************************************************************/
#define SIZE        16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF
#define X_WIDTH 	128
#define Y_WIDTH 	64

/************************************************************************/
/*                        Function Prototype                            */
/************************************************************************/
//void OLED_Init(void);
//void OLED_Clear(void);
//void OLED_Display_On(void);
//void OLED_Display_Off(void);
//
//void OLED_WR_Byte(uint8_t dat,uint8_t cmd);

//OLED���ƺ���
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr);
uint32_t oled_pow(uint8_t m,uint8_t n);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLEDIO_init(void);
void OLED_showFloat(uint8_t x,uint8_t y,float fnum);
void OLED_ShowInt(uint8_t x,uint8_t y,uint8_t num);
void OLED_ShowInitial(void);
#endif /* OLED_96_H_ */