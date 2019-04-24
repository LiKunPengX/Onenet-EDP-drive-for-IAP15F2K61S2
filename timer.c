#include "STC15FXX.H"
#include "HARDWARE\timer.h"
extern unsigned char allowsend;
extern unsigned char th,tl,hu;
unsigned char intercnt;
void    Timer0_init(void)
{
        TR0 = 0;    //停止计数
		IP=0X10;
    #if (Timer0_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer0设置的中断过快!"

    #elif ((Timer0_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET0 = 1;    //允许中断
    //  PT0 = 1;    //高优先级中断
        TMOD &= ~0x03;
        TMOD |= 0;  //工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
    //  TMOD |=  0x04;  //对外计数或分频
        TMOD &= ~0x04;  //定时
    //  INT_CLKO |=  0x01;  //输出时钟
        INT_CLKO &= ~0x01;  //不输出时钟

        #if (Timer0_Reload < 65536UL)
            AUXR |=  0x80;  //1T mode
            TH0 = (unsigned char)((65536UL - Timer0_Reload) / 256);
            TL0 = (unsigned char)((65536UL - Timer0_Reload) % 256);
        #else
            AUXR &= ~0x80;  //12T mode
            TH0 = (unsigned char)((65536UL - Timer0_Reload/12) / 256);
            TL0 = (unsigned char)((65536UL - Timer0_Reload/12) % 256);
        #endif

        TR0 = 1;    //开始运行
		EA=1;
    #else
        #error "Timer0设置的中断过慢!"
    #endif
}
void timer0 (void) interrupt 1
{
  	intercnt++;
	if(intercnt>=200)//2 second
	{
		intercnt=0;
		send(th,tl,hu);
	}
		serial_pro();
}