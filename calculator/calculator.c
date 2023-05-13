#include<reg52.h>
#include<stdio.h>
#include<string.h>

#define max 16
#define uchar unsigned char
sbit dula = P2^6;							//��ѡ
sbit wela = P2^7;							//λѡ
sbit Icden = P3^4;							//Һ��ʹ�ܶ�
sbit Icdrs = P3^5;							//Һ����������ѡ���
sbit key3=P3^6;								//��������P3^6
sbit key4=P3^7;								//��������P3^7

char express[max] = { '\0' };               //���ʽ
int len=0;									//���ʽ����
int flag=0;									//���ð�����־

const char table[]="0123456789(.+-*/)i";	//Һ����ʾ�����з��ţ�')'Ϊ'('�ĸ��÷��� ��'i'Ϊ'.'�ĸ��÷���

void delayms(int xms)						//ʱ�Ӻ���
{
	int i, j;
	for (i = xms;i > 0;i--)
		for (j = 110;j > 0;j--);
}											
void write_com(uchar com)					//Һ��ָ���
{
	Icdrs = 0;							    //��Ϊдָ��ģʽ
	P0 = com;								//д��ָ��
	delayms(5);								//ʹ�ܶ˲���������
	Icden = 1;
	delayms(5);
	Icden = 0;
}
void write_data(uchar datas)				//Һ����ʾ���ݺ���
{
	Icdrs = 1;								//��Ϊд����ģʽ
	P0 = datas;								//д������
	delayms(5);								//ʹ�ܶ˲���������
	Icden = 1;
	delayms(5);
	Icden = 0;								
}											//Һ��������ʼ������
void init()
{
	dula = 0;								//�رն�ѡ
	wela = 0;								//�ر�λѡ
	Icden = 0;								//ʹ�ܶ˳�ʼ��Ϊ�͵�ƽ
	write_com(0x38);						//16*2��ʾ��5*7����8λ���ݽӿ�
	write_com(0x0c);						//��ʾ���������ر�
	write_com(0x06);						//���ֲ��ƶ���ָ���Զ�+1
	write_com(0x01);						//����
}
void calculate(float a,float b,float c,float d,char o ,float result[2])		//���㺯��									  
{	//��һ��������ʵ��Ϊa���鲿Ϊb���ڶ���������ʵ��Ϊc���鲿Ϊd																											  
	switch (o)//������o																									  
	{
	case '+':result[0] = a + c, result[1] = b + d;break;														  
	case '-':result[0] = a - c, result[1] = b - d;break;														  
	case '*':result[0] = a * c - b * d, result[1] = b * c + a * d;break;										  
	case '/':result[0] = (a * c + b * d) / (c * c + d * d), result[1] = (b * c - a * d) / (c * c + d * d);break;  
	}
}
void deal()							//���ʽ������																			
{
    int i=0,n=len,df=1,nums=-1,o=-1;//i������n���ʽ�ĳ��ȣ�df��С����ǣ�nums����ջ��ջ��λ�ã�o�Ƿ���ջ��ջ��λ��
	float num=0,middleruselt[2]={0};//num��¼��ȡ��������middleruselt��¼calculate�����Ľ��
	float renums[max/2]={0},imnums[max/2]={0};//��ջ��renumsΪʵ����imnumsΪ�鲿
    char opr[max/2]={'\0'};//����ջ
	for(i=0;i<n;i++)
	{
	    switch(express[i])
		{
		case '(': //�����������������ջ
		{
		    o++;
		    opr[o]='(';
		}break;
		case ')': //���������ţ���������ջ��Ԫ�ؼ���ջ��Ԫ�ؽ������㣬ֱ������ջ����������Ϊֹ
		{
		    while(o!=-1&&opr[o]!='(')
			{
			    calculate(renums[nums-1],imnums[nums-1],renums[nums],imnums[nums],opr[o],middleruselt);
				renums[nums-1]=middleruselt[0],imnums[nums-1]=middleruselt[1];
				nums--,o--;
			}
			o--;
		}break;
		case '+'://�Ӻ���ջǰӦ��������ջ��Ԫ�ؽ������㣬ֱ������ջջ��Ԫ�ز�����'*'��'/'
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
		case '-'://������ջ������Ӻ���ͬ�Ĵ����⣬����������ʽ�Ĺ���Ϊ��������Ӧ��֤����ջʱջ��Ԫ�ز���'-'
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
		case '*'://�˺���������ջ
		{
		    o++;
		    opr[o]='*';
		}break;
		case '/'://�����ԭ����ͬ��Ϊ�˲��ı�����˳����ȷ��ջ��Ԫ�ز���'/'
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
		default://��������С�����������λi
		{
		     for(;i<n;i++)
			 {
			     if(express[i]>='0'&&express[i]<='9')
				 {
				     if(df==1)//���ڴ�����������
					     num=num*10+(express[i]-'0');
					 else//���ڴ���С������
					 {
					     num=num+(express[i]-'0')/df;
						 df=df*10;
					 }
				 }
				 else
				 {
				     if(express[i]=='.')//���С����ʼ����
					     df=10;
					 else				//���ֲ�������'i'������¼����
					     break;
				 }
			 }
			 if(i==n||express[i]!='i')//�����������¼�����Ĳ���������'i'����ʽ��ȡ����
			 {
			     nums++;
			     renums[nums]=num;//��ȡ������ʵ��
				 imnums[nums]=0;
				 i--;			  //��������¼�����Ĳ�����Ҫ����һ��ѭ����ȡ��
			 }
			 else
			 {
			     nums++;
			     renums[nums]=0;
				 imnums[nums]=num;//��ȡ��������������������¼������'i'������һ��ѭ����ȡ��
			 }
			 df=1;				  //С����Ǹ�λ
			 num=0;				  //����¼������λ
		}break;
		}
	}
	while(o!=-1)//�����ʽ��ȡ�����󣬷���ջ��Ϊ�գ������е�Ԫ��ȫ��������������
	{
	    calculate(renums[nums-1],imnums[nums-1],renums[nums],imnums[nums],opr[o],middleruselt);
		renums[nums-1]=middleruselt[0],imnums[nums-1]=middleruselt[1];
		nums--,o--;
	}
	sprintf(opr,"%0.2f",renums[0]);//��ȡ���ʽ��������ʵ��
	strcpy(express, opr);
	if(imnums[0]>=0)
	    strcat(express, "+" );
	sprintf(opr,"%0.2f",imnums[0]);//��ȡ���ʽ���������鲿
	strcat(express, opr);
	strcat(express, "i");
	write_com(0x80 + 0x40);
	for (i = 0;i < strlen(express);i++)//����������Һ���ϵڶ�����ʾ����
	{
		write_data(express[i]);
	    delayms(5);
	}
}
void keyscan1()							  //������̰�����⺯��
{
	uchar temp;
	int row,col;//�������ڵ�����
	P3 = 0xf0;
	temp = P3;
	P0=0xfe;   //��Һ���϶�Ӧ����ʾ�ǿո񣬼�ʹP3^4���ָ�����Ҳ����ʹ��ʾ���ֱ仯
	if (temp != 0xf0)
	{
		delayms(10);
		temp = P3;
		if (temp != 0xf0)
		{
			temp = P3;
			switch (temp)				 //�����м��
			{
			case 0xe0:col=0;break;		 //P3^4��1��Ϊ0�����������壬д������P0����ʾ�ޱ仯
			case 0xd0:col=1;break;		 //P3^5��Ϊ0��Һ��Ϊдָ��ģʽ�����޸����壬��Ӱ��
			case 0xb0:col=2;break;
			case 0x70:col=3;break;
			}
			P3 = 0x0f;
	        temp = P3;
			switch (temp)				//�����м��
			{
			case 0x0e:row=0;break;
			case 0x0d:row=1;break;
			case 0x0b:row=2;break;
			case 0x07:row=3;break;
			}						   
			while (temp != 0x0f)	   //�������ּ��
			{
				temp = P3;
			}
			express[len]=table[row*4+col+6*flag];//�ɰ������С���ֵȷ�����ţ�flagΪ���ü����ñ�־
			flag=0;								 //�����Ƿ��¸��ü������ñ�־����λ
			write_com(0x80+len);				 //д��λ���ɱ��ʽ��ʵʱ����ȷ�������ǵ����ܴ��ڵĿո���ʾ
			write_data(express[len]);			 //���·�����Һ���ϵ�һ����ʾ
			len++;								 //���ʽ����+1
		}
	}
	P3=0xff;									 //P3��λ
}
void keyscan2()									//�������̰�����⺯��
{												
    P0=0xfe;									//ͬ������̺����Ĵ���
	if (key3 == 0)								//���ü����ð�ť
	{
		delayms(10);
		if (key3 == 0)
		{
			flag=1;								//���ñ�־���Ϊ1
			while (!key3);						//�������ּ��
		}
	}
	if (key4 == 0)
	{
		delayms(10);
		if (key4 == 0)
		{
		    if(flag)						   //���ý�����ʱ���Լ��������и�λ
			{
			    init();						   //Һ����ʼ��
			    write_com(0x80);			   //д��ָ���ʼ��
				express[0]='\0';			   //���ʽ���
			    len=0;						   //���ʽ����Ϊ0
				flag=0;						   //���ñ�־��λ
			}
			else
			    deal();						  //���ü�������ʱ��������ʽ�Ľ��
			while (!key4);					  //�������ּ��
		}
	}
}

void main()									  //������
{
	init();									  //Һ����ʼ��
	write_com(0x80);						  //д��ָ���ʼ��
	while (1)
	{
		keyscan1();							  //����������
		delayms(10);
		keyscan2();							  //�����󰴼�
	}
}