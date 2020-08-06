#ifndef __HI_STM32F4_TIMER_C
#define __HI_STM32F4_TIMER_C

#include "stm32f4xx.h"

void PWM_PC8_TIM8_CH3_Setting(void){

   RCC->AHB1ENR |= 0x00000004;          //PC Clock enable(GPIOCEN = 1)
  GPIOC-> MODER &= 0xFFFCFFFF;        // set PC8 to alternative tunction mode
  GPIOC-> MODER |= 0x00020000;        
  GPIOC->AFR[1] |=     0x00000003;        // PC8 = Tim8_CH3    (Alternate Function3)
   
  RCC-> APB2ENR |= 0x00000002;          // TIM8 Clock enable 
  TIM8 -> PSC = 1679;
  TIM8 -> ARR = 999;
  TIM8 -> CCR3= 50;
  TIM8 ->  CNT = 0;                              //clear counter 
  TIM8 -> CCMR2 |=0x0060;               //PWMmode
  TIM8-> CCER |= 0x0100;                // CC3E =1 (Capture/Compare 3 output Enable )
  TIM8-> BDTR |= 0x8000;                // OC output enable 
  TIM8-> CR1  = 0x0005;                     //edge=aligned, up-counter, 
}

#endif

