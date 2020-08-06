
#ifndef __HI_STM32F4_LED_C
#define __HI_STM32F4_LED_C

#include "stm32f4xx.h"
void LED_SETTING(void){
    RCC->AHB1ENR |=  0x00000008;           //port D clock enable(GPIODEN = 1)
  GPIOD-> MODER &= 0x00FFFFFF;          //set PD15~PD12 to output
  GPIOD-> MODER |= 0x55000000;
  GPIOD-> ODR &=0xFFFF0FFF;             //set output of PD15~PD12 to =0;
  GPIOD-> ODR   =0x00001000; 
}


#endif /*__HI_STM32F4_LED_H */

