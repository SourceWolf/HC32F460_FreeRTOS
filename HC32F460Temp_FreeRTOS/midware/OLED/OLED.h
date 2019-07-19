#ifndef OLED_H
#define OLED_H

void OLED_Init(void);
void OLED_WR_Byte(unsigned char dat,unsigned char cmd);
void OLED_ShowString(unsigned char x,unsigned char y,unsigned char *chr);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2);
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Clear(void);
void OLED_Display_Off(void);
void OLED_Display_On(void);
#endif

