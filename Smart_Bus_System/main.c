
#include "stm32f767xx.h"
#include "OK-STM767.h"
#include "OK-STM767_large.h"
#include "OK-STM767_xlarge.h"

#define KEY1		0x0E			// KEY1 value
#define KEY2		0x0D			// KEY2 value
#define KEY3		0x0B			// KEY3 value
#define KEY4		0x07			// KEY4 value
void LED_on();
int outcount = 0;
int syscount = 0;

unsigned char RX3_char(void);			/* receive a character by USART3 */
void TX3_string(U08 *string);
void TX3_char(U08 data);
void Beep(void);
void SysTick_Handler(void);			/* SysTick interrupt function */

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function */
{
  outcount++;
  if(outcount == 15){
	TFT_string(2, 2, White, Black, "                                    ");
	TFT_string(2, 4, White, Black, "                                    ");
	TFT_string(2, 6, White, Black, "                                    ");
  }
  else if(outcount == 60){
    outcount = 0;
	if (syscount == 3) {
	  syscount = 0;
	}
	else {
		if (syscount == 0) {
			TFT_string(4, 2, White, Magenta, "  달리는 차량에서 서있는 것은 ");
			TFT_string(4, 4, Red, Magenta, "       매우 위험하오니        ");
			TFT_string(4, 6, White, Magenta, "정차 후에 일어나시길 바랍니다.");
		}
		else if (syscount == 1) {
			TFT_string(4, 2, White, Magenta, " 노약자석 누구나 앉을수 있습니다 ");
			TFT_string(4, 4, Red, Magenta, "        하지만 기억하세요        ");
			TFT_string(4, 6, White, Magenta, "도움이 필요한 사람에게 양보하세요");
		}
		else if (syscount == 2) {
			TFT_string(8, 2, White, Magenta, "      하차하실 때     ");
			TFT_string(8, 4, Red, Magenta, "       카드를 꼭      ");
			TFT_string(8, 6, White, Magenta, "찍고 내리시길 바랍니다");
		}
		else {
		}
		syscount++;
	}
  }
}
/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
	unsigned char RXD, count;
	unsigned short color;
	unsigned int PWM;// pulse width
	int temp;
	int stop_count = 0;
	Initialize_MCU();				// initialize MCU and kit
	Delay_ms(50);					// wait for system stabilization
	Initialize_LCD();				// initialize text LCD module
	Initialize_TFT_LCD();				// initialize TFT-LCD module

	Beep();


	/////////////////////////////////////////////////////////////////////// LED Setting - PA4
   // RCC -> AHB1ENR |= 0x00000001; 
   // GPIOA -> MODER &= 0xFFFFFCF;
   //GPIOA -> MODER |= 0x00000100;
   // GPIOA -> ODR &= 0xFFFF9FFEF;
	////////////////////////////////////////////////////////////////////// LED Setting -PC9 

  //  RCC -> AHB1ENR |= 0x00000004;
	//-GPIOC -> MODER &= 0x
   ////////////////////////////////////////////////////////////////////// LED Setting -PA 8
	RCC->AHB1ENR |= 0x00000001;
	GPIOA->MODER |= 0x00015500;

	/////////////////////////////////////////////////////////////////////////
	LCD_string(0x80, " MJU_shuttle_1 ");		// display title
	LCD_string(0xC0, " To Eng_Hall_3");
	////////////////////////////////////////////////////////////////////////
	TFT_string(2, 4, White, Magenta, " 자율 주행 버스 운행을 시작합니다.");
	///////////////////////////////////////////////////////
	GPIOB->MODER &= 0xFF0FFFFF;			// PB10 = USART3_TX, PB11 = USART3_RX
	GPIOB->MODER |= 0x00A00000;
	GPIOB->AFR[1] &= 0xFFFF00FF;
	GPIOB->AFR[1] |= 0x00007700;
	RCC->APB1ENR |= 0x00040000;			// enable USART3 clock

	USART3->CR1 = 0x0000000D;			// TE=RE=UE = 1, 8 data bit, oversampling by 16
	USART3->CR2 = 0x00000000;			// asynchronous mode, 1 stop bit
	USART3->CR3 = 0x00000000;			// 3 sampling bit
	USART3->BRR = 2813;				// 19200 bps = 54MHz/2812.5
	Delay_ms(1);
	RXD = USART3->RDR;				// dummy read

	count = 0;					// initialize received data display
	color = Cyan;
	TFT_xy(0, 20);
	TFT_color(color, Black);
	///////////////////////////////////////////////////////////////

	SysTick->LOAD = 26999999;			// 27MHz/(26999999+1) = 1 Hz
	SysTick->VAL = 0;				// clear SysTick Counter
	SysTick->CTRL = 0x00000003;			// 216MHz/8 = 27MHz, enable SysTick and interrupt

  ///////////////////////////////////////////////////////////////
	int result = 0;
	Rectangle(30, 105, 289, 171, Cyan);
	Rectangle(31, 106, 288, 170, Cyan);

	GPIOA->MODER |= 0x000000C0;
	RCC->APB2ENR |= 0X00000100;

	ADC->CCR = 0x00;
	ADC1->SMPR2 = 0x00000200;
	ADC1->CR1 = 0X00000000;
	ADC1->CR2 = 0X00000001;
	ADC1->SQR1 = 0X00000000;
	ADC1->SQR3 = 0X00000003;
	////////////////////////////////////////////////////////////////////////////

	PWM = 20; //Duty 2%

	GPIOE->MODER &= 0xCFFFFFFF;// PE14 = alternate function mode
	GPIOE->MODER |= 0x20000000;
	GPIOE->AFR[1] &= 0xF0FFFFFF;// PE14 = TIM1_CH4
	GPIOE->AFR[1] |= 0x01000000;
	RCC->APB2ENR |= 0x00000001;// enable TIM1 clock
	//TIM1->PSC = 107;// 108MHz/(107+1) = 1MHz
	//TIM1->ARR = 999;// 1MHz / (999+1) = 1kHz
	TIM1->PSC = 2059; // 108MHz//2059+1 = 50khz
	TIM1->ARR = 999;   // 50khz/999+1 =  50hz
	TIM1->CCR4 = PWM;
	TIM1->CNT = 0;// clear counter
	TIM1->CCMR2 = 0x00006C00;// OC4M = 0110(PWM mode 1), CC4S = 00(output)
	TIM1->CCER = 0x00001000;// CC4E = 1(enable OC4 output)
	TIM1->BDTR = 0x00008000;// MOE = 1
	TIM1->CR1 = 0x0005;// edge-aligned, up-counter, enable TIM1


	////////////////////////////////////////////////////////////////////////////////////////////////


	while (1)
	{
		switch (Key_input_silent())                       // key input
		{
		case KEY1: TX3_string("press the stop button in the bus");
			TX3_char(0x0D); TX3_char(0x0A);
			TFT_string(6, 14, White, Blue, " 다음 정거장에 정차합니다. ");                               
			LED_on();
			Beep();
			break;
		case KEY2: TX3_string("press the stop button outside the bus");
			TX3_char(0x0D); TX3_char(0x0A);
			TFT_string(6, 14, White, Blue, " 다음 정거장에 정차합니다. ");
			LED_on();
			Beep();
			break;
		case KEY3: stop_count++;
			//GPIOA -> ODR ^=0x00000100;  
		   // GPIOC -> ODR ^=0x00000200;

			switch (stop_count)
			{
			case 1:TFT_string(2, 4, White, Magenta, "this stop is E-mart                ");
				TX3_string("bus1 is at E-mart");  TX3_char(0x0D); TX3_char(0x0A);
				break;
			case 2:TFT_string(2, 4, White, Magenta, "this stop is MJU Intersection       ");
				TX3_string("bus1 is at MJU Intersection   ");  TX3_char(0x0D); TX3_char(0x0A);
				break;
			case 3:TFT_string(2, 4, White, Magenta, "this stop is MJU subway station     ");
				TX3_string("bus1 is at MJU subway station   ");  TX3_char(0x0D); TX3_char(0x0A);
				break;
			case 4:TFT_string(2, 4, White, Magenta, "this stop is E-mart                ");
				TX3_string("bus1 is at E-mart ");  TX3_char(0x0D); TX3_char(0x0A);
				break;
			case 5:TFT_string(2, 4, White, Magenta, "this stop is MJU main gate         ");
				TX3_string("bus1 is at MJU main gate ");  TX3_char(0x0D); TX3_char(0x0A);
				break;
			case 6:TFT_string(2, 4, White, Magenta, "this stop is 1rd Engneering_hall   ");
				TX3_string("bus1 is at 1rd Engneering_hall  ");  TX3_char(0x0D); TX3_char(0x0A);
				break;
			case 7:TFT_string(2, 4, White, Magenta, "this stop is Architecture_hall   ");
				TX3_string("bus1 is at Architecture_hall");  TX3_char(0x0D); TX3_char(0x0A);
				break;
			case 8:TFT_string(2, 4, White, Magenta, "this stop is 3rd Engneering_hall   ");
				TX3_string("bus1 is at 3rd Engneering_hall  ");  TX3_char(0x0D); TX3_char(0x0A);
				break;
			case 9:TFT_string(2, 4, White, Magenta, "this stop is Architecture_hall   ");
				TX3_string("bus1 is at Architecture_hall  ");  TX3_char(0x0D); TX3_char(0x0A);
				break;

			case 10:TFT_string(2, 4, White, Magenta, "this stop is MJU main gate         ");
				TX3_string("bus1 is at MJU main gate ");  TX3_char(0x0D); TX3_char(0x0A);
				break;
			}
			if (stop_count == 10) stop_count = 0;
			TFT_string(6, 14, White, Blue, "잠시후 정류장에 도착합니다.");
			TFT_string(6, 18, White, Blue, "현재 버스 내부 온도 :   ");
					

			//////////////////////////////////////////////////////////////////////
			result = 0;
			for (int i = 0; i < 256; i++)
			{
				ADC1->CR2 |= 0x40000000;
				while (!(ADC1->SR & 0x00000002));
				result += ADC1->DR;
				Delay_ms(1);
			}
			result >>= 8;
			TFT_unsigned_float((float)result*330. / 4095., 2, 1);
			temp = ((result * 330) / 4095);


			if (30 <= temp) {
				TFT_string(2, 8, White, Blue, "temp of Bus 1 is over 30degree. ");
				GPIOA->BSRR |= 0x00000100;
				TX3_string(" Air conditioner On ");  TX3_char(0x0D); TX3_char(0x0A);
			}



			else if (30 > temp) {
				TFT_string(2, 8, White, Blue, "temp of Bus 1 is under 30degree. ");
				GPIOA->BSRR |= 0x01000000;
				TX3_string(" Air conditioner Off ");  TX3_char(0x0D); TX3_char(0x0A);
			}

			else {
				TFT_string(2, 8, White, Blue, "error ");
			}


			break;
			/////////////////////////////////////////////////////
		case KEY4: TX3_string("Bus_1 has stopped at the bus stop.");
			TX3_char(0x0D); TX3_char(0x0A);
                        TFT_string(2, 4, White, Black, "                                    ");
			TFT_string(6, 14, White, Blue, "      버스가 정차합니다.   ");
			


			TIM1->CCR4 = 124;
			Delay_ms(4000);
			TIM1->CCR4 = 20;
			Delay_ms(4000);
			GPIOC->BSRR = 0x00100000;
			break;
		default:    break;
		}

		RXD = RX3_char();				// receive data if any
		if ((RXD >= 0x20) && (RXD <= 0x7F))
		{
			TFT_English(RXD);			// display a character
			count++;				// 120 character OK ?
			if (count >= 120)			// if yes, go home
			{
				count = 0;
				TFT_xy(0, 20);
				color ^= 0xFFFF;
				TFT_color(color, Black);
			}
		}
	}
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

unsigned char RX3_char(void)			/* receive a character by USART3 */
{
	if (USART3->ISR & 0x00000020)   		// if RXNE=1, return with a received character
		return USART3->RDR;
	else						// if RXNE=0, return with 0x00
		return 0x00;
}

void TX3_char(U08 data)				/* transmit a character by USART3 */
{
	while (!(USART3->ISR & 0x00000080));		// wait until TXE = 1
	USART3->TDR = data;
}

void TX3_string(U08 *string)			/* transmit a string by USART3 */
{
	while (*string != '\0')
	{
		TX3_char(*string);
		string++;
	}
}
