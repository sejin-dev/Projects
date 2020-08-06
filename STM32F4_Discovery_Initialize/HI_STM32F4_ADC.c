#ifndef __HI_STM32F4_ADC_C
#define __HI_STM32F4_ADC_C

#include "stm32f4xx.h"
#include "HI_STM32F4_Delay_for_168Mhz.h"
  //ADC setting , PC1, ADC123_IN11, ADC1 이용 


void ADC_Setting(void){


  GPIOC-> MODER |= 0x0000000C;                  // PC1 = Analog mode
  RCC-> APB2ENR |= 0x00000100;                    // Enable ADC1 Clock
  
  ADC-> CCR = 0x00000000;                       //ADCCLk = PCLK/2 = 42Mhz
  ADC1->SMPR1 =0x00000008;                      // Sampling time of channel 11 = 15cycle
  ADC1-> CR1 = 0x00000000;                              //12bit resolution 
  ADC1-> CR2 = 0x00000001;                              //right alignment, single conversion,ADON=1
  ADC1-> SQR1 = 0x00000000;                              //Total regular channel number = 1
  ADC1-> SQR3 = 0x0000000B;                             //변환할 채널 기입 , 11Channel 
}
  

// Read ADC value

int Read_ADC_Value(void){
    int ADC_Val;
    ADC1-> CR2|= 0x40000000;
    while(!(ADC1->SR & 0x00000002));
    ADC_Val = ADC1->DR;
    delay_ms(1);
    return ADC_Val;
}
#endif

