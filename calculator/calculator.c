#include<reg52.h>
#include<stdio.h>
#include<string.h>

#define max 16
#define uchar unsigned char
sbit dula = P2^6;							//段选
sbit wela = P2^7;							//位选
sbit Icden = P3^4;							//液晶使能端
sbit Icdrs = P3^5;							//液晶命令数据选择端
sbit key3=P3^6;								//独立按键P3^6
sbit key4=P3^7;								//独立按键P3^7

char express[max] = { '\0' };               //表达式
int len=0;									//表达式长度
int flag=0;									//复用按键标志

const char table[]="0123456789(.+-*/)i";	//液晶显示的所有符号，')'为'('的复用符号 ，'i'为'.'的复用符号

void delayms(int xms)						//时延函数
{
	int i, j;
	for (i = xms;i > 0;i--)
		for (j = 110;j > 0;j--);
}											
void write_com(uchar com)					//液晶指令函数
{
	Icdrs = 0;							    //置为写指令模式
	P0 = com;								//写入指令
	delayms(5);								//使能端产生高脉冲
	Icden = 1;
	delayms(5);
	Icden = 0;
}
void write_data(uchar datas)				//液晶显示数据函数
{
	Icdrs = 1;								//置为写数据模式
	P0 = datas;								//写入数据
	delayms(5);								//使能端产生高脉冲
	Icden = 1;
	delayms(5);
	Icden = 0;								
}											//液晶工作初始化函数
void init()
{
	dula = 0;								//关闭段选
	wela = 0;								//关闭位选
	Icden = 0;								//使能端初始化为低电平
	write_com(0x38);						//16*2显示，5*7点阵，8位数据接口
	write_com(0x0c);						//显示器开，光标关闭
	write_com(0x06);						//文字不移动，指针自动+1
	write_com(0x01);						//清屏
}
void calculate(float a,float b,float c,float d,char o ,float result[2])		//运算函数									  
{	//第一个操作数实部为a，虚部为b；第二个操作数实部为c，虚部为d																											  
	switch (o)//操作符o																									  
	{
	case '+':result[0] = a + c, result[1] = b + d;break;														  
	case '-':result[0] = a - c, result[1] = b - d;break;														  
	case '*':result[0] = a * c - b * d, result[1] = b * c + a * d;break;										  
	case '/':result[0] = (a * c + b * d) / (c * c + d * d), result[1] = (b * c - a * d) / (c * c + d * d);break;  
	}
}
void deal()							//表达式处理函数																			
{
    int i=0,n=len,df=1,nums=-1,o=-1;//i计数，n表达式的长度，df是小数标记，nums是数栈的栈顶位置，o是符号栈的栈顶位置
	float num=0,middleruselt[2]={0};//num记录读取到的数，middleruselt记录calculate函数的结果
	float renums[max/2]={0},imnums[max/2]={0};//数栈，renums为实部，imnums为虚部
    char opr[max/2]={'\0'};//符号栈
	for(i=0;i<n;i++)
	{
	    switch(express[i])
		{
		case '(': //左括号无条件入符号栈
		{
		    o++;
		    opr[o]='(';
		}break;
		case ')': //出现右括号，弹出符合栈的元素及数栈的元素进行运算，直到符合栈弹出左括号为止
		{
		    while(o!=-1&&opr[o]!='(')
			{
			    calculate(renums[nums-1],imnums[nums-1],renums[nums],imnums[nums],opr[o],middleruselt);
				renums[nums-1]=middleruselt[0],imnums[nums-1]=middleruselt[1];
				nums--,o--;
			}
			o--;
		}break;
		case '+'://加号入栈前应弹出符合栈的元素进行运算，直到符合栈栈顶元素不再是'*'或'/'
		{
		    while(o!=-1&&(opr[o]=='*'||opr[o]=='/'))
			{
			    calculate(renums[nums-1],imnums[nums-1],renums[nums],imnums[nums],opr[o],middleruselt);
				renums[nums-1]=middleruselt[0],imnums[nums-1]=middleruselt[1];
				nums--,o--;
			}
			o++;
			opr[o]='+';
		}break;
		case '-'://减号入栈除了与加号相同的处理外，由于运算表达式的过程为从右往左，应保证其入栈时栈顶元素不是'-'
		{
		    while(o!=-1&&(opr[o]=='*'||opr[o]=='/'))
			{
			    calculate(renums[nums-1],imnums[nums-1],renums[nums],imnums[nums],opr[o],middleruselt);
				renums[nums-1]=middleruselt[0],imnums[nums-1]=middleruselt[1];
				nums--,o--;
			}
			if (o!=-1&&opr[o] == '-')
			{
			    calculate(renums[nums-1],imnums[nums-1],renums[nums],imnums[nums],opr[o],middleruselt);
				renums[nums-1]=middleruselt[0],imnums[nums-1]=middleruselt[1];
				nums--,o--;
			}
			o++;
			opr[o]='-';
		}break;
		case '*'://乘号无条件入栈
		{
		    o++;
		    opr[o]='*';
		}break;
		case '/'://与减号原因相同，为了不改变运算顺序，需确保栈顶元素不是'/'
		{
		    if (o!=-1&&opr[o]=='/')
			{
			    calculate(renums[nums-1],imnums[nums-1],renums[nums],imnums[nums],opr[o],middleruselt);
				renums[nums-1]=middleruselt[0],imnums[nums-1]=middleruselt[1];
				nums--,o--;
			}
			o++;
			opr[o]='/';
		}break;
		default://处理数、小数点和虚数单位i
		{
		     for(;i<n;i++)
			 {
			     if(express[i]>='0'&&express[i]<='9')
				 {
				     if(df==1)//正在处理整数部分
					     num=num*10+(express[i]-'0');
					 else//正在处理小数部分
					 {
					     num=num+(express[i]-'0')/df;
						 df=df*10;
					 }
				 }
				 else
				 {
				     if(express[i]=='.')//标记小数开始出现
					     df=10;
					 else				//出现操作符或'i'，数记录结束
					     break;
				 }
			 }
			 if(i==n||express[i]!='i')//如果导致数记录结束的操作符不是'i'或表达式读取结束
			 {
			     nums++;
			     renums[nums]=num;//读取的数是实数
				 imnums[nums]=0;
				 i--;			  //导致数记录结束的操作符要被下一次循环读取到
			 }
			 else
			 {
			     nums++;
			     renums[nums]=0;
				 imnums[nums]=num;//读取的数是虚数，导致数记录结束的'i'不被下一次循环读取到
			 }
			 df=1;				  //小数标记复位
			 num=0;				  //数记录变量复位
		}break;
		}
	}
	while(o!=-1)//若表达式读取结束后，符合栈不为空，则将其中的元素全部弹出进行运算
	{
	    calculate(renums[nums-1],imnums[nums-1],renums[nums],imnums[nums],opr[o],middleruselt);
		renums[nums-1]=middleruselt[0],imnums[nums-1]=middleruselt[1];
		nums--,o--;
	}
	sprintf(opr,"%0.2f",renums[0]);//读取表达式运算结果的实部
	strcpy(express, opr);
	if(imnums[0]>=0)
	    strcat(express, "+" );
	sprintf(opr,"%0.2f",imnums[0]);//读取表达式运算结果的虚部
	strcat(express, opr);
	strcat(express, "i");
	write_com(0x80 + 0x40);
	for (i = 0;i < strlen(express);i++)//将运算结果在液晶上第二行显示出来
	{
		write_data(express[i]);
	    delayms(5);
	}
}
void keyscan1()							  //矩阵键盘按键检测函数
{
	uchar temp;
	int row,col;//按键所在的行列
	P3 = 0xf0;
	temp = P3;
	P0=0xfe;   //在液晶上对应的显示是空格，即使P3^4出现高脉冲也不会使显示出现变化
	if (temp != 0xf0)
	{
		delayms(10);
		temp = P3;
		if (temp != 0xf0)
		{
			temp = P3;
			switch (temp)				 //按键列检测
			{
			case 0xe0:col=0;break;		 //P3^4由1变为0，产生高脉冲，写入数据P0，显示无变化
			case 0xd0:col=1;break;		 //P3^5变为0，液晶为写指令模式，但无高脉冲，不影响
			case 0xb0:col=2;break;
			case 0x70:col=3;break;
			}
			P3 = 0x0f;
	        temp = P3;
			switch (temp)				//按键行检测
			{
			case 0x0e:row=0;break;
			case 0x0d:row=1;break;
			case 0x0b:row=2;break;
			case 0x07:row=3;break;
			}						   
			while (temp != 0x0f)	   //按键松手检测
			{
				temp = P3;
			}
			express[len]=table[row*4+col+6*flag];//由按键的行、列值确定符号，flag为复用键启用标志
			flag=0;								 //不论是否按下复用键，复用标志都复位
			write_com(0x80+len);				 //写入位置由表达式的实时长度确定，覆盖掉可能存在的空格显示
			write_data(express[len]);			 //将新符号在液晶上第一行显示
			len++;								 //表达式长度+1
		}
	}
	P3=0xff;									 //P3复位
}
void keyscan2()									//独立键盘按键检测函数
{												
    P0=0xfe;									//同矩阵键盘函数的处理
	if (key3 == 0)								//复用键启用按钮
	{
		delayms(10);
		if (key3 == 0)
		{
			flag=1;								//复用标志标记为1
			while (!key3);						//按键松手检测
		}
	}
	if (key4 == 0)
	{
		delayms(10);
		if (key4 == 0)
		{
		    if(flag)						   //复用建启用时，对计算器进行复位
			{
			    init();						   //液晶初始化
			    write_com(0x80);			   //写入指针初始化
				express[0]='\0';			   //表达式清空
			    len=0;						   //表达式长度为0
				flag=0;						   //复用标志复位
			}
			else
			    deal();						  //复用键不启用时，计算表达式的结果
			while (!key4);					  //按键松手检测
		}
	}
}

void main()									  //主函数
{
	init();									  //液晶初始化
	write_com(0x80);						  //写入指针初始化
	while (1)
	{
		keyscan1();							  //检测独立按键
		delayms(10);
		keyscan2();							  //检测矩阵按键
	}
}