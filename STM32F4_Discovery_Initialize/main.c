/********************************************************************************
  * @file    Hi_stm32f4/main.c                          @brief   Main program body
  * @author  SEJIN LIM                                    @system clock  168Mhz 
  * @version V1.0.0                                         @IAR EWARM V7.80.2 
  * @date    19-June-2020
  ******************************************************************************    */ 

/*----STM32F4 discovery 기본 함수           */-------------------------------*/
#include "stm32f4xx.h"

/*----/*      사용자 정의 함수 선언                          */----------------------*/
#include "HI_STM32F_LED.h"
#include "HI_STM32F_Initialize_MCU.h"
#include "HI_STM32F4_Delay_for_168Mhz.h"
#include "HI_STM32F4_Systick.h"
#include "HI_STM32F4_Push_Button.h"
#include "HI_STM32F4_Timer.h"
#include "HI_STM32F4_ADC.h"
    
/*----/*      Main program                              */------------------------*/
typedef unsigned char U08;
typedef signed char S08;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned int U32;
typedef signed int S32;       

 int PWM = 50;          //(50=0도, 100= 180도
 int ADC_Value = 0;
 
int main(void){

Initialize_MCU();                                   // Clock setting 

LED_SETTING();                          // set up for LED 

Push_Button_Setting();                    //set up for Push Button , PA0 is Push Button 

Systick_setting();                                //systick timer setting

PWM_PC8_TIM8_CH3_Setting(); // Timer for PWM pulse PC8, TIM8_CH3, 

ADC_Setting();                        //ADC setting , PC1, ADC123_IN11, ADC1 이용 
 
  
  while(1){
    
    ADC_Value = Read_ADC_Value();
      
    TIM8 -> CCR3= PWM+ADC_Value/50;             // Modify PWM Pulse Control 
  }
}
/*----     END            */------------------------*/

