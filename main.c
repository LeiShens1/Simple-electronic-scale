/**********************************************************************
����: ����С�����  QQ��562409206   

��������ר��Ϊ�Ա���̩΢���̵�ѹ�������� ���ԵĲ��Դ��룡

ת����ע��������

***********************************************************************/

#include "main.h"
#include "HX711.h"
#include "LCD1602.h"
#include "matrixkey.h"

unsigned long HX711_Buffer = 0;
unsigned long Weight_Maopi = 0;
long Weight_Shiwu = 0;
int cus_maopi = 0;
float cali = 0.63;
long dis;
unsigned int price = 1;
unsigned char num;
unsigned int Count;
unsigned int total_price = 0;
unsigned char flag = 0;
bit Flag_ERROR = 0;
sbit speak= P1^7;

//У׼����
//��Ϊ��ͬ�Ĵ������������߲��Ǻ�һ�£���ˣ�ÿһ����������Ҫ�������������������ʹ����ֵ��׼ȷ��
//�����ֲ��Գ���������ƫ��ʱ�����Ӹ���ֵ��
//������Գ���������ƫСʱ����С����ֵ��
//��ֵ����ΪС��
//#define GapValue 100


//****************************************************
//������
//****************************************************
void main()
{
  P1=0xf0;  
	/*����ΪʲôҪ��P1.0 1.1 1.2 1.3��0,��Ϊ����������Һ�����������������������,
	NPN�������Ǹߵ�ƽ��ͨ��,��Ϊ��Ƭ����IO��Ĭ�Ͽ�������1,Ϊ�˸�����ܶϿ�,
	������Ҫ������������ֶ���0. */
	Init_LCD1602();
	LCD1602_write_com(0x80);
	LCD1602_write_word("Welcome to use!");
	Delay_ms(1000);		 //��ʱ,�ȴ��������ȶ�

	Get_Maopi();				//��ëƤ����
	
	while(1)
	{
		EA = 0;
		Get_Weight();			//����
		EA = 1;

		//��ʾ��ǰ����
		if( Flag_ERROR == 1){
    		LCD1602_write_com(0x80+0x40);
			LCD1602_write_word("ERROR ");
			speak=0;
		}		
		else
		{         
			speak=1;
			LCD1602_write_com(0x80+0x40);
			if( Weight_Shiwu < 20)
				cali = 0.833;
			else if(Weight_Shiwu>20 && Weight_Shiwu<37)
				cali = 0.6896;
			else if(Weight_Shiwu>37&& Weight_Shiwu<53)
				cali = 0.6667;
			else if(Weight_Shiwu>53 && Weight_Shiwu<100)
				cali = 0.645;
			else if(Weight_Shiwu>100 && Weight_Shiwu<134)
				cali = 0.6352;
			else if(Weight_Shiwu>134 && Weight_Shiwu<150)
				cali = 0.6338;
			else if(Weight_Shiwu>150 && Weight_Shiwu<215)
				cali = 0.6270;
			else if(Weight_Shiwu>215 && Weight_Shiwu<441)
				cali = 0.625;
			else if(Weight_Shiwu>441)
				cali = 0.6216;

			Scan_Key();

			dis = price*(Weight_Shiwu*cali - cus_maopi) + total_price;
			if(dis < 0)
				dis = 0;
			else if(dis%10 <= 2)
				dis -= dis%10;
			else if(dis%10 >= 8)
				dis += (10-dis%10);
			LCD1602_write_data((dis)/1000 + 0X30);
      		LCD1602_write_data((dis)%1000/100 + 0X30);
     		LCD1602_write_data((dis)%100/10 + 0X30);
      		LCD1602_write_data((dis)%10 + 0X30);
			LCD1602_write_word(" g");
		}
		//���۹���
		num=matrixkey();
		if(num==16){
			Delay_ms(100);
			Init_LCD1602();
			LCD1602_write_com(0x80);
			LCD1602_write_word("ENTER THE PRICE");
			num = 0;
			price = 0;
			while(1)
			{
				LCD1602_write_com(0x80+0x40);
				num=matrixkey();
				if(num)
					if(num<=10)						//���s1-s10���£�����۸�
					{
						if(Count<4)					//�������С��4
						{
							price*=10;					//�۸�����һλ
							price+=num%10;				//���۲�����4λ
							Count++;					//λ����һ
						}
						LCD1602_write_data(price/1000 + 0X30);		//������ʾ
      					LCD1602_write_data(price%1000/100 + 0X30);
     					LCD1602_write_data(price%100/10 + 0X30);
      					LCD1602_write_data(price%10 + 0X30);
						LCD1602_write_word(" $");	
					}	
					if(num==11)						//s11���£�ȡ��
					{
						price=0;					//�۸�����
						Count=0;					//�ƴ�����
						LCD1602_write_data(0 + 0X30);		//������ʾ
      					LCD1602_write_data(0 + 0X30);
     					LCD1602_write_data(0 + 0X30);
      					LCD1602_write_data(0 + 0X30);
						LCD1602_write_word(" $");
					}
					if(num == 12) 					// s12���£�ȷ�ϵ��۲�����
					{  
						Init_LCD1602();
						LCD1602_write_com(0x80);
						LCD1602_write_word("Welcome to use!");
						break;
					}
					if(num == 14)
					{				
						total_price = price*(Weight_Shiwu*cali - cus_maopi);
						price = 0;
						Init_LCD1602();
						LCD1602_write_com(0x80);
						LCD1602_write_word("ENTER THE PRICE");
					}
			}
		}
		else if (num == 15){
			price = 1;
			total_price = 0;
		}	
	}
}

//ɨ�谴��ʵ��ȥƤ����
void Scan_Key()
{
	if(KEY1 == 0)
	{
		Delay_ms(5);
		if(KEY1 == 0){
			Delay_ms(50);
			cus_maopi = Weight_Shiwu*cali;		//ȥƤ
			Delay_ms(500);
		}	
	}
}

//****************************************************
//����
//****************************************************
void Get_Weight()
{
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//��ȡ����
	if(Weight_Shiwu > 0)			
	{	
		Weight_Shiwu = (unsigned int)((float)Weight_Shiwu/100); 	//����ʵ���ʵ������
																		
																		
		/*if(Weight_Shiwu > 5000)		//���ر���
		{
			Flag_ERROR = 1;	
		}
		else
		{
			Flag_ERROR = 0;
		}*/
	}
	else
	{
		Weight_Shiwu = 0;
	//	Flag_ERROR = 1;				//���ر���
	}
	
}

//****************************************************
//��ȡëƤ����
//****************************************************
void Get_Maopi()
{
	Weight_Maopi = HX711_Read();	
} 

//****************************************************
//MS��ʱ����(12M�����²���)
//****************************************************
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}