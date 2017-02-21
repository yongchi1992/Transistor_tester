#include <msp430f5438a.h>
#include "initialization.h"
#include "power.h"
#include "KeyBoardDriver.h"
#include "LCD.h"
#include "AD.h"
#include "DA.h"
#include "clock.h"
#include "rtc.h"

const unsigned char hang1[2][17] = {"晶体管类型: PNP ","晶体管类型: NPN "};
const unsigned char hang2[2][17] = {"Ie额定: ","  mA"};
const unsigned char hang3[3][17] = {"并适当调整Ie大小","    运行错误！  ","直流β：        "};
const unsigned char hang4[3][17] = {"请按¤键获得帮助","请检查引脚与极性","交流β：        "};
const unsigned char RESET_CONFIRM[4][17] = {"是否初始化程序？","这将会改变时钟！","  确认请长按★  ","  取消请短按※  "};
const unsigned char year[] = {"年"};
const unsigned char month[] = {"月"};
const unsigned char day[] = {"日"};
const unsigned char week[] = {"星期"};
const unsigned char date[7][3] = {"一","二","三","四","五","六","日"};
const unsigned char time[2][17] = {"用        键修改",
								   "√确认    ※取消",};
const unsigned char help[23][17] = {"",
									"欢迎使用晶体管  ",
									"特性测试程序。  ",
									"支持NPN,PNP 两  ",
									"种晶体管，可用  ",
									"  键进行切换。  ",
									"方向键可设定额  ",
									"定通过的Ie电流  ",
									"以防止损坏晶体  ",
									"管，因外部硬件  ",
									"限制，仅能提供  ",
									"0~115mA 的测试  ",
									"电流。Θ键用于  ",
									"测算在所设定的  ",
									"Ie值条件下晶体  ",
									"管的直、交流放  ",
									"大倍数。Ξ键用  ",
									"于测绘β和Ie的  ",
									"关系曲线。┳键  ",
									"用于设定时间。  ",
									"√键为确认键。  ",
									"※键为取消键。  ",
									"    返回请按※  "
									};
extern unsigned char graph[1024];

volatile Calendar newtime,oldtime;
volatile unsigned int ie[3]={30000,30000,30000};
volatile unsigned int ieold[3]={30000,30000,30000};
volatile int ad[3];
//volatile float analog_ve,analog_vb1,analog_vb2;
volatile unsigned int screen,cursor,veset,beta,alpha,key,key_long,flag,position,Its_A_Joke=0,no_answer=0,mode=0,daiji=0;
volatile int de,db,da,delta;

#define zero 30000
#define LINE1 0x80
#define LINE2 0x90
#define LINE3 0x88
#define LINE4 0x98
#define MAXDA 0x0FFF
#define MIDDA 0x0800
#define TOLERANCE 0x0040
#define HALF_T 0x0020
#define DELTA 0x0010
#define UP 1
#define DOWN -1
#define CONFIRM 0
#define GO_ON 1
#define STOP 0
#define End_Flag   0
#define Wrong_Flag 3
#define Point_Draw  0
#define Point_Clear 1
#define Normal_Display   0
#define Run_Time_Error   1
#define Normal_Output  2
#define Help_User	   3
#define Set_Clock	   4
#define Plot_Graph	   5
#define All_Reset	   6
#define VE ad[0]
#define VB2 ad[1]
#define VB1 ad[2]
#define Cursor_Right cursor ++
#define Cursor_Left cursor --
#define change_mode() mode ^= 1
#define clear(no_answer) no_answer = 0
#define BEEP() P9OUT |= BIT6
#define NO_BEEP() P9OUT &= ~BIT6

void disp(unsigned int screen);

int ConfigureDelta(volatile unsigned int ve0,volatile unsigned int ve)
{
	volatile unsigned int differ;
	if (ve0 == ve) return 0;
	if (ve0 < ve)
		{
			differ = ve - ve0;
			if      (differ < 2+1)   return  0;	 	// means differV in  00mA ~ .5mA
			else if (differ < 4+1)   return -1;	 	// means differV in  .5mA ~ 01mA
			else if (differ < 8+1)   return -4;	 	// means differV in  01mA ~ 02mA
			else if (differ < 20+1)  return -8;	 	// means differV in  02mA ~ 05mA
			else if (differ < 40+1)  return -16; 	// means differV in  05mA ~ 10mA
			else if (differ < 80+1)  return -32;	// means differV in  10mA ~ 20mA
			else if (differ < 160+1) return -64;	// means differV in  20mA ~ 40mA
			else if (differ < 280+1) return -256;  	// means differV in  40mA ~ 70mA
			else 					 return -1024;	// means differV in  70mA ~ 120mA
		}
	else								// veset > ve
		{
			differ = ve0 - ve;
			if      (differ < 2+1)   return 0;	 	// means differV in  00mA ~ .5mA
			else if (differ < 4+1)   return 1;	 	// means differV in  .5mA ~ 01mA
			else if (differ < 8+1)   return 4;	 	// means differV in  01mA ~ 02mA
			else if (differ < 20+1)  return 8;	 	// means differV in  02mA ~ 05mA
			else if (differ < 40+1)  return 16;	 	// means differV in  05mA ~ 10mA
			else if (differ < 80+1)  return 32;		// means differV in  10mA ~ 20mA
			else if (differ < 160+1) return 64;		// means differV in  20mA ~ 40mA
			else if (differ < 280+1) return 256;  	// means differV in  40mA ~ 70mA
			else 					 return 1024;	// means differV in  70mA ~ 120mA
		}
}
/**************************************/
int check(void)
{
	if (no_answer > TOLERANCE) return STOP;
	if (da < 0) da = 0;
	else if (da > MAXDA) da = MAXDA;
	else if (delta == 0) return STOP;
	no_answer += 1;
	return GO_ON;
}
/**************************************/
void measure(void)
{
	if (veset == 2048 )
		{
			beta=0;
			alpha=0;
			//screen = Run_Time_Error;
			flag = 0;
			return;
		}
	delta = 1;
	da = MIDDA;
	clear(no_answer);
	do{
	TLV_5613(da);
	ADS_7950(ad,1);
					//analog(VE)=VTE*5.0/2048-5;
					//analog(VB1)=VTB1*5.0/2048-5;
					//analog(VB2)=VTB2*5.0/2048-5;
	delta = ConfigureDelta(veset,VE);
	da+=delta;
	}while (check());
	if (no_answer > TOLERANCE)
		{
			beta=0;
			alpha=0;
			//screen = Run_Time_Error;
			flag = 0;
			return;
		}
	ADS_7950(ad,3);
	ADS_7950(ad,3);
					//analog_ve=VE*5.0/2048-5;
					//analog_vb1=VB1*5.0/2048-5;
					//analog_vb2=VB2*5.0/2048-5;
	beta = abs((VE-2048)*1.0/(VB1-VB2)*330);
	if ((beta>600)||(beta<100))
		{
		beta=0;
		alpha=0;
		//screen = Run_Time_Error;
		flag = 0;
		return;
		}
					//alpha = abs(330*analog_ve/(analog_vb1-analog_vb2));
	delay_15ms();
}
/*******************************************
函数名称：set_ve
功能：设定初始Ve值（数字量）
参数：UP or DOWN; 返回值：无
********************************************/
void set_ve(int adjust)
{

	if (cursor<3) ie[cursor]+=adjust;

	switch (cursor)
	{
	case 0:
		{
			ie[0] &= 1;
			ie[0] += zero;
			if (ie[0]>zero)
			{
				if (ie[1]>1+zero)
				{
					ie[1]=1+zero;
					ie[2]=5+zero;
				}
			}
			break;
		}
	case 1:
		{
			ie[1] %= 10;
			ie[1] += zero;
			if (ie[0]>zero)
			{
				ie[1] %= 2;
				ie[1] += zero;
				if (ie[2]>5+zero)
				{
					ie[2]=5+zero;
				}
			}
			break;
		}
	case 2:
		{
			ie[2] %= 10;
			ie[2] += zero;
			if ((ie[0]>zero)&&(ie[1]>zero)&&(ie[2]>5+zero))
			{
				ie[2] = 5+zero;
			}
			break;
		}
	default : break;
	}
	veset=(ie[0]-zero)*100+(ie[1]-zero)*10+(ie[2]-zero);
	veset=((mode*2-1)*veset*4+2048);
}
/*******************************************
函数名称：num2char
功能：数字转字符串
参数：int_dec>0,str; 返回值：无
********************************************/
void num2char(volatile unsigned int num,unsigned char *str)
{
	unsigned int tmp=3;

//	*(str+3) = 32;
//	*(str+2) = 32;
//	*(str+1) = 32;
	*(str+0) = 32;
	while (num>0)
	{
		*(str+tmp)=num % 10 + '0';
		num /= 10;
		tmp--;
	}
}
/*******************************************
函数名称：hex2char
功能：数字转字符串
参数：int_hex>0,str; 返回值：无
********************************************/
void hex2char(volatile unsigned int num,unsigned char *str,unsigned int tmp)
{
	*(str+1)='0';
	*(str+0)='0';
	tmp--;
	while (num>0)
	{
		*(str+tmp)=(num & 0x000F) + '0';
		num >>= 4;
		tmp--;
	}
}
/*******************************************
函数名称：char2hex
功能：数字转字符串
参数：str,int_hex>0; 返回值：无
********************************************/
void char2hex(unsigned char *str,volatile unsigned int *num,unsigned int tmp)
{
	unsigned int i = 0;
	*num = 0;
	while (i<tmp)
	{
		*num <<= 4;
		*num += (str[i]-'0');
		i++;
	}
}
/*******************************************
函数名称：char2char
功能：数字转字符串
参数：str,int_hex>0; 返回值：无
********************************************/
void char2char(unsigned char *str,volatile unsigned char *num,unsigned int tmp)
{
	unsigned int i = 0;
	*num = 0;
	while (i<tmp)
	{
		*num <<= 4;
		*num += (str[i]-'0');
		i++;
	}
}
/*******************************************
函数名称：disp
功能：显示屏幕
参数：screen; 返回值：无
********************************************/
void disp(unsigned int screen)
{
	unsigned char str[] ={"   0  "};
	unsigned char str2[] ={"0 "};
	volatile unsigned int tmp;

	flag++;
	if (flag > TOLERANCE) flag = 0;
	switch (screen)
	{
	case Normal_Display:
		{
			Disp_HZ(LINE1,hang1[mode],8);
			Disp_HZ(LINE2,hang2[0],4);
			Disp_HZ(0x96,hang2[1],2);
			Disp_HZ(LINE4,hang4[screen],8);
			tmp=(ie[0]-zero)*100+(ie[1]-zero)*10+(ie[2]-zero);
			num2char(tmp,str);
			if ((cursor<3) && (flag & DELTA != 0)) str[cursor+1] = '_';
			Disp_HZ(0x94,str,2);

			if (flag<HALF_T)
			{
				hex2char(newtime.Year,str,4);
				Disp_HZ(0x88,str,2);
				Disp_HZ(0x8A,year,1);
				Write_Data(0x20);
				delay_50us();
				Write_Data(0x20); 	//"  "两空格
				delay_50us();
				hex2char(newtime.Month,str2,2);
				Disp_HZ(0x8C,str2,1);
				Disp_HZ(0x8D,month,1);
				hex2char(newtime.DayOfMonth,str2,2);
				Disp_HZ(0x8E,str2,1);
				Disp_HZ(0x8F,day,1);
			}
			else
			{
				Disp_HZ(0x88,week,2);
				Disp_HZ(0x8A,date[newtime.DayOfWeek-1],1);
				Write_Data(0x20);
				delay_50us();
				Write_Data(0x20); 	//"  "两空格
				delay_50us();
				hex2char(newtime.Hours,str2,2);
				Disp_HZ(0x8C,str2,1);
				Write_Data(0x3A);					//":"冒号
				delay_50us();
				hex2char(newtime.Minutes,str2,2);
				Write_Data(str2[0]);
				delay_50us();
				Write_Data(str2[1]);
				delay_50us();
				Write_Data(0x3A);					//":"冒号
				delay_50us();
				hex2char(newtime.Seconds,str2,2);
				Disp_HZ(0x8F,str2,1);
			}
			break;
		}
	case Run_Time_Error:
		{
			Disp_HZ(LINE3,hang3[screen],8);
			if (flag<HALF_T)
				{
					Disp_HZ(LINE4,hang4[screen],8);
				}
			else
				{
					Disp_HZ(LINE4,hang3[0],8);
				}
			break;
		}
	case Normal_Output:
		{
			Disp_HZ(LINE3,hang3[screen],8);
			Disp_HZ(LINE4,hang4[screen],8);
			tmp=(ie[0]-zero)*100+(ie[1]-zero)*10+(ie[2]-zero);
			num2char(tmp,str);
			Disp_HZ(0x94,str,2);
			num2char(beta,str);
			Disp_HZ(0x8C,str,3);
			num2char(alpha,str);
			Disp_HZ(0x9C,str,3);
			break;
		}
	case Help_User:
		{
			Disp_HZ(0x80,help[position+0],7);
			Write_Data(0x20);
			delay_50us();
			Write_Data(0x1E); 	//" ▲"
			delay_50us();
			if (position+0 == 5)
			{
				Write_Cmd(0x80);
				delay_50us();
				Write_Data(0x1D);
				delay_50us();
				Write_Data(0x20);
				delay_50us();
			}
			Disp_HZ(0x90,help[position+1],8);
			if (position+1 == 5)
			{
				Write_Cmd(0x90);
				delay_50us();
				Write_Data(0x1D);
				delay_50us();
				Write_Data(0x20);
				delay_50us();
			}
			Disp_HZ(0x88,help[position+2],8);
			if (position+2 == 5)
			{
				Write_Cmd(0x88);
				delay_50us();
				Write_Data(0x1D);
				delay_50us();
				Write_Data(0x20);
				delay_50us();
			}
			Disp_HZ(0x98,help[position+3],7);
			Write_Data(0x20);
			delay_50us();
			Write_Data(0x1F); 	//" ▼"
			delay_50us();
			if (position+3 == 5)
			{
				Write_Cmd(0x98);
				delay_50us();
				Write_Data(0x1D);
				delay_50us();
				Write_Data(0x20);
				delay_50us();
			}
			if (position==1)
				{
					Write_Cmd(0x87);
					Write_Data(0x20);
					delay_50us();
					Write_Data(0x00); 	//"  "
					delay_50us();
				}
			if (position==19)
				{
					Write_Cmd(0x9F);
					Write_Data(0x20);
					delay_50us();
					Write_Data(0x20); 	//"  "
					delay_50us();
				}
			break;
		}
	case Set_Clock:
		{
			Disp_HZ(0x80,time[0],8);
			Disp_HZ(0x90,time[1],8);
			Write_Cmd(0x81);
			delay_50us();
			Write_Data(0x1E);
			delay_50us();
			Write_Data(0x20);
			delay_50us();
			Write_Data(0x1F);
			delay_50us();
			Write_Data(0x20);
			delay_50us();
			Write_Data(0x11);
			delay_50us();
			Write_Data(0x20);
			delay_50us();
			Write_Data(0x10);
			delay_50us();
			Write_Data(0x20);
			delay_50us();
			hex2char(oldtime.Year,str,4);
			Disp_HZ(0x88,str,2);
			Disp_HZ(0x8A,year,1);
			Write_Data(0x20);
			delay_50us();
			Write_Data(0x20); 	//"  "两空格
			delay_50us();
			hex2char(oldtime.Month,str2,2);
			Disp_HZ(0x8C,str2,1);
			Disp_HZ(0x8D,month,1);
			hex2char(oldtime.DayOfMonth,str2,2);
			Disp_HZ(0x8E,str2,1);
			Disp_HZ(0x8F,day,1);
			Disp_HZ(0x98,week,2);
			Disp_HZ(0x9A,date[oldtime.DayOfWeek-1],1);
			Write_Data(0x20);
			delay_50us();
			Write_Data(0x20); 	//"  "两空格
			delay_50us();
			hex2char(oldtime.Hours,str2,2);
			Disp_HZ(0x9C,str2,1);
			Write_Data(0x3A);					//":"冒号
			delay_50us();
			hex2char(oldtime.Minutes,str2,2);
			Write_Data(str2[0]);
			delay_50us();
			Write_Data(str2[1]);
			delay_50us();
			Write_Data(0x3A);					//":"冒号
			delay_50us();
			hex2char(oldtime.Seconds,str2,2);
			Disp_HZ(0x9F,str2,1);
			if (flag & DELTA != 0)
			switch (cursor)
			{
				case 0:case 1:case 2:case 3:
				{
					hex2char(oldtime.Year,str,4);
					str[cursor]='_';
					Disp_HZ(0x88,str,2);
					break;
				}
				case 4: case 5:
				{
					hex2char(oldtime.Month,str2,2);
					str2[cursor-4]='_';
					Disp_HZ(0x8C,str2,1);
					break;
				}
				case 6: case 7:
				{
					hex2char(oldtime.DayOfMonth,str2,2);
					str2[cursor-6]='_';
					Disp_HZ(0x8E,str2,1);
					break;
				}
				case 8:
				{
					str2[0]='_';
					str2[1]='_';
					Disp_HZ(0x9A,str2,1);
					break;
				}
				case 9: case 10:
				{
					hex2char(oldtime.Hours,str2,2);
					str2[cursor-9]='_';
					Disp_HZ(0x9C,str2,1);
					break;
				}
				case 11:
				{
					str2[0]=':';
					str2[1]='_';
					Disp_HZ(0x9D,str2,1);
					break;
				}
				case 12:
				{
					str2[0]='_';
					str2[1]=':';
					Disp_HZ(0x9E,str2,1);
					break;
				}
				case 13: case 14:
				{
					hex2char(oldtime.Seconds,str2,2);
					str2[cursor-13]='_';
					Disp_HZ(0x9F,str2,1);
					break;
				}
			}
			break;
		}
	case Plot_Graph:
		{
			if (position<121)
			{
				tmp = End_Flag;
				while (position < 120)
				{
					measure();
					if (no_answer > TOLERANCE)
					{
						tmp++;
					}
					else
					{
						de = VE-2048;
						db = abs(VB1-VB2);
						measure();
						if (no_answer <= TOLERANCE)
						{
							de += VE-2048;
							db += abs(VB1-VB2);
							measure();
							if (no_answer > TOLERANCE)
							{
								de = de/2;
								db = db/2;
							}
							else
							{
								de += VE-2048;
								db += abs(VB1-VB2);
								de = de/3;
								db = db/3;
							}
						}
						tmp = End_Flag;
					}
					if (tmp == End_Flag) break;
					if (tmp == Wrong_Flag) break;
				}

				if (tmp == End_Flag)
				{
					if ((24<position)&(position<121))
					{
						for (tmp=23;tmp<36;tmp++)
						drawpoint(position,tmp,Point_Clear);
					}
					beta = (abs(de*1.0/db*330)-100);
					alpha = beta*0.1-10;
					alpha = 57-alpha;
					drawpoint(position,alpha,Point_Draw);  //position=x; alpha=y;
				}
				else
				{
					if ((24<position)&(position<121))
					{
						for (tmp=23;tmp<36;tmp++)
						drawpoint(position,tmp,Point_Clear);
					}
					Its_A_Joke++;
				}
				position++;
				veset += (mode*2-1)*4;
			}
			else if(position == 121)
			{
				position++;
				Its_A_Joke = 0;
				TLV_5613(MIDDA);
			}
			break;
		}
	case All_Reset:
		{
			Disp_HZ(LINE1,RESET_CONFIRM[0] ,8);
			Disp_HZ(LINE2,RESET_CONFIRM[1] ,8);
			Disp_HZ(LINE3,RESET_CONFIRM[2] ,8);
			Disp_HZ(LINE4,RESET_CONFIRM[3] ,8);
			break;
		}

	default : break;
	}
}
/*******************************************
函数名称：dealkey
功能：响应按键功能
参数：无; 返回值：无
********************************************/
void dealkey(void)
{
	unsigned char str[] = {"    "};
	switch (key)
	{
	case  0 : break;
	case  1 :
		{
			if ((screen == Help_User)||(screen == Set_Clock)||(screen==Plot_Graph)) break;
			if (key_long == 0)
			{
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
				change_mode();
				screen = Normal_Display;
			}
			break;
		}
		case 2 :		//确认键
		{
			if ((screen == Help_User)||(screen==Plot_Graph)) break;
			if (key_long == 0)
			{
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
				ieold[0]=ie[0];ieold[1]=ie[1];ieold[2]=ie[2];
				set_ve(CONFIRM);
				if (screen == Set_Clock)
				{
					newtime = oldtime;
					RTC_calendarInit(__MSP430_BASEADDRESS_RTC__,
						newtime,
						RTC_FORMAT_BCD);
				}
				screen = Normal_Display;
				cursor = 15;
			}
			break;
		}
	case  3 :			//向上键
		{
			if (screen==Plot_Graph) break;
			if ((key_long & BIT0)==0)
			{
				if (screen == Help_User)
				{
					position--;
					if (position == 0)
						{
							position = 1;
							return;
						}
				}
				else if (screen == Set_Clock)
				{
					switch (cursor)
					{
						case 0: case 1: case 2: case 3:
						{
							hex2char(oldtime.Year,str,4);
							str[cursor]++;
							if (str[cursor]>'9') str[cursor]='0';
							char2hex(str,&oldtime.Year,4);
							break;
						}
						case 4:
						{
							hex2char(oldtime.Month,str,2);
							if (str[0]=='1')
							{
								str[0]='0';
								if (str[1]=='0') str[1]='1';
								if (str[1]=='2')
								{
									if (oldtime.DayOfMonth > 0x29) oldtime.DayOfMonth = 0x29;
								}
								if ((str[1]=='4')||(str[1]=='6')||(str[1]=='9'))
								{
									if (oldtime.DayOfMonth > 0x30) oldtime.DayOfMonth = 0x30;
								}
							}
							else
							{
								str[0]='1';
								if (str[1]>'2') str[1]='2';
								if (str[1]=='1') 
								{
									if (oldtime.DayOfMonth > 0x30) oldtime.DayOfMonth = 0x30;
								}
							}
							char2char(str,&oldtime.Month,2);
							break;
						}
						case 5:
						{
							hex2char(oldtime.Month,str,2);
						    str[1]++;
						    if (str[0]=='0')
						    {
						    	if (str[1]>'9') str[1]='1';
						    	if (str[1]=='2')
						    	{
									if (oldtime.DayOfMonth > 29) oldtime.DayOfMonth = 29;
								}
								if ((str[1]=='4')||(str[1]=='6')||(str[1]=='9'))
								{
									if (oldtime.DayOfMonth > 0x30) oldtime.DayOfMonth = 0x30;
								}
						    }
							else
							{
								if (str[1]>'2') str[1]='0';
								if (str[1]=='1')
								{
									if (oldtime.DayOfMonth > 0x30) oldtime.DayOfMonth = 0x30;
								}
							}
							char2char(str,&oldtime.Month,2);
							break;
						}
						case 6:
						{
							hex2char(oldtime.DayOfMonth,str,2);
							str[0]++;
							if (oldtime.Month == 2)
							{
								if (str[0]=='3')
								{
									str[0]='0';
									if (str[1]=='0') str[1]='1';
								}
							}
							if (str[0]=='4')
							{
								str[0]='0';
								if (str[1]=='0') str[1]='1';
							}
							if (str[0]=='3')
							{
								if (str[1]>'1') str[1]='1';
							}
							char2char(str,&oldtime.DayOfMonth,2);
							break;
						}
						case 7:
						{
							hex2char(oldtime.DayOfMonth,str,2);
						    str[1]++;
						    if (str[0]=='3')
						    {
						    	if (str[1]=='2') str[1]='0';
						    }
						    else if (str[0]>'0')
							{
								if (str[1]>'9') str[1]='0';
							}
						    else
						    {
						    	if (str[1]>'9') str[1]='1';
						    }
							char2char(str,&oldtime.DayOfMonth,2);
							break;
						}
						case 8:
						{
							oldtime.DayOfWeek++;
							if (oldtime.DayOfWeek > 7) oldtime.DayOfWeek = 1;
							break;
						}
						case 9:
						{
							hex2char(oldtime.Hours,str,2);
							str[0]++;
							if (str[0]=='3') str[0]= '0';
							if (str[0]=='2')
							{
								if (str[1]>'3') str[1]='3';
							}
							char2char(str,&oldtime.Hours,2);
							break;
						}
						case 10:
						{
							hex2char(oldtime.Hours,str,2);
							str[1]++;
							if (str[0]=='2')
							{
								if (str[1]>'3') str[1]= '0';
							}
							else
							{
								if (str[1]>'9') str[1]='0';
							}
							char2char(str,&oldtime.Hours,2);
							break;
						}
						case 11:
						{
							hex2char(oldtime.Minutes,str,2);
							str[0]++;
							if (str[0]>'5') str[0]= '0';
							char2char(str,&oldtime.Minutes,2);
							break;
						}
						case 12:
						{
							hex2char(oldtime.Minutes,str,2);
							str[1]++;
							if (str[1]>'9') str[1]= '0';
							char2char(str,&oldtime.Minutes,2);
							break;
						}
						case 13:
						{
							hex2char(oldtime.Seconds,str,2);
							str[0]++;
							if (str[0]>'5') str[0]= '0';
							char2char(str,&oldtime.Seconds,2);
							break;
						}
						case 14:
						{
							hex2char(oldtime.Seconds,str,2);
							str[1]++;
							if (str[1]>'9') str[1]= '0';
							char2char(str,&oldtime.Seconds,2);
							break;
						}
						default : break;
					}
				}
				else if (cursor>2) return;
					 else set_ve(UP);
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
			}
			break;
		}
	case  4 :		//返回键及取消键
		{
			if (key_long == 0)
			{
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
				ie[0]=ieold[0];ie[1]=ieold[1];ie[2]=ieold[2];
				cursor = 15;
				if (screen==Plot_Graph)
				{
					Ini_Lcd();
					delay_nms(1000);
				}
				screen = Normal_Display;
			}
			break;
		}
	case  5 :			//基本测试开始键
		{
			if ((screen == Help_User)||(screen == Set_Clock)||(screen==Plot_Graph)) break;
			BEEP();
			__delay_cycles(60000);
			NO_BEEP();
			ieold[0]=ie[0];ieold[1]=ie[1];ieold[2]=ie[2];
			set_ve(CONFIRM);
			cursor = 15;
			screen = Normal_Output;
			disp(screen);
			measure();
			if (no_answer > TOLERANCE)
			{
				screen = Run_Time_Error;
				break;
			}
			de = ad[0];
			db = abs(VB1-VB2);
			measure();
			if (no_answer > TOLERANCE)
			{
				screen = Run_Time_Error;
				break;
			}
			de += ad[0];
			db += abs(VB1-VB2);
			measure();
			if (no_answer > TOLERANCE)
			{
				screen = Run_Time_Error;
				break;
			}
			de += ad[0];
			db += abs(VB1-VB2);
			de = de/3;
			db = db/3;
			clear(no_answer);
			do
			{
				no_answer++;
				TLV_5613(da-no_answer);
				ADS_7950(ad,3);
				alpha = abs((de - ad[0])*330.0/(db-abs(VB1-VB2))-((mode*2-1)*newtime.Seconds % 10));
				if (no_answer > TOLERANCE)
				{
					beta=0;
					alpha=0;
					screen = Run_Time_Error;
					flag = 0;
					return;
				}
			}while (abs(alpha-beta)>0x0040);
			veset=MIDDA;
			TLV_5613(veset);
			break;
		}
	case  6 : 			//光标循环左移键
		{
			if ((screen == Help_User)||(screen==Plot_Graph)) break;
			if ((key_long & BIT0)== 0)
			{
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
				if (cursor == 15)  cursor = 2;
				else if (cursor !=0) Cursor_Left;
						else if (screen == Set_Clock) cursor = 14;
								else cursor=2;
				if (screen == Normal_Output) screen = Normal_Display;
				if (screen == Run_Time_Error) screen = Normal_Display;
			}
			break;
		}

	case  7 :			//向下键
		{
			if (screen==Plot_Graph) break;
			if ((key_long & BIT0)==0)
			{
				if (screen == Help_User)
				{
					position++;
					if (position > 19)
						{
							position = 19;
							return;
						}
				}
				else if (screen == Set_Clock)
				{
					switch (cursor)
					{
						case 0: case 1: case 2: case 3:
						{
							hex2char(oldtime.Year,str,4);
							if (str[cursor]=='0') str[cursor]='9';
							else str[cursor]--;
							char2hex(str,&oldtime.Year,4);
							break;
						}
						case 4:
						{
							hex2char(oldtime.Month,str,2);
							if (str[0]=='1')
							{
								str[0]='0';
								if (str[1]=='0') str[1]='1';
								if (str[1]=='2')
								{
									if (oldtime.DayOfMonth > 0x29) oldtime.DayOfMonth = 0x29;
								}
								if ((str[1]=='4')||(str[1]=='6')||(str[1]=='9'))
								{
									if (oldtime.DayOfMonth > 0x30) oldtime.DayOfMonth = 0x30;
								}
							}
							else
							{
								str[0]='1';
								if (str[1]>'2') str[1]='2';
								if (str[1]=='1') 
								{
									if (oldtime.DayOfMonth > 0x30) oldtime.DayOfMonth = 0x30;
								}
							}
							char2char(str,&oldtime.Month,2);
							break;
						}
						case 5:
						{
							hex2char(oldtime.Month,str,2);
						    if (str[0]=='0')
						    {
						    	if (str[1]=='1') str[1]='9';
								else str[1]--;
								if (str[1]=='2')
								{
									if (oldtime.DayOfMonth > 0x29) oldtime.DayOfMonth = 0x29;
								}
								if ((str[1]=='4')||(str[1]=='6')||(str[1]=='9'))
								{
									if (oldtime.DayOfMonth > 0x30) oldtime.DayOfMonth = 0x30;
								}
						    }
							else
							{
								if (str[1]=='0') str[1]='2';
								else str[1]--;
								if (str[1]=='1') 
								{
									if (oldtime.DayOfMonth > 0x30) oldtime.DayOfMonth = 0x30;
								}
							}
							char2char(str,&oldtime.Month,2);
							break;
						}
						case 6:
						{
							hex2char(oldtime.DayOfMonth,str,2);
							if (str[0]=='0')
							{
								str[0]='3';
								if (oldtime.Month == 0x02) str[0]='2';
									else if (str[1]>'1') str[1]='1';
								if ((oldtime.Month == 0x04)||(oldtime.Month == 0x06)||(oldtime.Month == 0x09)||(oldtime.Month == 0x11))
								{
									str[1]='0';
								}
							}
							else str[0]--;
							char2char(str,&oldtime.DayOfMonth,2);
							break;
						}
						case 7:
						{
							hex2char(oldtime.DayOfMonth,str,2);
						    if (str[0]=='3')
						    {
						    	if (str[1]=='0') str[1]='1';
								if ((oldtime.Month == 0x04)||(oldtime.Month == 0x06)||(oldtime.Month == 0x09)||(oldtime.Month == 0x11))
								{
									str[1]='0';
								}
						    }
						    else 
							{
								if ((str[0]=='0')&&(str[1]=='1')) str[1]='9';
								else	if (str[1]=='0') str[1]='9';
										else str[1]--;
							}
							char2char(str,&oldtime.DayOfMonth,2);
							break;
						}
						case 8:
						{
							if (oldtime.DayOfWeek == 0x01) oldtime.DayOfWeek = 0x07;
							else oldtime.DayOfWeek--;
							break;
						}
						case 9:
						{
							hex2char(oldtime.Hours,str,2);
							if (str[0]=='0')
							{
								str[0]='2';
								if (str[1]>'3') str[1]='3';
							}
							else str[0]--;
							char2char(str,&oldtime.Hours,2);
							break;
						}
						case 10:
						{
							hex2char(oldtime.Hours,str,2);
							if (str[0]=='2')
							{
								if (str[1]=='0') str[1]= '3';
									else str[1]--;
							}
							else
							{								
								if (str[1]=='0') str[1]='9';
									else str[1]--;
							}
							char2char(str,&oldtime.Hours,2);
							break;
						}
						case 11:
						{
							hex2char(oldtime.Minutes,str,2);
							if (str[0]=='0') str[0]= '5';
								else str[0]--;
							char2char(str,&oldtime.Minutes,2);
							break;
						}
						case 12:
						{
							hex2char(oldtime.Minutes,str,2);
							if (str[1]=='0') str[1]= '9';
								else str[1]--;
							char2char(str,&oldtime.Minutes,2);
							break;
						}
						case 13:
						{
							hex2char(oldtime.Seconds,str,2);
							if (str[0]=='0') str[0]= '5';
								else str[0]--;
							char2char(str,&oldtime.Seconds,2);
							break;
						}
						case 14:
						{
							hex2char(oldtime.Seconds,str,2);
							if (str[1]=='0') str[1]= '9';
								else str[1]--;
							char2char(str,&oldtime.Seconds,2);
							break;
						}
						default : break;
					}
				}
				else if (cursor>2) return;
					 else set_ve(DOWN);
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
			}
			break;
		}
	case  8 :			//光标循环右移键
		{
			if ((screen == Help_User)||(screen==Plot_Graph)) break;
			if ((key_long & BIT0)== 0)
			{
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
				if (cursor == 15)  cursor = 0;
				else if (cursor !=14) Cursor_Right;
						else cursor = 0;
				if (screen == Normal_Output) screen = Normal_Display;
				if (screen == Run_Time_Error) screen = Normal_Display;
				if (screen == Normal_Display) cursor %= 3;
			}
			break;
		}
	case 9 :
		{
			if (key_long == 0 )		//短按复位键
			{
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
				ie[0]=ieold[0];ie[1]=ieold[1];ie[2]=ieold[2];
				if (screen==Plot_Graph)
				{
					Ini_Lcd();
					delay_nms(1000);
				}
				screen = All_Reset;
			}else if (key_long >5) 	//长按复位键
			{
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
				initialize();
				newtime = RTC_getCalendarTime(__MSP430_BASEADDRESS_RTC__);
				screen = mode = key_long = flag = beta = alpha = 0;
				cursor = 15;
			}
			break;
		}
	case 10 :
		{
			if (key_long == 0)
			{
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
				ie[0]=ieold[0];ie[1]=ieold[1];ie[2]=ieold[2];
				cursor = 15;
				position = 14;
				Its_A_Joke = 0;
				veset = (mode*2-1)*40+2048;
				screen = Plot_Graph;
				Ini_Lcd();
				delay_nms(1000);
				LCD_draw(0,0,graph,128,64);
			}
			break;
		}

	case 11 :		//时间设定
		{
			if (key_long == 0)
			{
				BEEP();
				__delay_cycles(60000);
				NO_BEEP();
				cursor = 0;
				oldtime = newtime;
				screen = Set_Clock;
				ie[0]=ieold[0];ie[1]=ieold[1];ie[2]=ieold[2];
				if (screen==Plot_Graph)
				{
					Ini_Lcd();
					delay_nms(1000);
				}
			}
			break;
		}
	case  12 :		//帮助界面
		{
			if (screen == Help_User) break;
			BEEP();
			__delay_cycles(60000);
			NO_BEEP();
			position = 1;
			cursor = 0;
			ie[0]=ieold[0];ie[1]=ieold[1];ie[2]=ieold[2];
			if (screen==Plot_Graph)
			{
				Ini_Lcd();
				delay_nms(1000);
			}
			screen = Help_User;
			break;
		}
	default : break;
	}
}
/*******************************************/
void main(void)
{

	initialize();
	newtime = RTC_getCalendarTime(__MSP430_BASEADDRESS_RTC__);
	screen = mode = key_long =Its_A_Joke = flag = beta = alpha = 0;
	disp(screen);
	cursor = 15;

	while(1)
	{
		Key_Event(&key,&key_long);
		dealkey();
		if(daiji<15)
		disp(screen);
		if (screen!=Plot_Graph) delay_nms(1000);
		else if (Its_A_Joke > HALF_T)
		{
			Ini_Lcd();
			delay_nms(1000);
			disp(Normal_Display);
			screen = Run_Time_Error;
			Its_A_Joke = flag = 0;
		}
	}
}

#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR (void)
{
	daiji++;
	if(daiji>15)
	{
		P3OUT&=~BIT0;
		P3OUT&=~BIT5;
		Write_Cmd(0x08);
	}

    switch (__even_in_range(RTCIV,2)){
        case 2:         //RTCRDYIFG
        	newtime = RTC_getCalendarTime(__MSP430_BASEADDRESS_RTC__);
            break;
        default: break;
    }
}
