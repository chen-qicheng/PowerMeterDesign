#include "w25qx.h"
#include "sys.h"
#include "lcd.h"
#include "touch.h"


//��ʼ��SPI_FLASH
void W25QX_init(void)
{
		RCC->APB2ENR|=1<<2;    //PAʱ��ʹ��
		JTAG_Set(SWD_ENABLE);  //����SWD
	
	  GPIOA->CRL&=0XFFFFFF00;
		GPIOA->CRL|=0X00000083;
		GPIOA->CRH&=0X0FFFFF00;
		GPIOA->CRH|=0X30000033;
		GPIOA->ODR|=0X0303;     
}
//SPI_FLASH д
void SPI_Write(u8 data) 
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        F_SCLK=0;
        if(data&0x80)
            F_DIO=1;
        else
            F_DIO=0;
        F_SCLK=1;
        data<<=1;
     }
} 
//SPI_FLASH ��
u8 SPI_Read(void) 
{
    u8 buff=0x00,i;
    for(i=0;i<8;i++)
    {
        F_SCLK=0;
        buff<<=1;
        if(DO_Read)
            buff|=0x01;
        F_SCLK=1;
    }
    return buff;
}
//SPI_FLASHдʹ��	
//��WEL��λ 
void Write_Enable(void) 
{
    F_CS=0;
    SPI_Write(W25X_WriteEnable);
    F_CS=1;
}
//SPI_FLASHд��ֹ	
//��WEL����  
void Write_Disable(void) 
{
    F_CS=0;
     SPI_Write(W25X_WriteDisable);
        F_CS=1;
}
//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 W25Q16_ReadSR(void) 
{
    uint8_t flag;
    F_CS=0;
    SPI_Write(W25X_ReadStatusReg);
    flag=SPI_Read();
    F_CS=1;
    flag&=0x01;
    return flag;
}

#ifndef MEM_ALLOC_TABLE_SIZE
u8 SPI_FLASH_BUFFER[4096];
#endif 
void W25Q16_Write(uint8_t *data,uint32_t address,u16 NumByteToWrite)
{

	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * SPI_FLASH_BUF;	  
#ifdef MEM_ALLOC_TABLE_SIZE			
	SPI_FLASH_BUF=mymalloc(4096);	//ʹ���ڴ���� 
	if(SPI_FLASH_BUF==NULL)return;	//����ʧ��
#else
	SPI_FLASH_BUF=SPI_FLASH_BUFFER;	//��ʹ���ڴ����
#endif	
 	secpos=address/4096;//������ַ  
	secoff=address%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		W25Q16_Read(SPI_FLASH_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			W25QX_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				SPI_FLASH_BUF[i+secoff]=data[i];	  
			}
			W25QX_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//д����������  

		}else W25QX_Write_NoCheck(data,address,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 
		if(NumByteToWrite==secremain)
		{
			//myfree(SPI_FLASH_BUF);
			break;//д�������
		}
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	data+=secremain;  //ָ��ƫ��
			address+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	};		
	
#ifdef MEM_ALLOC_TABLE_SIZE			
	myfree(SPI_FLASH_BUF);		//�ͷ��ڴ�	 	 
#endif	
		
}
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QX_Write_NoCheck(u8* data,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QX_Flash_Write_Page(data,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			data+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
}
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QX_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    Write_Enable();                  //SET WEL 
		F_CS=0;                            //ʹ������   
    SPI_Write(W25X_PageProgram);      //����дҳ����   
    SPI_Write((u8)((WriteAddr)>>16)); //����24bit��ַ    
    SPI_Write((u8)((WriteAddr)>>8));   
    SPI_Write((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI_Write(pBuffer[i]);//ѭ��д��  
		F_CS=1;                           //ȡ��Ƭѡ 
		while(W25Q16_ReadSR()); 					   //�ȴ�д�����
} 
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QX_Erase_Sector(u32 Dst_Addr)   
{    
		Dst_Addr*=4096;
    Write_Enable();                  //SET WEL 	 
    while(W25Q16_ReadSR());   
  	F_CS=0;                            //ʹ������   
    SPI_Write(W25X_SectorErase);      //������������ָ�� 
    SPI_Write((u8)((Dst_Addr)>>16));  //����24bit��ַ    
    SPI_Write((u8)((Dst_Addr)>>8));   
    SPI_Write((u8)Dst_Addr);  
		F_CS=1;                            //ȡ��Ƭѡ     	      
    while(W25Q16_ReadSR());   				   //�ȴ��������
}
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//data:���ݴ洢��
//address:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25Q16_Read(uint8_t *data,uint32_t address,u16 NumByteToRead)
{
    u16 i;
    while(W25Q16_ReadSR());
    F_CS=0;
    SPI_Write(W25X_ReadData);
    SPI_Write(address>>16);
    SPI_Write(address>>8);
    SPI_Write(address);
    for(i=0;i<NumByteToRead;i++)
    {
        data[i]=SPI_Read();
    }
    F_CS=1;
}
//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25q16_Erasure()
{
    Write_Enable();
    SPI_Write(0x00);
    Write_Enable();
    while(W25Q16_ReadSR());
    F_CS=0;
    SPI_Write(W25X_ChipErase);
    F_CS=1;
    while(W25Q16_ReadSR());
}
//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
u16 W25QX_Flash_ReadID(void)
{
	u16 Temp=0;
	F_CS=0;
	SPI_Write(0X90);
	SPI_Write(0X00);
	SPI_Write(0X00);
	SPI_Write(0X00);
	Temp|=(SPI_Read()<<8);
	Temp|=SPI_Read();
	F_CS=1;

	return Temp;
}

