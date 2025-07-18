#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"
#include "delay.h"
#include "string.h"


//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;
	 					    
//写寄存器函数
//data:寄存器值
void LCD_WR_REG(u16 data)
{
		u8 i;	

		SPI_CS = 0;		
		SPI_DC = 0;//RS=0 命令
		
		for(i=0; i<8; i++)
		{
			if (data & 0x80)
			 SPI_SDI = 1;
			else
			 SPI_SDI = 0;
			
			data <<= 1;		 
			SPI_SCK = 0;		  
			SPI_SCK = 1;
		}
		SPI_CS=1;				
}
//写数据函数
//data:寄存器值
void LCD_WR_DATA(u16 data)
{
		u8 i;
	
		SPI_CS = 0;		
		SPI_DC = 1;//RS=1 数据
		
		for(i=0; i<8; i++)
		{
			if (data & 0x80)
			 SPI_SDI = 1;
			else
			 SPI_SDI = 0;
			
			data <<= 1;		 
			SPI_SCK = 0;		  
			SPI_SCK = 1;
		}
		SPI_CS=1;			
}

//写寄存器
//LCD_Reg:寄存器编号
//LCD_RegValue:要写入的值
void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WriteRAM(LCD_RegValue);	    		 
}   

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
} 
//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{							    
//写十六位GRAM
  u8 i;
	SPI_CS = 0;
	SPI_DC = 1;
	
	for(i=0; i<16; i++)
	{
		if (RGB_Code & 0x8000)
		 SPI_SDI = 1;
		else
		 SPI_SDI = 0;
		
		RGB_Code <<= 1;		 
		SPI_SCK = 0;		  
		SPI_SCK = 1;
	} 
	SPI_CS=1;	
}

//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
	while(i--);
}

//LCD开启显示
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X29);	//开启显示
}	 
//LCD关闭显示
void LCD_DisplayOff(void)
{	   
	LCD_WR_REG(0X28);	//关闭显示
}   
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 	 
   if(lcddev.dir==0)
	 { 
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA((Xpos+26)>>8);LCD_WR_DATA((Xpos+26)&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA((Ypos+1)>>8);LCD_WR_DATA((Ypos+1)&0XFF); 
		
	 }
  else	if(lcddev.dir==1)
		{ 
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA((Xpos+26)>>8);LCD_WR_DATA((Xpos+26)&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA((Ypos+1)>>8);LCD_WR_DATA((Ypos+1)&0XFF);
	 }
	else	if(lcddev.dir==2)
		{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA((Xpos+1)>>8);LCD_WR_DATA((Xpos+1)&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA((Ypos+26)>>8);LCD_WR_DATA((Ypos+26)&0XFF);
    }
	else	if(lcddev.dir==3)
		{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA((Xpos+1)>>8);LCD_WR_DATA((Xpos+1)&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA((Ypos+26)>>8);LCD_WR_DATA((Ypos+26)&0XFF);
    }		 
} 		 
   
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD_WriteRAM(POINT_COLOR); 
}	 
//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
		//设置光标位置
		LCD_SetCursor(x,y); 	 
		//写入颜色
		LCD_WriteReg(lcddev.wramcmd,color);
}


//dir:方向选择 	0-0度旋转，1-180度旋转，2-270度旋转，3-90度旋转
void LCD_Display_Dir(u8 dir)
{
	lcddev.dir=dir;
	if(lcddev.dir==0||lcddev.dir==1)			//竖屏
	{
			lcddev.width=80;
			lcddev.height=160;

			lcddev.wramcmd=0X2C;
			lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;
		
		if(lcddev.dir==0)         //0-0度旋转
		{
			LCD_WR_REG(0x36); 
			LCD_WR_DATA((1<<3)|(1<<7)|(1<<6)|(0<<5));
		}else							//1-180度旋转
		{
			LCD_WR_REG(0x36); 
			LCD_WR_DATA((1<<3)|(0<<7)|(0<<6)|(0<<5));		
		}
		
	}else if(lcddev.dir==2||lcddev.dir==3)  //横屏
	{
		
			lcddev.width=160;
			lcddev.height=80;

			lcddev.wramcmd=0X2C;
			lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B; 

				if(lcddev.dir==2)				//2-270度旋转
				{
					LCD_WR_REG(0x36); 
					LCD_WR_DATA((1<<3)|(0<<7)|(1<<6)|(1<<5));

				}else							//3-90度旋转
				{
					LCD_WR_REG(0x36); 
					LCD_WR_DATA((1<<3)|(1<<7)|(0<<6)|(1<<5));
				}		
	}	
	
	
}	 
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height. 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    
	u16 ex,ey;
	ex=sx+width-1;
	ey=sy+height-1;
	
	if(lcddev.dir==0)
	{
		    sx=sx+26;
		    ex=ex+26;
		    sy=sy+1;
	      ey=ey+1;
	}else if(lcddev.dir==1)
		{
		    sx=sx+26;
		    ex=ex+26;
		    sy=sy+1;
	      ey=ey+1;
	  }else if(lcddev.dir==2)
			{
		    sx=sx+1;
		    ex=ex+1;
		    sy=sy+26;
	      ey=ey+26;
	    }else if(lcddev.dir==3)
				{
		    sx=sx+1;
		    ex=ex+1;
		    sy=sy+26;
	      ey=ey+26;
	      }
	
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA((sx)&0XFF);	 
		LCD_WR_DATA(ex>>8); 
		LCD_WR_DATA(ex&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(ey>>8); 
		LCD_WR_DATA(ey&0XFF); 
}
//初始化lcd
void LCD_Init(void)
{

	RCC->APB2ENR|=1<<2;//先使能外设PORTA时钟
 	RCC->APB2ENR|=1<<3;//先使能外设PORTB时钟
 	RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟

	RCC->APB2ENR|=1<<0;    //开启辅助时钟
	JTAG_Set(SWD_ENABLE);  //开启SWD
	
	GPIOA->CRL&=0XFFFFF0FF;
	GPIOA->CRL|=0X00000300;//LED(背光开关) = PA2
	GPIOA->CRH&=0X0FF00FFF;
	GPIOA->CRH|=0X30038000;//SPI_SDO = PA11  CS = PA12   DC = PA15
	GPIOA->ODR|=0X9804;    //IO上拉
	
	GPIOC->CRH&=0X000FFFFF;
	GPIOC->CRH|=0X33300000;//SDI = PC13  SCK = PC14  RST = PC15 	
	GPIOC->ODR|=0XE000;    //IO上拉
	

	
	SPI_RST=1;
	delay_ms(1);
	SPI_RST=0;
	delay_ms(10);
	SPI_RST=1;
	delay_ms(120);  


//************* Start Initial Sequence **********//

LCD_WR_REG(0x11);     //Sleep out

delay_ms(120);                //delay_ms 120ms

LCD_WR_REG(0xB1);     //Normal mode
LCD_WR_DATA(0x05);   
LCD_WR_DATA(0x3C);   
LCD_WR_DATA(0x3C);   

LCD_WR_REG(0xB2);     //Idle mode
LCD_WR_DATA(0x05);   
LCD_WR_DATA(0x3C);   
LCD_WR_DATA(0x3C);   

LCD_WR_REG(0xB3);     //Partial mode
LCD_WR_DATA(0x05);   
LCD_WR_DATA(0x3C);   
LCD_WR_DATA(0x3C);   
LCD_WR_DATA(0x05);   
LCD_WR_DATA(0x3C);   
LCD_WR_DATA(0x3C);   

LCD_WR_REG(0xB4);     //Dot inversion
LCD_WR_DATA(0x03);   

LCD_WR_REG(0xC0);     //AVDD GVDD
LCD_WR_DATA(0x04);   
LCD_WR_DATA(0x04);   
LCD_WR_DATA(0x84);   

LCD_WR_REG(0xC1);     //VGH VGL
LCD_WR_DATA(0x06);   

LCD_WR_REG(0xC2);     //Normal Mode
LCD_WR_DATA(0x0D);   
LCD_WR_DATA(0x00);   

LCD_WR_REG(0xC3);     //Idle
LCD_WR_DATA(0x8D);   
LCD_WR_DATA(0x6A);   

LCD_WR_REG(0xC4);     //Partial+Full
LCD_WR_DATA(0x8D);   
LCD_WR_DATA(0xEE);   

LCD_WR_REG(0xC5);     //VCOM
LCD_WR_DATA(0x3A);   

LCD_WR_REG(0xE0);     //positive gamma
LCD_WR_DATA(0x10);   
LCD_WR_DATA(0x0E);   
LCD_WR_DATA(0x05);   
LCD_WR_DATA(0x0D);   
LCD_WR_DATA(0x0E);   
LCD_WR_DATA(0x06);   
LCD_WR_DATA(0x03);   
LCD_WR_DATA(0x07);   
LCD_WR_DATA(0x0A);   
LCD_WR_DATA(0x0E);   
LCD_WR_DATA(0x1D);   
LCD_WR_DATA(0x32);   
LCD_WR_DATA(0x03);   
LCD_WR_DATA(0x0B);   
LCD_WR_DATA(0x00);   
LCD_WR_DATA(0x2F);   

LCD_WR_REG(0xE1);     //negative gamma
LCD_WR_DATA(0x0E);   
LCD_WR_DATA(0x12);   
LCD_WR_DATA(0x05);   
LCD_WR_DATA(0x07);   
LCD_WR_DATA(0x0C);   
LCD_WR_DATA(0x05);   
LCD_WR_DATA(0x02);   
LCD_WR_DATA(0x06);   
LCD_WR_DATA(0x07);   
LCD_WR_DATA(0x0B);   
LCD_WR_DATA(0x13);   
LCD_WR_DATA(0x2C);   
LCD_WR_DATA(0x03);   
LCD_WR_DATA(0x0D);   
LCD_WR_DATA(0x02);   
LCD_WR_DATA(0x1F);   

LCD_WR_REG(0x3A);     
LCD_WR_DATA(0x05);   

LCD_WR_REG(0x36);     //BGR
LCD_WR_DATA(0x08);   

LCD_WR_REG(0x21);     //Display inversion

LCD_WR_REG(0x29);     //Display on

} 


//读数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{	
  u8 i;	
	u16 t=0;
		for(i=0; i<8; i++)
		{
			SPI_SCK =0;
			//opt_delay(2);//延时	
			t=t << 1 | SPI_SDI_RD; 
			SPI_SCK = 1;	
      //opt_delay(2);			
		}	
	return t;  
	

}

//读显示屏 ID
u16 LCD_Read_ID(u8 reg)
{
	
		u8 i;	

		SPI_CS = 0;		
		SPI_DC = 0;//RS=0 命令
		
		for(i=0; i<8; i++)
		{
			if (reg & 0x80)
			 SPI_SDI = 1;
			else
			 SPI_SDI = 0;
			
			reg <<= 1;		 
			SPI_SCK = 0;	  
			SPI_SCK = 1;
		}
		
	GPIOC->CRH&=0X000FFFFF;
	GPIOC->CRH|=0X33800000;//SDI = PC13  SCK = PC14  RST = PC15 	
	GPIOC->ODR|=0XE000;    //IO上拉

	SPI_SCK =0;
	SPI_SCK =1;	

	LCD_RD_DATA(); //7C
	
	lcddev.id=LCD_RD_DATA();  //89  									   
	lcddev.id<<=8;
	lcddev.id|=LCD_RD_DATA(); //F0 	
			
		SPI_CS=1;
	
	GPIOC->CRH&=0X000FFFFF;
	GPIOC->CRH|=0X33300000;//SDI = PC13  SCK = PC14  RST = PC15 	
	GPIOC->ODR|=0XE000;    //IO上拉
		
	return lcddev.id;
}


//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色
u16 LCD_ReadPoint(u16 x,u16 y)
{
	u8 i,r,g,b,reg=0x2e;
 	u16 color=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	LCD_SetCursor(x,y);
	SPI_CS = 0;		
	SPI_DC = 0;
		
		for(i=0; i<8; i++)
		{
			if (reg & 0x80)
			 SPI_SDI = 1;
			else
			 SPI_SDI = 0;
			
			reg <<= 1;		 
			SPI_SCK = 0;  
			SPI_SCK = 1;	
		}

	GPIOC->CRH&=0X000FFFFF;
	GPIOC->CRH|=0X33800000;//SDI = PC13  SCK = PC14  RST = PC15 	
	GPIOC->ODR|=0XE000;    //IO上拉		
		

		for(i=0; i<9; i++)							//第一次空读 后三次分别为R G B
		{
			SPI_SCK = 0;		 	
			SPI_SCK = 1;			
		}

		

		for(i=0; i<8; i++)
		{
			SPI_SCK = 0;		r=r << 1 | SPI_SDI_RD; 	
			SPI_SCK = 1;			
		}


		for(i=0; i<8; i++)
		{
			SPI_SCK = 0;		g=g << 1 | SPI_SDI_RD; 	
			SPI_SCK = 1;			
		}


		for(i=0; i<8; i++)
		{
			SPI_SCK = 0;		b=b << 1 | SPI_SDI_RD; 	
			SPI_SCK = 1;			
	
		}		
		
		color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
		
		SPI_CS=1;	
		
	GPIOC->CRH&=0X000FFFFF;
	GPIOC->CRH|=0X33300000;//SDI = PC13  SCK = PC14  RST = PC15 	
	GPIOC->ODR|=0XE000;    //IO上拉
		
		
return color;
}		



//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	  	  
	for(index=0;index<totalpoint;index++)LCD_WriteRAM(color);
}  
//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;

		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
		 	LCD_Set_Window(sx,i,lcddev.width,lcddev.height);     //设置光标位置 
			LCD_WriteRAM_Prepare();     			//开始写入GRAM	  
			for(j=0;j<xlen;j++)LCD_WriteRAM(color);	//设置光标位置 	    
		}
	
}  
//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)LCD_WriteRAM(color[i*width+j]);//写入数据 
	}  
}
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  	LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }
}


//汉字 16*16
void GUI_DrawFont16(u16 x, u16 y,u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//自动统计汉字数目
	
			
	for (k=0;k<HZnum;k++) 
	{
	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
	  { 	LCD_Set_Window(x,y,16,16);
				LCD_WriteRAM_Prepare();
		    for(i=0;i<16*2;i++)
		    {
				for(j=0;j<8;j++)
		    	{	
					if(!mode) //非叠加方式
					{
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_WriteRAM(POINT_COLOR);
						else LCD_WriteRAM(BACK_COLOR);
					}
					else
					{
						//POINT_COLOR=fc;
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
						x++;
						if((x-x0)==16)
						{
							x=x0;
							y++;
							break;
						}
					}

				}
				
			}
			
			
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//恢复窗口为全屏  
} 

//汉字 24*24
void GUI_DrawFont24(u16 x, u16 y, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//自动统计汉字数目
		
			for (k=0;k<HZnum;k++) 
			{
			  if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
			  { 	LCD_Set_Window(x,y,24,24);
						LCD_WriteRAM_Prepare();
				    for(i=0;i<24*3;i++)
				    {
							for(j=0;j<8;j++)
							{
								if(!mode) //非叠加方式
								{
									if(tfont24[k].Msk[i]&(0x80>>j))	LCD_WriteRAM(POINT_COLOR);
									else LCD_WriteRAM(BACK_COLOR);
								}
							else
							{
								//POINT_COLOR=fc;
								if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
								x++;
								if((x-x0)==24)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}
					
					
				}				  	
				continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
			}

	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//恢复窗口为全屏  
}

//汉字 32*32
void GUI_DrawFont32(u16 x, u16 y, u8 *s,u8 mode)
{
	u8 i,j;
	u16 k;
	u16 HZnum;
	u16 x0=x;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//自动统计汉字数目
	for (k=0;k<HZnum;k++) 
			{
			  if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
			  { 	LCD_Set_Window(x,y,32,32);
						LCD_WriteRAM_Prepare();
				    for(i=0;i<32*4;i++)
				    {
						for(j=0;j<8;j++)
				    	{
							if(!mode) //非叠加方式
							{
								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_WriteRAM(POINT_COLOR);
								else LCD_WriteRAM(BACK_COLOR);
							}
							else
							{
								//POINT_COLOR=fc;
								if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
								x++;
								if((x-x0)==32)
								{
									x=x0;
									y++;
									break;
								}
							}
						}
					}
					
					
				}				  	
				continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
			}
	
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//恢复窗口为全屏  
} 



//显示汉字或者字符串
void Show_Str(u16 x, u16 y,u8 *str,u8 size,u8 mode)
{			
	u16 x0=x;							  	  
  	u8 bHz=0;     //字符或者中文 
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size)) 
			return; 
	        if(*str>0x80)bHz=1;//中文 
	        else              //字符
	        {          
		        if(*str==0x0D)//换行符号
		        {         
		            y+=size;
		            x=x0;
		            str++; 
		        }  
		        else
				{
					if(size>=24)//字库中没有集成12X24 16X32的英文字体,用8X16代替
					{ 						
					LCD_ShowChar(x,y,*str,24,mode);
					x+=12; //字符,为全字的一半 
					}
					else
					{
					LCD_ShowChar(x,y,*str,size,mode);
					x+=size/2; //字符,为全字的一半 
					}
				} 
				str++; 
		        
	        }
        }else//中文 
        {   
				if(x>(lcddev.width-size)||y>(lcddev.height-size)) 
				return;  
							bHz=0;//有汉字库    
				if(size==32)
				GUI_DrawFont32(x,y,str,mode);	 	
				else if(size==24)
				GUI_DrawFont24(x,y,str,mode);	
				else
				GUI_DrawFont16(x,y,str,mode);
					
						str+=2; 
						x+=size;//下一个汉字偏移	    
        }						 
    }   
}


//显示40*40图片
void Gui_Drawbmp16(u16 x,u16 y,const unsigned char *p) //显示40*40图片
{
  	int i; 
	unsigned char picH,picL; 
	LCD_Set_Window(x,y,40,40);
	LCD_WriteRAM_Prepare();	
	
    for(i=0;i<40*40;i++)
	{	
	 	picL=*(p+i*2);	//数据低位在前
		picH=*(p+i*2+1);				
		LCD_WriteRAM(picH<<8|picL);  						
	}	
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//恢复显示窗口为全屏	

}

//居中显示
void Gui_StrCenter(u16 x, u16 y, u8 *str,u8 size,u8 mode)
{
	u16 x1;
	u16 len=strlen((const char *)str);
	if(size>16)
	{
		x1=(lcddev.width-len*(size/2))/2;
	}else
	{
		x1=(lcddev.width-len*8)/2;
	}
	
	Show_Str(x+x1,y,str,size,mode);
} 


void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//设置字体为蓝色
	BACK_COLOR=WHITE;
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  POINT_COLOR=RED;//设置画笔蓝色 
}
////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  

//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}































