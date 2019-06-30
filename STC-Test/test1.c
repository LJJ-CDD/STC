//**** 声明 ********************************************************************
/*******************************************************************************
 * 下面来自互联开源程序，由广州汇承信息科技有限公司收集
 * 方便用户参考学习，本公司不提供任何技术支持
 * 程序仅供测试参考，不能应用在实际工程中，不一定能通过编译
 * 公司网站  http://www.hc01.com/
 * 淘宝网址  http://hc-com.taobao.com/
*******************************************************************************/
               		       
/*-----------------------------------------------------------------------------
* 实 验 名		 : RS232串口试验
* 实验说明     : 单片机串口接收数据控制灯亮灭
* 连接方式     : 
* 注    意		 : 该试验使用的晶振是11.0592
*******************************************************************************/

#include<reg52.h>
#include<intrins.h>

#define uchar unsigned char
#define uint  unsigned int

char PuZh[10] = "LJJ-6.62-0";

uchar code  LedShowData[]={0x03,0x9F,0x25,0x0D,0x99,0x49,0x41,0x1F,0x01,0x19};
//0,1,2,3,4,5,6,7,8,9 //定义数码管显示数据

//--定义使用的IO--//
/*************完成电机控制端口定义***********/
sbit M1A=P1^2;                                        //定义电机1正向端口
sbit M1B=P1^3;                                        //定义电机1反向端口
sbit M2A=P1^4;                                        //定义电机2正向端口
sbit M2B=P1^5;                                        //定义电机2反向端口

sbit BUZZ=P2^3; 
sbit S1=P3^2;                                         //定义S1按键端口

#define ShowPort P0                                   //定义数码管显示端口

//--声明全局函数--//   
void UsartConfiguration();
void Delay_1ms(uint i);
void delayms(uint );
void Delay1ms_XC(unsigned int i) ;
void delayms_XC(unsigned char x);                         //0.14mS延时程序
void zuozhuan();
void youzhuan();
void houtui();
void qianjin();
void tingzhi();
void BUZZ_1();
void UART0SendByte(unsigned char c);
void UART0SendString(char *str);
void ControlCar(unsigned char ConType);    //定义电机控制子程序
	uchar a;


/********************************************************************
* 名称 : Com_Int()
* 功能 : 串口中断子函数
* 输入 : 无 * 输出 : 无
***********************************************************************/
void Com_Int(void) interrupt 4
{
	uchar i;
  uchar receive_data;
  EA = 0;
	
  if(RI == 1) //当硬件接收到一个数据时，RI会置位
	{ 		
		RI = 0;
		receive_data = SBUF;//接收到的数据
		if(receive_data == '1')	 
		{
				ShowPort=LedShowData[1];
				tingzhi();						      //进入前进之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
				delayms_XC(240);
				qianjin();
				BUZZ_1();
				PuZh[9]='1';
		}
		if(receive_data == '2')	 
		{
					ShowPort=LedShowData[2];
			    tingzhi();							      //进入后退之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
					delayms_XC(240); 
					houtui();
					BUZZ_1();
					PuZh[9]='2';
		}		
		if(receive_data == '3')	 
		{
					ShowPort=LedShowData[3];
			    tingzhi();								  //进入左转之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
					delayms_XC(240); 
					zuozhuan();
					BUZZ_1();
					PuZh[9]='3';
		}
		if(receive_data == '4')	 
		{
					ShowPort=LedShowData[4];
			    tingzhi();								  //进入右转之前 先停止一段时间  防止电机反向电压冲击主板 导致系统复位
					delayms_XC(240);
					youzhuan(); 
					BUZZ_1();
					PuZh[9]='4';
		}
		if(receive_data == '5')	 
		{
					ShowPort=LedShowData[5];
			    tingzhi();
					BUZZ_1();
					PuZh[9]='5';
		}		
	}
	
		for(i=0; i<10; i++)
		{
			SBUF = PuZh[i];   //将要发送的数据放入到发送寄存器
			while(!TI);		    //等待发送数据完成
			TI=0;			        //清除发送完成标志位
			Delay_1ms(1);
		}
	
	//UART0SendString(PuZh);
		EA = 1;
}

/*******************************************************************************
* 函 数 名       : main
* 函数功能		   : 主函数
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/

void main()
{
	unsigned char i;
	ShowPort=LedShowData[0];                             //数码管显示数字0
	B:		for(i=0;i<50;i++) //判断K4是否按下
		{
		   Delay_1ms(1);	//1ms内判断50次，如果其中有一次被判断到K0没按下，便重新检测
		   if(S1!=0)//当S1按下时，启动小车前进
		   goto B; //跳转到标号B，重新检测  
		} 
	BUZZ_1();	
	UsartConfiguration();//初始化串口
	
	while(1);	
}
	
/*******************************************************************************
* 函 数 名       : UsartConfiguration()
* 函数功能		   : 设置串口
* 输    入       : 无
* 输    出       : 无
*******************************************************************************/

void UsartConfiguration()//设值9600bps的程序，大家可以试试
{
        SCON=0X50; //设置串口工作模式为10位异步收发器               
        T2CON=0X34; //RCLK=1,TCLK=1,TR2=1               
        PCON=0X80; //SMOD=1               
        RCAP2H=(65536-39)/256; // 装入计算好的值，这是我喜欢的懒方法
        RCAP2L=(65536-39)%256;//同上，不过这是对（65536-39）求余，前者是求模（也就是求商）
        ES=1;//开串口中断 
        EA=1;//开总中断
}


/********************************************************************
* 名称 : Delay_1ms()
* 功能 : 延时子程序，延时时间为 1ms
* x * 输入 : x (延时一毫秒的个数)
* 输出 : 无
***********************************************************************/
void Delay_1ms(uint i)//1ms延时
{
  uchar x,j;
  
  for(j=0;j<i;j++)
    for(x=0;x<=148;x++);
}

void delayms(uint xms)
{
	uint i,j;
	for(i=xms;i>0;i--)
		for(j=110;j>0;j--);
}

void delayms_XC(unsigned char x)                         //0.14mS延时程序
{
  unsigned char i;                                    //定义临时变量
  while(x--)                                          //延时时间循环
  {
    for (i = 0; i<13; i++) {}                         //14mS延时
  }
}

void Delay1ms_XC(unsigned int i) 
{ 
unsigned char j,k; 
do{ 
  j = 10; 
  do{ 
   k = 50; 
   do{ 
    _nop_(); 
   }while(--k);     
  }while(--j); 
}while(--i); 
}



void tingzhi()
{
   M1A=0;                                   //将M1电机A端初始化为0
   M1B=0;                                   //将M1电机B端初始化为0
   M2A=0;                                   //将M2电机A端初始化为0
   M2B=0;

}

void qianjin()
{
   M1A=1;                                   
   M1B=0;                                   
   M2A=1;                                   
   M2B=0;
}

void houtui()
{
   M1A=0;                                   
   M1B=1;                                   
   M2A=0;                                   
   M2B=1;
}

void youzhuan()
{
   M1A=1;                                   
   M1B=0;                                   
   M2A=0;                                   
   M2B=1;

}

void zuozhuan()
{
   M1A=0;                                   
   M1B=1;                                   
   M2A=1;                                   
   M2B=0;

}

void BUZZ_1()
{
	 BUZZ=0;	//50次检测K0确认是按下之后，蜂鸣器发出“滴”声响，然后启动小车。
	 Delay_1ms(50);
	 BUZZ=1;//响50ms后关闭蜂鸣器
}

/*********************************************************************
 * 函数名称：UART0SendByte
 * 功    能：UART0发送一个字节
 * 入口参数：c
 * 出口参数：无
 * 返 回 值：无
 ********************************************************************/
void UART0SendByte(unsigned char c)
{
  SBUF = c;       // 将要发送的1字节数据写入U0DBUF(串口 0 收发缓冲器)  
  while (!TI);  // 等待TX中断标志，即U0DBUF就绪
  TI = 0;       // 清零TX中断标志 
}


/*********************************************************************
 * 函数名称：UART0SendString
 * 功    能：UART0发送一个字符串
 * 入口参数：无
 * 出口参数：无
 * 返 回 值：无
 ********************************************************************/
void UART0SendString(char *str)
{
  while(1)
  {
    if(*str == '\0') break;  // 遇到结束符，退出
    UART0SendByte(*str++);   // 发送一字节
  } 
}
