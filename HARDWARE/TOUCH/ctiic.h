#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H
#include "sys.h"	    

 	 
//IO方向设置
#define CT_SDA_IN()  {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=8<<4*4;}
#define CT_SDA_OUT() {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=3<<4*4;}


//IO操作函数	 
#define CT_IIC_SCL    PAout(6) 			//SCL     
#define CT_IIC_SDA    PAout(4) 			//SDA	 
#define CT_READ_SDA   PAin(4)  			//输入SDA 
 

//IIC所有操作函数
void CT_IIC_Init(void);                	//初始化IIC的IO口				 
void CT_IIC_Start(void);				//发送IIC开始信号
void CT_IIC_Stop(void);	  				//发送IIC停止信号
void CT_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 CT_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
u8 CT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void CT_IIC_Ack(void);					//IIC发送ACK信号
void CT_IIC_NAck(void);					//IIC不发送ACK信号

#endif







