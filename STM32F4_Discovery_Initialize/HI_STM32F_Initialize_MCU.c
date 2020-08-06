#ifndef __HI_STM32F4_CLOCK_SETTING_C
#define __HI_STM32F4_CLOCK_SETTING_C

#include "stm32f4xx.h"


void  Initialize_MCU(void )
{
// (1) enable Data cache, Instruction cache and Prefetch    -> FLASH->ACR 레지스터로 변경됨. 
// (2)setup wait cycle   HCLK = 168Mhz, wait cycle = 6   *HCLK의 속도에 따라서 웨이트 사이클 변경해야함.
  GPIOD-> ODR =0x00002000;
  FLASH->ACR |= 0x00000706;              // set wait cycle to 6 for 168Mhz

  // (3) set up HSE and PLL (SYSCLK = 168MHz)
  GPIOD-> ODR =0x00003000;
  RCC -> CR |= 0x00010001;                 // HSE on, HSI on
   GPIOD-> ODR =0x00004000;
  while((RCC-> CR & 0x00000002) ==0)   //wait until HSIRDY = 1
     GPIOD-> ODR =0x00005000;
  RCC->CFGR = 0x00000000;                // SYSCLK = HSI(16Mhz)
   GPIOD-> ODR =0x00006000;
  while((RCC-> CFGR & 0x0000000C)!=0) //wait untill SWS(system clock status is ready )
     GPIOD-> ODR =0x00007000;
  RCC->CR =0x00010001;                                               //PLL off, HSE on, HSI on
  RCC->PLLCFGR =0x00400000;                                     //PLLSRC = HSE= 8Mhz
  RCC->PLLCFGR |=0x00000004;                                    //PLLM = 4
  RCC->PLLCFGR |=0x00002A00;                                    //PLLN = 168
  RCC->PLLCFGR |=0x07000000;                                    //PLLQ = 7  for USB OTG FS clock, USB OTG requaire 48Mhz CLK. and it is VCOI/PLLQ = 336/7=48Mhz
  RCC->PLLCFGR |=0x00000000;                                    //PLLC = 2, System CLK = VCO / PLLP. so, system clock = 336/2 = 168Mhz
  
  RCC->CR= 0x01010001;                                              //PLL on, HSE on, HSI on
  while((RCC->CR & 0x02000000) ==0)                             //wait  untill PLLRDY =1
    
    

// (4) over drive       STM32F407 Don't have over drive mode     
    
// (5) set up CLK
    GPIOD-> ODR =0x00005000;
    RCC->CFGR = 0xC0000000;                                   //MCO2= PLL(168Mhz)
    RCC->CFGR |= 0x30000000;                                   //prescalear setting for MCO2 = PLL/4 = 42Mhz
    
    RCC->CFGR |= 0x00400000;                                   // MCO1 = HSE(8Mhz)
    
    RCC->CFGR |= 0x00000002;                                   //System clock = PLL(168Mhz)
    RCC->CFGR |= 0x00000000;                                   //AHB Clock = Systel clock, AHB prescaler =0. so, AHB = 168Mhz
    RCC->CFGR |= 0x0000B400;                                  //APB1,APB2 Clock = 42Mhz    (AHB/4 =168/4), so APB prescaler =4
                                                                             // Timer Clock = 84Mhz (as APB prescaler is 4, Timer clock is APB *2, refer to datasheet 216page) 
    
    while((RCC->CFGR & 0x0000000C) !=0x00000008);       //wait untill SYSCLK = PLL
    RCC->CR |=0x00080000;                                        //CSS on 
    
    //(6) I/O 보상 설정 
    GPIOD-> ODR =0x00006000;
    RCC->APB2ENR |= 0x00004000;                                 //주변장치 클럭 (SYSCFG=1)
    SYSCFG->CMPCR = 0x00000001;                               //enable compensation cell 
   
    GPIOD-> ODR =0x00009000;
}

#endif /*__HI_STM32F4_CLOCK_SETTING_C */
    


