#include "time2_delay.h"

volatile uint8_t f_time_2_end;

volatile u32 time; // ms 计时变量



/* TIM2中断优先级配置 */
void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*中断周期为1ms*/
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=1000;	//自动重装载寄存器周期的值(计数值) 
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);	//时钟预分频数 72M/72      
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	// 清除溢出中断标志 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);	// 开启时钟    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);	//先关闭等待使用  
}


void TIM2_init(void)
{

   TIM2_NVIC_Configuration();

   TIM2_Configuration();

    START_TIME;	
}


void TIM2_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET ) 
	{	
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);    
  		 time++;
	}		 	
}

void delay_us(uint16_t times)
{
   	SysTick->LOAD = 72*times;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
	while(!(SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk));

	SysTick->CTRL = ~SysTick_CTRL_ENABLE_Msk;
}

void delay_ms(uint16_t times)
{
   for(;times>0;times--)
     delay_us(1000);
}

