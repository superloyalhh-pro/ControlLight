#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "font.h"
#include "Lcd.h"


/*������������ʾһ��16*16�ĺ��ֻ���8*16��ascll��
*xy����ʾ�����꣬ȡֵ 0~128
*fc:�ֵ���ɫ
*bc:��������ɫ
*s:Ҫ��ʾ���ַ���
*/
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
    unsigned char i,j;
    unsigned short k,x0;
    x0=x;

    while(*s) 
    {	
        if((*s) < 128) 
        {
            k=*s;
            if (k==13) 
            {
                x=x0;
                y+=16;
            }
            else 
            {
                if (k>32) k-=32; else k=0;

                for(i=0;i<16;i++)
                for(j=0;j<8;j++) 
                {
                    if(asc16[k*16+i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
                    else 
                    {
                        if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
                    }
                }
                x+=8;
            }
            s++;
        }			
        else 
        {
            for (k=0;k<hz16_num;k++) 
            {
                if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
                { 
                    for(i=0;i<16;i++)
                    {
                        for(j=0;j<8;j++) 
                        {
                            if(hz16[k].Msk[i*2]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
                            else {
                            	if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
                            }
                        }
                        
                       for(j=0;j<8;j++) 
                       {
                            if(hz16[k].Msk[i*2+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
                            else 
                            {
                            	if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
                            }
                        }
                    }
                }
            }
            s+=2;x+=16;
        } 
    }
}

/*������������ʾ24*24�ĺ���
*xy����ʾ�����꣬ȡֵ 0~128
*fc:�ֵ���ɫ
*bc:��������ɫ
*s:Ҫ��ʾ���ַ���
*/
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
    unsigned char i,j;
    unsigned short k;

    while(*s) 
    {
        if( *s < 0x80 ) 
        {
            k=*s;
            if (k>32) k-=32; else k=0;

            for(i=0;i<16;i++)
            for(j=0;j<8;j++) 
            {
                if(asc16[k*16+i]&(0x80>>j))	
                Gui_DrawPoint(x+j,y+i,fc);
                else 
                {
                    if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
                }
            }
            s++;x+=8;
        }
        else 
        {

            for (k=0;k<hz24_num;k++) 
            {
                if ((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
                { 
                    for(i=0;i<24;i++)
                    {
                        for(j=0;j<8;j++) 
                        {
                            if(hz24[k].Msk[i*3]&(0x80>>j))
                            Gui_DrawPoint(x+j,y+i,fc);
                            else 
                            {
                                if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
                            }
                        }
                        
                        for(j=0;j<8;j++) 
                        {
                            if(hz24[k].Msk[i*3+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
                            else {
                                if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
                            }
                        }
                        
                        for(j=0;j<8;j++) 
                        {
                            if(hz24[k].Msk[i*3+2]&(0x80>>j))	
                            Gui_DrawPoint(x+j+16,y+i,fc);
                            else 
                            {
                                if (fc!=bc) Gui_DrawPoint(x+j+16,y+i,bc);
                            }
                        }
                    }
                }
            }
            s+=2;x+=24;
        }
    }
}


/*������������ʾ���������
*xy����ʾ�����꣬ȡֵ 0~128
*fc:�ֵ���ɫ
*bc:��������ɫ
*num:��ʾ������
*/
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
{
    unsigned char i,j,k,c;

    for(i=0;i<32;i++)
    {
        for(j=0;j<4;j++) 
        {
            c=*(sz32+num*32*4+i*4+j);
            for (k=0;k<8;k++)	
            {
                if(c&(0x80>>k))	Gui_DrawPoint(x+j*8+k,y+i,fc);
                else {
                    if (fc!=bc) Gui_DrawPoint(x+j*8+k,y+i,bc);
                }
            }
        }
    }
}

    
/*������������ʾ6*8һ���׼ASCII�ַ���    ��ʾ������
*xy����ʾ�����꣬ȡֵ 0~128
*fc:�ֵ���ɫ
*bc:��������ɫ
*ch:Ҫ��ʾ���ַ���
*/
void LCD_P6x8Str(unsigned char x, unsigned char y, u16 fc, u16 bc,unsigned char ch[])
{
    unsigned char c=0,idx=0,i=0,j=0,k=0;      
    while (ch[j]!='\0')
    {
        c=ch[j];
        idx =ch[j]-32;
        for(i=0;i<6;i++)     
        {
            c=F6x8[idx][i];
            for(k=0; k<8; k++)
            {
                if((c&0x01)==1)
                {
                    Gui_DrawPoint(x+i,y+k,fc);
                }
                else
                {
                    Gui_DrawPoint(x+i,y+k,bc);
                }
                c>>=1;
            }
        }
        x+=6;
        j++;
    }
}


void delay_ms(int n)
{
	int i;
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
}

//��SPI���ߴ���һ��8λ����
void  SPI_WriteData(u8 Data)
{
    unsigned char i=0;

    for(i=8;i>0;i--)
    {
        if(Data&0x80)	
        {
            LCD_SDA_SET; //�������
        }
        else
        {
            LCD_SDA_CLR;
        }

        LCD_SCL_CLR;
        LCD_SCL_SET;
        Data<<=1; 
    }
}

//��Һ����дһ��8λָ��
void Lcd_WriteIndex(u8 Index)
{
    LCD_RS_CLR;
    SPI_WriteData(Index);
}

//��Һ����дһ��8λ����
void Lcd_WriteData(u8 Data)
{
    LCD_RS_SET;
    SPI_WriteData(Data);
}
//��Һ����дһ��16λ����
void LCD_WriteData_16Bit(u16 Data)
{
    LCD_RS_SET;
    SPI_WriteData(Data>>8); 	//д���8λ����
    SPI_WriteData(Data); 			//д���8λ����
}

//д�Ĵ�������
void Lcd_WriteReg(u8 Index,u8 Data)
{
    Lcd_WriteIndex(Index);
    Lcd_WriteData(Data);
}

//LCD��λ
void Lcd_Reset(void)
{
    LCD_RST_CLR;
    delay_ms(1000);
    LCD_RST_SET;
    delay_ms(500);
}

//Ӳ��IO�ڳ�ʼ��
void InitIo(void)
{
  int i=0;

  gpio_pad_select_gpio(GPIO_NUM_33);
  gpio_pad_select_gpio(GPIO_NUM_14);
  gpio_pad_select_gpio(GPIO_NUM_12);
  gpio_pad_select_gpio(GPIO_NUM_13);

  gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT);
  gpio_set_direction(GPIO_NUM_13, GPIO_MODE_OUTPUT);

  LCD_SCL_CLR;
  LCD_SDA_CLR;
  LCD_RST_CLR;
  LCD_RS_CLR;
  vTaskDelay(10);
  LCD_SCL_SET;
  LCD_SDA_SET;
  LCD_RST_SET;
  LCD_RS_SET;
  vTaskDelay(10);
}

//LCD ��ʼ��
void Lcd_Init(void)
{	
    InitIo();
    Lcd_Reset(); //��λ LCD

    Lcd_WriteIndex(0x11);//Sleep exit 
    delay_ms (200);

    //ST7735R Frame Rate
    Lcd_WriteIndex(0xB1); 
    Lcd_WriteData(0x01); 
    Lcd_WriteData(0x2C); 
    Lcd_WriteData(0x2D); 

    Lcd_WriteIndex(0xB2); 
    Lcd_WriteData(0x01); 
    Lcd_WriteData(0x2C); 
    Lcd_WriteData(0x2D); 

    Lcd_WriteIndex(0xB3); 
    Lcd_WriteData(0x01); 
    Lcd_WriteData(0x2C); 
    Lcd_WriteData(0x2D); 
    Lcd_WriteData(0x01); 
    Lcd_WriteData(0x2C); 
    Lcd_WriteData(0x2D); 

    Lcd_WriteIndex(0xB4); //Column inversion 
    Lcd_WriteData(0x07); 

    //ST7735R Power Sequence
    Lcd_WriteIndex(0xC0); 
    Lcd_WriteData(0xA2); 
    Lcd_WriteData(0x02); 
    Lcd_WriteData(0x84); 
    Lcd_WriteIndex(0xC1); 
    Lcd_WriteData(0xC5); 

    Lcd_WriteIndex(0xC2); 
    Lcd_WriteData(0x0A); 
    Lcd_WriteData(0x00); 

    Lcd_WriteIndex(0xC3); 
    Lcd_WriteData(0x8A); 
    Lcd_WriteData(0x2A); 
    Lcd_WriteIndex(0xC4); 
    Lcd_WriteData(0x8A); 
    Lcd_WriteData(0xEE); 

    Lcd_WriteIndex(0xC5); //VCOM 
    Lcd_WriteData(0x0E); 

    Lcd_WriteIndex(0x36); //MX, MY, RGB mode 
    Lcd_WriteData(0xC8); 

    //ST7735R Gamma Sequence
    Lcd_WriteIndex(0xe0); 
    Lcd_WriteData(0x0f); 
    Lcd_WriteData(0x1a); 
    Lcd_WriteData(0x0f); 
    Lcd_WriteData(0x18); 
    Lcd_WriteData(0x2f); 
    Lcd_WriteData(0x28); 
    Lcd_WriteData(0x20); 
    Lcd_WriteData(0x22); 
    Lcd_WriteData(0x1f); 
    Lcd_WriteData(0x1b); 
    Lcd_WriteData(0x23); 
    Lcd_WriteData(0x37); 
    Lcd_WriteData(0x00); 	
    Lcd_WriteData(0x07); 
    Lcd_WriteData(0x02); 
    Lcd_WriteData(0x10); 

    Lcd_WriteIndex(0xe1); 
    Lcd_WriteData(0x0f); 
    Lcd_WriteData(0x1b); 
    Lcd_WriteData(0x0f); 
    Lcd_WriteData(0x17); 
    Lcd_WriteData(0x33); 
    Lcd_WriteData(0x2c); 
    Lcd_WriteData(0x29); 
    Lcd_WriteData(0x2e); 
    Lcd_WriteData(0x30); 
    Lcd_WriteData(0x30); 
    Lcd_WriteData(0x39); 
    Lcd_WriteData(0x3f); 
    Lcd_WriteData(0x00); 
    Lcd_WriteData(0x07); 
    Lcd_WriteData(0x03); 
    Lcd_WriteData(0x10);  

    Lcd_WriteIndex(0x2a);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x7f);

    Lcd_WriteIndex(0x2b);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x9f);

    Lcd_WriteIndex(0xF0); //Enable test command  
    Lcd_WriteData(0x01); 
    Lcd_WriteIndex(0xF6); //Disable ram power save mode 
    Lcd_WriteData(0x00); 

    Lcd_WriteIndex(0x3A); //65k mode 
    Lcd_WriteData(0x05); 

    Lcd_WriteIndex(0x29);//Display on
    Lcd_Clear(WHITE);	
}


/*************************************************
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
void Lcd_SetRegion(u16 x_start,u16 y_start,u16 x_end,u16 y_end)
{		
    Lcd_WriteIndex(0x2a);
    Lcd_WriteData(0x00);
    Lcd_WriteData(x_start+2);
    Lcd_WriteData(0x00);
    Lcd_WriteData(x_end+2);

    Lcd_WriteIndex(0x2b);
    Lcd_WriteData(0x00);
    Lcd_WriteData(y_start+3);
    Lcd_WriteData(0x00);
    Lcd_WriteData(y_end+3);

    Lcd_WriteIndex(0x2c);
}
	
/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�������
����ֵ����
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
    if(x>=X_MAX_PIXEL) return;
    if(y>=Y_MAX_PIXEL) return;
    
    Lcd_SetRegion(x,y,x+1,y+1);
    LCD_WriteData_16Bit(Data);
}    


/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Clear(u16 Color)               
{	
    unsigned int i,m;
    Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
    Lcd_WriteIndex(0x2C);
    
    for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    { 
        LCD_WriteData_16Bit(Color);
    }
}


/*************************************************
��������Lcd_Clear
���ܣ��������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_ClearRect(u16 x1,u16 y1, u16 x2,u16 y2, u16 Color)               
{	
    unsigned int i,m;
    Lcd_SetRegion(x1,y1, x2,y2);
    Lcd_WriteIndex(0x2C);
    
    for(i=x1;i<x2;i++)
    {
        for(m=y1;m<y2;m++)
        {	
            LCD_WriteData_16Bit(Color);
        }   
    }
}
