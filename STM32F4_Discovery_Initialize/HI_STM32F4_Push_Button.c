#ifndef __HI_STM32F4_PUSH_BUTTON_C
#define __HI_STM32F4_PUSH_BUTTON_C

#include "stm32f4xx.h"

//set up for Push Button , PA0 is User Push Button 

void Push_Button_Setting(void){
  RCC->AHB1ENR |= 0x00000001;           //PA Clock Enable
  GPIOA ->MODER &= 0xFFFFFFC;          //PA0 set to input
}
#endif

