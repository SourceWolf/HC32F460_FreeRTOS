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
void OLED_Refresh(void);
void OLED_Set_Point(unsigned char x,unsigned char y);
void OLED_Draw_line(unsigned char x1,unsigned char y1,unsigned char x2,unsigned char y2);
void showsin(void);
void move_left(void);
void insertdisplaydata(unsigned char data);
void OLED_ShowChar2(unsigned char x,unsigned char y,unsigned char chr);
void OLED_unShowChar2(unsigned char x,unsigned char y,unsigned char chr);
#endif

