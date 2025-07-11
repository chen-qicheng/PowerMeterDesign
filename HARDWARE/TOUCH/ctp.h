#ifndef __CTP_H
#define __CTP_H	
#include "sys.h"	



//����ݴ��������ӵ�оƬ����(δ����IIC����) 
//IO��������	 
#define FT_RST    				PAout(7)	//FT6336��λ����
#define FT_INT    				PAin(5)		//FT6336�ж�����	


//I2C��д����	
#define FT_CMD_WR 				0X70    	//д����
#define FT_CMD_RD 				0X71		//������
  
//FT6336 ���ּĴ������� 
#define FT_REG_NUM_FINGER       0x02		//����״̬�Ĵ���

#define FT_TP1_REG 				0X03	  //��һ�����������ݵ�ַ
#define FT_TP2_REG 				0X09		//�ڶ������������ݵ�ַ
#define FT_TP3_REG 				0X0F		//���������������ݵ�ַ
#define FT_TP4_REG 				0X15		//���ĸ����������ݵ�ַ
#define FT_TP5_REG 				0X1B		//��������������ݵ�ַ  
 


u8 FT6336_WR_Reg(u16 reg,u8 *buf,u8 len);
void FT6336_RD_Reg(u16 reg,u8 *buf,u8 len);
void FT6336_Init(void);
u8 FT6336_Scan(u8 mode);

#endif

















