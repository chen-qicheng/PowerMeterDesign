#include "sys.h"
#include "usart.h"		
#include "delay.h"	  
#include "lcd.h"   
#include "touch.h"  
#include "pic.h"
#include "w25qx.h"
#include "test code.h"

u16 ColorTab[5]={RED,GREEN,BLUE,YELLOW,BRED};//定义颜色数组
u16 ColornTab[4]={RED,GREEN,BLUE};

								 
//电阻触摸屏测试函数
void rtp_test(void)
{ 
	while(1)
	{
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{	
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	
				if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16)Load_Drow_Dialog();//清除
				else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//画图	  			   
			}
		}else delay_ms(10);	//没有按键按下的时候 	    
	}
}
const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  

//电容触摸屏测试函数
void ctp_test(void)
{
	u8 t=0; 	    
 	u16 lastpos[5][2];		//最后一次的数据 
	while(1)
	{
		tp_dev.scan(0);
		for(t=0;t<CT_MAX_TOUCH;t++)//最多5点触摸
		{
			if((tp_dev.sta)&(1<<t))//判断是否有点触摸？
			{
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)//在LCD范围内
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//画线
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<16)
					{
						Load_Drow_Dialog();//清除
					}								
				}
			}else lastpos[t][0]=0XFFFF;
		}
		
	}	
}


//主固定栏
void DrawTestPage(u8 *str)
{
	//绘制固定栏up
	LCD_Clear(WHITE);
	LCD_Fill(0,0,lcddev.width,20,BLUE);
	POINT_COLOR=GREEN;
	LCD_DrawLine(0,20,lcddev.width,20);
	POINT_COLOR=BLUE;
	LCD_DrawLine(0,21,lcddev.width,21);
	POINT_COLOR=BRRED;
	LCD_DrawLine(0,22,lcddev.width,22);
	//绘制固定栏down
	LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
	POINT_COLOR=GREEN;
	LCD_DrawLine(0,lcddev.height-20,lcddev.width,lcddev.height-20);
	POINT_COLOR=BLUE;
	LCD_DrawLine(0,lcddev.height-21,lcddev.width,lcddev.height-21);
	POINT_COLOR=BRRED;            
	LCD_DrawLine(0,lcddev.height-22,lcddev.width,lcddev.height-22);	
	POINT_COLOR=WHITE;
	Gui_StrCenter(0,2,str,16,1);//居中显示
	Gui_StrCenter(0,lcddev.height-18,"鸿讯电子",16,1);//居中显示
}

//中文测试
void Chinese_Font_test(void)
{	
	DrawTestPage("中文显示");
	
		POINT_COLOR=RED;	//画笔颜色
		BACK_COLOR=BLACK;  //背景色 
	
		Show_Str(3,30,"16:鸿讯",16,1);
		Show_Str(3,50,"16:电子",16,1);
		Show_Str(3,70,"24:显",24,1);
	  Show_Str(3,100,"24:示",24,1);
//		Show_Str(6,100,"32X32:屏",32,1);
		delay_ms(1500);
}

//测试主页
void main_test(u8 *id)
{
	DrawTestPage("屏幕测试");
	POINT_COLOR=RED;
	Gui_StrCenter(0,30,"显示屏",16,1);
	POINT_COLOR=MAGENTA;
	Gui_StrCenter(0,50,"测试程序",16,1);
	POINT_COLOR=BLUE;
	Gui_StrCenter(0,70,id,16,1);
	POINT_COLOR=BLACK;
	Gui_StrCenter(0,90,"80RGB*160",16,1);
//	POINT_COLOR=BLACK;
//	Gui_StrCenter(0,110,"TEL:13528896127",16,1);
	delay_ms(3500);		
}

//图形测试
void FillRec_Test(void)
{
	u8 i=0;
	DrawTestPage("图形显示");
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

//英文测试
void English_Font_test(void)
{
	DrawTestPage("英文显示");
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

//图片测试
void Pic_test(void)
{
	DrawTestPage("图片显示");
	POINT_COLOR=BLUE;
	//LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	Gui_Drawbmp16(20,60,gImage_1);
	delay_ms(1500);
}

//旋转测试
void Rotate_Test(void)
{
	u8 num=0;
	u8 *Direction[4]={"R:0","R:180","R:270","R:90"};
	
		for(num=0;num<4;num++)
		{		
			LCD_Display_Dir(num);
			DrawTestPage("旋转测试");
			POINT_COLOR=BLUE;
			Show_Str(25,30,Direction[num],16,1);
//			Gui_Drawbmp16(20,25,gImage_1);
			delay_ms(1000);delay_ms(1000);
		}

	LCD_Display_Dir(USE_LCM_DIR);	

}



//彩条显色
void DispBand(void)	 
{
	unsigned int i,j,k;
	//unsigned int color[8]={RED,RED,GREEN,GREEN,BLUE,BLUE,WHITE,WHITE};
	unsigned int color[8]={BLUE,GREEN,RED,GBLUE,BRED,YELLOW,BLACK,WHITE};
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	
    	
																								  
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

//纯色测试
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

//显示开关测试
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

//读颜色 测试
void Read_Test(void)
{
	u16 color;
	u8 buf[10] = {0},i;
	u8 cbuf[20] = {0};

	DrawTestPage("读屏测试");
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

