#include "STC15FXX.H"
#include "HARDWARE\onewire.h"
#include "HARDWARE\timer.h"
#include "string.h"
#define bufnummax 60
unsigned char bufnum=0;
unsigned char failure=0,reccnt=0;
unsigned char recflag=0,inited=0;
unsigned char th,tl,ts,hu=0,le=1;
//unsigned char code bittable[8]={0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x01};
//unsigned char code displaytable[18]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0XBF,0XFF};
//unsigned char displaybuf[8]={2,3,3,3,3,3,3,3};
xdata unsigned char buflog[bufnummax];
xdata unsigned char reclog[60];
xdata unsigned char con_dat[51]={0X10,0X31,0X00,0X03,0X45,0X44,0X50,0X01,
						   		 0X40,0X01,0X2C,0X00,0X08,0X33,0X36,0X37,
						   		 0X31,0X38,0X39,0X37,0X38,0X00,0X1C,0X73,
						   		 0X55,0X48,0X79,0X61,0X6E,0X4D,0X41,0X78,
						  		 0X66,0X78,0X6B,0X35,0X45,0X53,0X35,0X54,
						 		 0X6D,0X59,0X61,0X46,0X6C,0X53,0X74,0X6E,
						 		 0X42,0X4D,0X3D};
xdata unsigned char send_a[87]={0X80,0XEA,0X01,0X80,0X00,0X08,0X33,0X36,
								0X37,0X31,0X38,0X39,0X37,0X38,0X01,0X00,
								0XDC,0X7B,0X0A,0X09,0X22,0X64,0X61,0X74,
								0X61,0X73,0X74,0X72,0X65,0X61,0X6D,0X73,
								0X22,0X3A,0X09,0X5B,0X7B,0X0A,0X09,0X09,
								0X09,0X22,0X69,0X64,0X22,0X3A,0X09,0X22,
								0X54,0X22,0X2C,0X0A,0X09,0X09,0X09,0X22,
								0X64,0X61,0X74,0X61,0X70,0X6F,0X69,0X6E,
								0X74,0X73,0X22,0X3A,0X09,0X5B,0X7B,0X0A,
								0X09,0X09,0X09,0X09,0X09,0X22,0X76,0X61,
								0X6C,0X75,0X65,0X22,0X3A,0X09,0X22};//2
xdata unsigned char send_b[65]={0X22,0X0A,0X09,0X09,0X09,0X09,0X7D,0X5D,
								0X0A,0X09,0X09,0X7D,0X2C,0X20,0X7B,0X0A,
								0X09,0X09,0X09,0X22,0X69,0X64,0X22,0X3A,
								0X09,0X22,0X43,0X22,0X2C,0X0A,0X09,0X09,
								0X09,0X22,0X64,0X61,0X74,0X61,0X70,0X6F,
								0X69,0X6E,0X74,0X73,0X22,0X3A,0X09,0X5B,
								0X7B,0X0A,0X09,0X09,0X09,0X09,0X09,0X22,
								0X76,0X61,0X6C,0X75,0X65,0X22,0X3A,0X09,
								0X22};//2
xdata unsigned char send_c[65]={0X22,0X0A,0X09,0X09,0X09,0X09,0X7D,0X5D,
								0X0A,0X09,0X09,0X7D,0X2C,0X20,0X7B,0X0A,
								0X09,0X09,0X09,0X22,0X69,0X64,0X22,0X3A,
								0X09,0X22,0X53,0X22,0X2C,0X0A,0X09,0X09,
								0X09,0X22,0X64,0X61,0X74,0X61,0X70,0X6F,
								0X69,0X6E,0X74,0X73,0X22,0X3A,0X09,0X5B,
								0X7B,0X0A,0X09,0X09,0X09,0X09,0X09,0X22,
								0X76,0X61,0X6C,0X75,0X65,0X22,0X3A,0X09,
								0X22};//1
xdata unsigned char send_d[15]={0X22,0X0A,0X09,0X09,0X09,0X09,0X7D,0X5D,
								0X0A,0X09,0X09,0X7D,0X5D,0X0A,0X7D};

void UartInit(void)		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xE0;		//设定定时初值
	TH1 = 0xFE;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	SM0=0;
	SM1=1;
	REN=1;
	EA=1;
	ES=1;
}
void decode(unsigned char cde)
{
	switch(cde)
	{
		case 0xa0:P25=0;P26=0;P27=0;break;
		case 0xa1:P25=1;P26=0;P27=0;break;
		case 0xa2:P25=0;P26=1;P27=0;break;
		case 0xa3:P25=1;P26=1;P27=0;break;
		case 0xa4:P25=0;P26=0;P27=1;break;
		case 0xa5:P25=1;P26=0;P27=1;break;
		case 0xa6:P25=0;P26=1;P27=1;break;
		case 0xa7:P25=1;P26=1;P27=1;break;
		default:break;
	}	
}
void delayus(unsigned int i)  //1us
{
	while(i--);
}
void Uart1Send(unsigned char c)
{
	SBUF=c;
	while(!TI);
	TI=0;	
}
/*****************************串口发送字符串************************************************/
void Uart1Sends(unsigned char *str)
{
	while(*str!='\0')
	{
		SBUF=*str;
		while(!TI);
		TI=0;
		str++;
	}
}
/*****************************串口缓存数组清空*******************************/
void CLR_Buf(void)
{
	unsigned char k;
	for(k=0;k<bufnummax;k++)
	{
		buflog[k] = 0;
	}
    bufnum=0;   
}
/*****************************串口中断处理************************************************/
void Serial_interrupt() interrupt 4 
{
	unsigned char rtemp;
	EA=0;
	rtemp=SBUF;
	
		if((rtemp==0x40)&&inited)
		{
			WDT_CONTR=0x25;
		}

			if((rtemp==0xa0)&&inited)
			{																	
				recflag=1;
				reccnt=0;
			}
			if(recflag==1)
			{
				reclog[reccnt++]=rtemp;
				if(reccnt>=49)
				{
					reccnt=0;
					recflag=2;
				}
			}
			
				buflog[bufnum++]=rtemp;	
				if(bufnum>=bufnummax)
				{
					bufnum=0;
				}
				else
				{
					RI=0;
				}
				EA=1;
}
/*****************************查找串口数据缓存************************************************/
unsigned char Find(unsigned char *a)
{ 
	if(strstr(buflog,a)!=NULL)
	{
		return 1;
	}	
	else
	{
		return 0;
	}
		
}
void delayms(unsigned int i)  //1us
{
	while(i--);
	while(i--);
	while(i--);
	while(i--);
	while(i--);
	while(i--);
	while(i--);
	while(i--);
	while(i--);
	while(i--);
	while(i--);
	while(i--);
}
/*****************************高级串口发送************************************************/
unsigned char UART1_Send_AT_Command(unsigned char *b,unsigned char *a,unsigned int interval_time)         
{
	unsigned char i;
	CLR_Buf(); 
	i = 0;
	Uart1Sends(b);
	delayms(interval_time);
	if(Find(a))    
	{
		return 1;
	}
	else return 0;
}
void set_online()
{
	unsigned char cnt;
	for(cnt=0;cnt<51;cnt++)
	{
		Uart1Send(con_dat[cnt]);
		delayus(3000);
	}
}
void send_pa()
{
	unsigned char cnt;
	for(cnt=0;cnt<87;cnt++)
	{
		Uart1Send(send_a[cnt]);
		delayus(3000);
	}
}
void send_pb()
{
	unsigned char cnt;
	for(cnt=0;cnt<65;cnt++)
	{
		Uart1Send(send_b[cnt]);
		delayus(3000);
	}
}
void send_pc()
{
	unsigned char cnt;
	for(cnt=0;cnt<65;cnt++)
	{
		Uart1Send(send_c[cnt]);
		delayus(3000);
	}
}
void send_pd()
{
	unsigned char cnt;
	for(cnt=0;cnt<15;cnt++)
	{
		Uart1Send(send_d[cnt]);
		delayus(3000);
	}
}
void send(unsigned char tph,tpl,hul)
{
		send_pa();
		switch(tph)
		{
			case 0:Uart1Send(0x30);break;
			case 1:Uart1Send(0x31);break;
			case 2:Uart1Send(0x32);break;
			case 3:Uart1Send(0x33);break;
			case 4:Uart1Send(0x34);break;
			case 5:Uart1Send(0x35);break;
			case 6:Uart1Send(0x36);break;
			case 7:Uart1Send(0x37);break;
			case 8:Uart1Send(0x38);break;
			case 9:Uart1Send(0x39);break;
			default:Uart1Send(0x30);break;
		}
		switch(tpl)
		{
			case 0:Uart1Send(0x30);break;
			case 1:Uart1Send(0x31);break;
			case 2:Uart1Send(0x32);break;
			case 3:Uart1Send(0x33);break;
			case 4:Uart1Send(0x34);break;
			case 5:Uart1Send(0x35);break;
			case 6:Uart1Send(0x36);break;
			case 7:Uart1Send(0x37);break;
			case 8:Uart1Send(0x38);break;
			case 9:Uart1Send(0x39);break;
			default:Uart1Send(0x30);break;
		}
		send_pb();
		Uart1Send(0x31);
		Uart1Send(0x38);
		send_pc();
		switch(hul)
		{
			case 0:Uart1Send(0x30);break;
			case 1:Uart1Send(0x31);break;
			default:Uart1Send(0x30);break;
		}
		send_pd();
}
void esp_init()
{
	unsigned char ret=0;
	while(!ret)
	{
	   	ret = UART1_Send_AT_Command("AT\r\n","OK",2500);
	}
	ret=0;
	while(!ret)
	{
	   	ret = UART1_Send_AT_Command("AT+CWMODE=1\r\n","OK",2500);
	}
	ret=0;
	
	while(!ret)
	{
	   	ret = UART1_Send_AT_Command("AT+CWJAP=\"DQCK\",\"dqckdqck\"\r\n","GOT IP",50000);
	}
	ret=0;
	while(!ret)
	{
	   	ret = UART1_Send_AT_Command("AT+CIPMUX=0\r\n","OK",2500);
	}
	ret=0;
	CLR_Buf();
	while(!ret)
	{
   		ret = UART1_Send_AT_Command("AT+CIPSTART=\"TCP\",\"jjfaedp.hedevice.com\",876\r\n","CONNECT",5500);
		//ret = UART1_Send_AT_Command("AT+CIPSTART=\"TCP\",\"192.168.1.100\",876","OK",3500);
	}
	ret=0;
	while(!ret)
	{
	   	ret = UART1_Send_AT_Command("AT+CIPMODE=1\r\n","OK",2500);
	}
	ret=0;
	CLR_Buf(); 
	while(!ret)
	{
	   	ret = UART1_Send_AT_Command("AT+CIPSEND\r\n",">",5500);
	}
	ret=0;
	delayms(2500);	
}
void serial_pro()
{
			if(recflag==2)
			{
				for(reccnt=0;reccnt<49;reccnt++)
				{
					if(reclog[reccnt]==0x00)recflag=3;
					if((reclog[reccnt]==0x31)&&(recflag==3))
					{
						decode(0XA5);P0|=0x50;decode(0XA2);P0=0Xff;delayus(5);decode(0XA4);P0=0xff;decode(0XA0);	
					}
					else if((reclog[reccnt]==0x32)&&(recflag==3))
					{
						decode(0XA5);P0&=0Xef;decode(0XA2);P0=0Xff;delayus(5);decode(0XA4);P0=0xff;decode(0XA0);
					}
				}
//					for(reccnt=0;reccnt<49;reccnt++)
//					{
//						reclog[reccnt]=0;
//					}
					 recflag=0; reccnt=0;	
			}
}

//void dispdelay(unsigned int t)
//{
//	while(t--);
//}
//void disp()
//{
//	unsigned char dcnt;
//	for(dcnt=0;dcnt<8;dcnt++)
//	{
//		P0=displaytable[displaybuf[dcnt]];
//		decode(0xa7);
//		dispdelay(100);
//		P0=0xff;
//		decode(0xa2);
//
//		P0=bittable[dcnt];
//		decode(0xa6);
//		dispdelay(20);
//		decode(0xa2);
//	}
//}
void pdetect()
{
	if(le)
		{
			if(P35==0)
			{
				delayus(5);
				if(P35==0)
				{
					hu=1;	
				}		
			}
				else if(P35==1)
				{
					hu=0;
				}
		}	
}
//void keyscan()
//{
//		if(P33==0)
//		{
//			delayus(5);
//			if(P33==0)
//			{
//				if(le==0)
//				{
//					le=1;
//					decode(0XA4);
//					P0=0XFF;
//					decode(0XA0);
//				}
//				else if(le==1)
//				{
//					decode(0XA4);
//					P0=0XAF;
//					decode(0XA2);
//					le=0;
//					hu=0;
//				}
//			}
//			while(!P33);
//		}
//}
void main()
{

	decode(0XA5);
	P0=0X00;
	decode(0XA0);
	delayus(5);
	decode(0XA4);
	P0=0XFF;
	decode(0XA0);
	delayus(5);
	UartInit();
	delayus(1500);
	esp_init();
	delayms(2500);
	set_online();
	delayms(2500);
	set_online();
	delayms(2500);
	set_online();
	inited=1;
	Timer0_init();
	while(1)
	{
		//keyscan();
		pdetect();
		ts=datpro();
		th=ts/10;
		tl=ts%10;
//		disp();
	}
}