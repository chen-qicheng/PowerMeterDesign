#include "sys.h"
#include "usart.h"		
#include "delay.h"	  
#include "lcd.h"   
#include "touch.h"  
#include "pic.h"
#include "w25qx.h"
#include "test code.h"

u16 ColorTab[5]={RED,GREEN,BLUE,YELLOW,BRED};//������ɫ����
u16 ColornTab[4]={RED,GREEN,BLUE};

								 
//���败�������Ժ���
void rtp_test(void)
{ 
	while(1)
	{
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{	
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	
				if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16)Load_Drow_Dialog();//���
				else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//��ͼ	  			   
			}
		}else delay_ms(10);	//û�а������µ�ʱ�� 	    
	}
}
const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  

//���ݴ��������Ժ���
void ctp_test(void)
{
	u8 t=0; 	    
 	u16 lastpos[5][2];		//���һ�ε����� 
	while(1)
	{
		tp_dev.scan(0);
		for(t=0;t<CT_MAX_TOUCH;t++)//���5�㴥��
		{
			if((tp_dev.sta)&(1<<t))//�ж��Ƿ��е㴥����
			{
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)//��LCD��Χ��
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//����
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<16)
					{
						Load_Drow_Dialog();//���
					}								
				}
			}else lastpos[t][0]=0XFFFF;
		}
		
	}	
}


//���̶���
void DrawTestPage(u8 *str)
{
	//���ƹ̶���up
	LCD_Clear(WHITE);
	LCD_Fill(0,0,lcddev.width,20,BLUE);
	POINT_COLOR=GREEN;
	LCD_DrawLine(0,20,lcddev.width,20);
	POINT_COLOR=BLUE;
	LCD_DrawLine(0,21,lcddev.width,21);
	POINT_COLOR=BRRED;
	LCD_DrawLine(0,22,lcddev.width,22);
	//���ƹ̶���down
	LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
	POINT_COLOR=GREEN;
	LCD_DrawLine(0,lcddev.height-20,lcddev.width,lcddev.height-20);
	POINT_COLOR=BLUE;
	LCD_DrawLine(0,lcddev.height-21,lcddev.width,lcddev.height-21);
	POINT_COLOR=BRRED;            
	LCD_DrawLine(0,lcddev.height-22,lcddev.width,lcddev.height-22);	
	POINT_COLOR=WHITE;
	Gui_StrCenter(0,2,str,16,1);//������ʾ
	Gui_StrCenter(0,lcddev.height-18,"��Ѷ����",16,1);//������ʾ
}

//���Ĳ���
void Chinese_Font_test(void)
{	
	DrawTestPage("������ʾ");
	
		POINT_COLOR=RED;	//������ɫ
		BACK_COLOR=BLACK;  //����ɫ 
	
		Show_Str(3,30,"16:��Ѷ",16,1);
		Show_Str(3,50,"16:����",16,1);
		Show_Str(3,70,"24:��",24,1);
	  Show_Str(3,100,"24:ʾ",24,1);
//		Show_Str(6,100,"32X32:��",32,1);
		delay_ms(1500);
}

//������ҳ
void main_test(u8 *id)
{
	DrawTestPage("��Ļ����");
	POINT_COLOR=RED;
	Gui_StrCenter(0,30,"��ʾ��",16,1);
	POINT_COLOR=MAGENTA;
	Gui_StrCenter(0,50,"���Գ���",16,1);
	POINT_COLOR=BLUE;
	Gui_StrCenter(0,70,id,16,1);
	POINT_COLOR=BLACK;
	Gui_StrCenter(0,90,"80RGB*160",16,1);
//	POINT_COLOR=BLACK;
//	Gui_StrCenter(0,110,"TEL:13528896127",16,1);
	delay_ms(3500);		
}

//ͼ�β���
void FillRec_Test(void)
{
	u8 i=0;
	DrawTestPage("ͼ����ʾ");
	LCD_Fill(0,24,lcddev.width,lcddev.height-24,WHITE);
	for (i=0; i<5; i++) 
	{
		POINT_COLOR=ColorTab[i];
		LCD_DrawRectangle(lcddev.width/2-35+(i*12),lcddev.height/2-5+(i*8),lcddev.width/2-35+(i*12)+20,lcddev.height/2-5+(i*8)+20); 
		delay_ms(100);
	}
	for (i=0; i<5; i++) 
	{
		POINT_COLOR=ColorTab[i];
		LCD_Draw_Circle(16+(i*12),50,12);
		delay_ms(100);
	}
	delay_ms(1500);
	
	
	for (i=0; i<5; i++) 
	{
		POINT_COLOR=ColorTab[i];
		LCD_Fill(lcddev.width/2-35+(i*12),lcddev.height/2-5+(i*8),lcddev.width/2-35+(i*12)+20,lcddev.height/2-5+(i*8)+20,POINT_COLOR); 
		delay_ms(100);
	}
	for (i=0; i<5; i++) 
	{
		POINT_COLOR=ColorTab[i];
		gui_fill_circle(16+(i*12),50,12,POINT_COLOR);
		delay_ms(100);
	}
	delay_ms(1500);
}

//Ӣ�Ĳ���
void English_Font_test(void)
{
	DrawTestPage("Ӣ����ʾ");
	POINT_COLOR=RED;
	BACK_COLOR=WHITE;
	LCD_ShowString(8,30,200,12,12,"12:abc");
	LCD_ShowString(8,45,200,12,12,"12:ABC");
	POINT_COLOR=BLUE;
	LCD_ShowString(8,60,200,16,16,"16:abc");
	LCD_ShowString(8,80,200,16,16,"16:ABC");
	POINT_COLOR=BLUE;
	LCD_ShowString(8,100,200,24,24,"24:aBc");
//	LCD_ShowString(8,128,200,24,24,"12X24:ABC");
	delay_ms(1500);
}

//ͼƬ����
void Pic_test(void)
{
	DrawTestPage("ͼƬ��ʾ");
	POINT_COLOR=BLUE;
	//LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	Gui_Drawbmp16(20,60,gImage_1);
	delay_ms(1500);
}

//��ת����
void Rotate_Test(void)
{
	u8 num=0;
	u8 *Direction[4]={"R:0","R:180","R:270","R:90"};
	
		for(num=0;num<4;num++)
		{		
			LCD_Display_Dir(num);
			DrawTestPage("��ת����");
			POINT_COLOR=BLUE;
			Show_Str(25,30,Direction[num],16,1);
//			Gui_Drawbmp16(20,25,gImage_1);
			delay_ms(1000);delay_ms(1000);
		}

	LCD_Display_Dir(USE_LCM_DIR);	

}



//������ɫ
void DispBand(void)	 
{
	unsigned int i,j,k;
	//unsigned int color[8]={RED,RED,GREEN,GREEN,BLUE,BLUE,WHITE,WHITE};
	unsigned int color[8]={BLUE,GREEN,RED,GBLUE,BRED,YELLOW,BLACK,WHITE};
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	
    	
																								  
	for(i=0;i<8;i++)
	{
		for(j=0;j<lcddev.height/8;j++)
		{
	        for(k=0;k<lcddev.width;k++)
			{
				
				LCD_WriteRAM(color[i]);				
				
			} 
		}
	}
	for(j=0;j<lcddev.height%8;j++)
	{
        for(k=0;k<lcddev.width;k++)
		{
			
				LCD_WriteRAM(color[7]);			
			
		} 
	}
	

	delay_ms(1000);
}

//��ɫ����
void Color_Test(void)
{
	DispBand();               
	LCD_Clear(WHITE);
	POINT_COLOR=BLACK;
	Show_Str(20,15,"WHITE",16,1);delay_ms(1500);
	POINT_COLOR=WHITE;
	LCD_Clear(RED);
	Show_Str(25,15,"RED ",16,1);delay_ms(1500);
	LCD_Clear(GREEN);
	Show_Str(20,15,"GREEN ",16,1);delay_ms(1500);
	LCD_Clear(BLUE);
	Show_Str(22,15,"BLUE ",16,1);delay_ms(1500);
	
}

//��ʾ���ز���
void Switch_test(void)
{
	u8 num=3;
	while(num--)
	{
	LCD_DisplayOff();
	delay_ms(400);
	LCD_DisplayOn();
	delay_ms(400);
	}
	
}

//����ɫ ����
void Read_Test(void)
{
	u16 color;
	u8 buf[10] = {0},i;
	u8 cbuf[20] = {0};

	DrawTestPage("��������");
	LCD_Fill(0,23,lcddev.width,lcddev.height-23,WHITE);	
	LCD_Read_ID(0x04);
	sprintf((char *)buf,"ID:0x%x",lcddev.id);
	POINT_COLOR=RED;
	Show_Str(3,28,buf,16,1);
	
		for (i=0; i<3; i++) 
	{
		POINT_COLOR=ColornTab[i];
		LCD_Fill(2,60+i*25-10,15,60+i*25+10,POINT_COLOR);
		color = LCD_ReadPoint(4,60+i*25);
		sprintf((char *)cbuf,"=0x%04x",color);
		Show_Str(18,60+i*25-8,cbuf,16,1);
	}

	delay_ms(1500);
}

