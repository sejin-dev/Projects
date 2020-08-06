#ifndef __HI_STM32F4_SYSTICK_C
#define __HI_STM32F4_SYSTICK_C

#include "stm32f4xx.h"


volatile unsigned int key_value;
volatile unsigned int systick_count_1ms =0; 

void Systick_setting(void){
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock/1000);                          //systick timer 초기화 1ms로 
  }
  
  
void SysTick_Handler(void)
{  
      if(systick_count_1ms<500)  {                             //1ms 마다 실행 
        //Push button
        key_value = GPIOA ->IDR &= 0x00000001;               //Read PA0
         
        if(key_value==1){                                                  //키 눌리면, LED count 초기화
          //+= 50 ;
          GPIOD-> ODR =0x00000000; 
          
        }
        
      systick_count_1ms++;  
    }
    
    
  else{ 
    systick_count_1ms =0;
    GPIOD-> ODR +=0x00001000;

   }
}
#endif
  

