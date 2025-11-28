#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AD.h"
#include "Key.h"
#include "Timer.h"
#include "math.h"
uint16_t AD0, AD1, AD2, AD3;
int AD0_filter;
float voltage_filter;
uint8_t KeyNum1,KeyNum2,last_KeyNum2=-1,KeyNum3,KeyNum4;
int last_key1,last_key2,last_key3,last_key4;
int key1_err,key2_err,key3_err,key4_err;
int count=0,stop_flag=0,count_test=0,count_tim3=0;
float voltage,resis,resis_origin,Al_voltage,Al_voltage_sum,Al_voltage_equal;
int DV_range1_flag=0,DV_range2_flag=0,DV_over_range_flag=0;
int lsat_DV_range1_flag,last_DV_range2_flag,last_DV_over_range_flag;
int R_range1_flag=0,R_range2_flag=0,R_over_range_flag=0;
int lsat_R_range1_flag,last_R_range2_flag,last_R_over_range_flag;
int AV_range1_flag=0,AV_range2_flag=0,AV_over_range_flag=0;
int lsat_AV_range1_flag,last_AV_range2_flag,last_AV_over_range_flag;
int clear_flag=0;
int DV_error_1,DV_error_2,DV_error_over;
int R_error_1,R_error_2,R_error_over;
int AV_error_1,AV_error_2,AV_error_over;
int DV_error,AV_error,R_error;
int DV_flag=0,AV_flag=0,R_flag=0;
int DV_last_flag=0,AV_last_flag=0,R_last_flag=0;
int ko_flag=0,last_ko_flag,ko_error;
int samp_count=0;
int switch_R1,switch_R2;//手动换量程，根据硬件开关状态置位1、2
int AD_filtter[9];
    int rms = 0;
int main(void)
{
	OLED_Init();
	AD_Init();
//	ADB_Init();
	Key_Init();
	Key_Init_A();
  Timer_Init();
	Timer_Init3();
		OLED_ShowString(1,1,"1.DC_voltage");    
	OLED_ShowString(2,1,"2.AC_voltage"); 
	while (1)
	{
		
//		OLED_ShowFNum(2, 1,samp_count , 5,0);
		/*开关配置，根据实际硬件更改*/
		KeyNum1 = Key_GetNum1();//PB12
		KeyNum2 = Key_GetNum2();//PB13
		KeyNum3 = Key_GetNum3();//PB14
		KeyNum4 = Key_GetNum4();//PB15
		key1_err = KeyNum1 - last_key1;
		key2_err = KeyNum2 - last_key2;
		key3_err = KeyNum3 - last_key3;
		key4_err = KeyNum4 - last_key4;
		last_key1 = KeyNum1;
		last_key2 = KeyNum2;
		last_key3 = KeyNum3;
		last_key4 = KeyNum4;
		
		
		if(KeyNum1==1&&KeyNum2==0&&KeyNum3==0&&KeyNum4==0)/*直流开关*/
		{
						OLED_Clear();

			DV_flag=1;
			R_flag=0;
			AV_flag=0;
//			GPIO_SetBits( GPIOA, GPIO_Pin_1 );
			GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5);
		}
		else if(KeyNum1==0&&KeyNum2==1&&KeyNum3==0&&KeyNum4==0)/*交流开关*/
		{
						OLED_Clear();

		  R_flag=0;
			DV_flag=0;
			AV_flag=1;
//			GPIO_SetBits( GPIOA, GPIO_Pin_1 );
			GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5);
		}
//		else if(KeyNum1==0&&KeyNum2==0&&KeyNum3==1&&KeyNum4==0)/*电阻200开关*/
//		{
//			R_flag=1;
//			DV_flag=0;
//			AV_flag=0;
//			R_range1_flag=1;
//			R_range2_flag=0;
//			GPIO_SetBits(GPIOA, GPIO_Pin_5 );
//			GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_2);
//			switch_R1=1;
//			switch_R2=0;
//		}
//		else if(KeyNum1==0&&KeyNum2==0&&KeyNum3==0&&KeyNum4==1)/*电阻200k开关*/
//		{
//			R_flag=1;
//			DV_flag=0;
//			AV_flag=0;
//			R_range1_flag=0;
//			R_range2_flag=1;
//			GPIO_SetBits(GPIOA, GPIO_Pin_1 );
//			GPIO_ResetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_4);
//			switch_R1=0;
//			switch_R2=1;
//		}
//		else/*可以改成手动待机*/
//		{
////			stop_flag=1;
//			//		  OLED_Clear();
//			R_flag=0;
//			DV_flag=0;
//			AV_flag=0;
//			R_range1_flag=0;
//			R_range2_flag=0;
//			GPIO_ResetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5);//初始都不通
//			if(stop_flag==0)
//			{
//			OLED_ShowString(3, 1, "incorrect switch");	
//			OLED_ShowString(1, 10, "tim:");
//			OLED_ShowString(1, 16, "s");	
//			OLED_ShowNum(1, 14, count, 2);		
//			Delay_ms(100);//屏幕刷新时间
//			}
//		}
		
		
		R_error_1=R_range1_flag-lsat_R_range1_flag;
		R_error_2=R_range2_flag-last_R_range2_flag;
		lsat_R_range1_flag=R_range1_flag;
		last_R_range2_flag=R_range2_flag;
		
				
		DV_error=DV_flag-DV_last_flag;
		DV_last_flag=DV_flag;
		R_error=R_flag-R_last_flag;
		R_last_flag=R_flag;
		AV_error=AV_flag-AV_last_flag;
		AV_last_flag=AV_flag;

		
		
		if(stop_flag==0)
		{

			/*电压换量程显示（自动）*/
			if(DV_flag==1)
	{
			if(DV_range1_flag==1)
			{
				OLED_ShowString(3, 5, "DC:0-1V");		
		OLED_ShowFNum(4, 5, 1000*voltage, 5,2);
		OLED_ShowString(4, 12, "mV");
			}
			if(DV_range2_flag==1)
      {		
				OLED_ShowString(3, 5, "DC:0-5V");
		OLED_ShowFNum(4, 5, voltage, 5,4);
		OLED_ShowString(4, 12, "V");
			}
		if(DV_over_range_flag==1)
			{OLED_ShowString(4, 1, "DC:OverRange");}
	}	
		
		
//		/*电阻换量程显剩示（手动）*/
//	  if(R_flag==1)
//	{
//			if(R_range1_flag==1&&R_over_range_flag==0)
//			{
//				OLED_ShowString(3, 5, "0-200ou");
//				OLED_ShowFNum(4, 5, resis, 4,1);
//				OLED_ShowString(4, 12, "ou");
//			}
//			if(R_range2_flag==1&&ko_flag==0)
//      {		
//				OLED_ShowString(3, 5, "0-200kou");
//				OLED_ShowFNum(4, 5, resis, 4,0);
//				OLED_ShowString(4, 12, "ou");			
//			}
//				if(R_range2_flag==1&&ko_flag==1)
//			{
//				OLED_ShowString(3, 5, "0-200kou");
//				OLED_ShowFNum(4, 5, resis/1000, 4,1);
//				OLED_ShowString(4, 12, "kou");
//			}
//			
//		if(R_over_range_flag==1)
//		{
//			OLED_ShowString(4, 1, "OverRange");
//		}
//	}	
//		
	
	/*交流电压换量程（自动）*/
				if(AV_flag==1)
	{
			if(AV_range1_flag==1)
			{
				OLED_ShowString(3, 5, "AC:0-0.2V");		
		OLED_ShowFNum(4, 5, 1000*Al_voltage_equal, 4,1);
		OLED_ShowString(4, 12, "mV");
			}
			if(AV_range2_flag==1)
      {		
				OLED_ShowString(3, 5, "AC:0-5V");
		OLED_ShowFNum(4, 5, Al_voltage_equal, 4,3);
		OLED_ShowString(4, 12, "V");
			}
		if(AV_over_range_flag==1)
			{OLED_ShowString(4, 1, "AC:OverRange");}
	}
	
		/*待机时间显示*/
		OLED_ShowString(1, 10, "tim:");
		OLED_ShowString(1, 16, "s");	
		OLED_ShowNum(1, 14, count, 2);	
			
		Delay_ms(100);//屏幕刷新时间
		}
		
		if(clear_flag==1)//清屏标志位
		{
			OLED_Clear();
			clear_flag=0;
		}
		
			if(key1_err!=0||key2_err!=0||key3_err!=0||key4_err!=0)//拨动任意开关待机时间清零
			{	
			  count=0;
				stop_flag=0;
				clear_flag=1;
			}
			if(count>=60)
			{
				stop_flag=1;
				count=0;
				OLED_Clear();
			}
}
}





void TIM2_IRQHandler(void)//计时器，60秒后待机
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET )
	{
		/*执行语句*/
	if(key1_err==0&&key2_err==0&&key3_err==0&&key4_err==0&&stop_flag==0)
			count++;//待机时间，单位s
	
	
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//勿删
	}
}



void TIM3_IRQHandler(void)//adc采集中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET )
	{
		/*执行语句*/
		
		
		if(stop_flag==0)
		{
			/*adc通道采集*/
//		AD0 = AD_GetValue(ADC_Channel_0);
//			AD0_filter=Read_ADC(AD0);
//		AD1 = AD_GetValue(ADC_Channel_1);
//		AD2 = AD_GetValue(ADC_Channel_2);
//		AD3 = AD_GetValue(ADC_Channel_3);
			
			int i, j, k, temp;
    int ad_valu[9][5], ad_sum[9];

    //*******************************电感值滤波处理*******************************//
    for (i = 0; i < 5; i++)
    {
        ad_valu[0][i] = AD_GetValue(ADC_Channel_0); // 直流
        ad_valu[1][i] = AD_GetValue(ADC_Channel_3); // 交流
        ad_valu[2][i] = AD_GetValue(ADC_Channel_6); // 电阻
    }

    /*=========================冒泡排序升序==========================*/ // 舍弃最大值和最小值
    for (i = 0; i < 9; i++)
    {
        for (j = 0; j < 5; j++)
        {
            for (k = 0; k < 5 - j; k++)
            {
                if (ad_valu[i][k] > ad_valu[i][k + 1]) // 前面的比后面的大  则进行交换
                {
                    temp = ad_valu[i][k + 1];
                    ad_valu[i][k + 1] = ad_valu[i][k];
                    ad_valu[i][k] = temp;
                }
            }
        }
    }

    /*===========================中值滤波=================================*/
    for (i = 0; i < 9; i++) // 求中间三项的和
    {
        ad_sum[i] = ad_valu[i][1] + ad_valu[i][2] + ad_valu[i][3];
        AD_filtter[i] = ad_sum[i] / 3;
    }
			
			
			
			
			
			/*模数转换，根据实际硬件电路改变公式即可*/

//		voltage = 3.3*AD_filtter[0]/4096;

//			
//			if(switch_R1==1&&switch_R2==0)//200
//		    resis = AD_filtter[0];
//			if(switch_R1==0&&switch_R2==1)//200k
//				resis = 2*AD_filtter[0];
//			
//		Al_voltage = 3.3*AD_filtter[0]/4096;//单个采样点数据

					voltage = 3.3*AD_filtter[0]/4096;
		Al_voltage = 3.3*AD_filtter[0]/4096;//??????????????
		 
//			if(switch_R1==1&&switch_R2==0)//200
//			{
//				resis_origin = 3.3*AD_filtter[1]/4096;
//		    resis = 220*resis_origin/(3.11-resis_origin);
//			}
//			if(switch_R1==0&&switch_R2==1)//200k
//			{
//				resis_origin = 3.3*AD_filtter[2]/4096;
//				resis = 100*resis_origin/(3.3-resis_origin) *1000;		
//			}
//			
			
			

			
//			if(Al_voltage!=0)//是采样点个数，可自行调整
//			{
//				samp_count++;
//				Al_voltage_sum += Al_voltage*Al_voltage;	
//			}
//			else if(Al_voltage==0)
//			{	
//      Al_voltage_equal = sqrt(Al_voltage_sum/samp_count);
//			Al_voltage_sum=0;
//			samp_count=0;
//			}


			
			
			
			
			
			
			
			samp_count++;
			if(samp_count<=3600)//是采样点个数，可自行调整
			{
				Al_voltage_sum += Al_voltage*Al_voltage;	
			}
			if(samp_count>3600)
			{	
      Al_voltage_equal = sqrt(Al_voltage_sum/1800);
			Al_voltage_sum=0;
			samp_count=0;
			}
		}
		
		
		/*直流电压换量程*/
		if(DV_flag==1)
	{
    lsat_DV_range1_flag=DV_range1_flag;
		last_DV_range2_flag=DV_range2_flag;
		last_DV_over_range_flag=DV_over_range_flag;
		
		if(voltage<1)
		{
			DV_range1_flag=1;
			DV_range2_flag=0;
			DV_over_range_flag=0;
		}
		else if(voltage<5&&voltage>=1)
		{
			DV_range1_flag=0;
			DV_range2_flag=1;
			
			DV_over_range_flag=0;
		}	
		else if(voltage>=5)
		{
			DV_range1_flag=0;
			DV_range2_flag=0;
			DV_over_range_flag=1;
		}
		
		DV_error_1=DV_range1_flag-lsat_DV_range1_flag;
		DV_error_2=DV_range2_flag-last_DV_range2_flag;
		DV_error_over=DV_over_range_flag-last_DV_over_range_flag;
		
	if(DV_error_1!=0||DV_error_2!=0||DV_error_over!=0||DV_error!=0||R_error!=0||AV_error!=0)
		clear_flag=1;
 }	
	
	
		

/*电阻换量程*/
//		if(R_flag==1)
//{

//		last_R_over_range_flag=R_over_range_flag;
//	if(R_range1_flag==1&&R_range2_flag==0)	
//	{
//		if(resis<200)
//		{
//			R_over_range_flag=0;
//			ko_flag=0;
//		}
//		else if(resis>=200)
//		{
//			R_over_range_flag=1;
//			ko_flag=0;
//		}	
//	}
//	
//if(R_range1_flag==0&&R_range2_flag==1)	
//	{	
//		if(resis<2000)
//		{
//			R_over_range_flag=0;
//			ko_flag=0;
//		}	
//		else if(resis<200000&&resis>=2000)
//		{
//			R_over_range_flag=0;
//			ko_flag=1;
//		}	
//		else if(resis>=200000)
//		{
//			R_over_range_flag=1;
//			ko_flag=0;
//		}
//  }

//		R_error_over=R_over_range_flag-last_R_over_range_flag;
//		ko_error=ko_flag-last_ko_flag;
//		last_ko_flag=ko_flag;
//		
//	if(R_error_1!=0||R_error_2!=0||R_error_over!=0||DV_error!=0||R_error!=0||ko_error!=0||AV_error!=0)
//		clear_flag=1;
//}	
//	
	
	
    /*交流电压换量程*/
		if(AV_flag==1)
	{
    lsat_AV_range1_flag=AV_range1_flag;
		last_AV_range2_flag=AV_range2_flag;
		last_AV_over_range_flag=AV_over_range_flag;
		
		if(Al_voltage_equal<0.2)
		{
			AV_range1_flag=1;
			AV_range2_flag=0;
			AV_over_range_flag=0;
		}
		else if(Al_voltage_equal<2&&Al_voltage_equal>=0.2)
		{
			AV_range1_flag=0;
			AV_range2_flag=1;
			AV_over_range_flag=0;
		}	
		else if(Al_voltage_equal>=2)
		{
			AV_range1_flag=0;
			AV_range2_flag=0;
			AV_over_range_flag=1;
		}
		
		AV_error_1=AV_range1_flag-lsat_AV_range1_flag;
		AV_error_2=AV_range2_flag-last_AV_range2_flag;
		AV_error_over=AV_over_range_flag-last_AV_over_range_flag;
		
	if(AV_error_1!=0||AV_error_2!=0||AV_error_over!=0||DV_error!=0||R_error!=0||AV_error!=0)
		clear_flag=1;
 }	 
	
	
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//勿删
	}
}