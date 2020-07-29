#include <avr/io.h>
#define F_CPU 16000000 //16MHz
#include <util/delay.h>


//ADC결과를 저장하는 16비트의 변수 선언
unsigned int ADC_Temp = 0;			//저장함수
unsigned int ADC_average = 0;

int i,j;

//AD변환을 위한 함수 원형 선언.
void ADC_Read(unsigned char);   //1번의 AD변환을 함

//10번의 AD변환을 하고 , 평균값을 반환함.

unsigned int ADC_10Read(unsigned char);

//주 실행함수

int main(void)
{
//DDRA = 0xff;	//LED Test
//DDRC = 0xff;	//LED Test

DDRB = 0x80;	//모터
DDRF = 0x00;	//sensor


//REFS1~0 :AREF단자 전압을 이용, MUX4~0 : ADC3단극성 입력
ADMUX = 0x03;

//ADEN :ADC 허용,인터럽트 활성화, ADPS2~0:분주비 CLK/2
ADCSRA = 0x81;


	while(1)
	{
	
	ADC_average = ADC_10Read(1);	//CDS와 연결된 ADC의 3번 채널에서
											//ADC를 10번 수행하여 평균값을 구함.
	
	  
	  	if (ADC_average > 600)  {for(i= 0 ;i<50;i++) {PORTB = 0x80;	_delay_us(600);		PORTB = 0x00;	_delay_ms(20); } }
	
	else if(ADC_average > 0 )	{for(i= 0 ;i<50;i++){ PORTB = 0x80;	_delay_us(1500);	PORTB = 0x00;	_delay_ms(20); } }



	}

	//함수의 형태와 같이 정수형(int)의 값을 반환한다
	return 1;
}

//AD변환을 1번 수행한다
void ADC_Read(unsigned char chn)
{
	ADMUX = chn;				 //AD채널을 선택

	ADCSR |= (1<<ADSC);			 //AD변환 시작

	//ADIF 레지스터가 SET될때까지
	while((ADCSRA& 0X10)== 0);

	//ADC변환 결과를 ADCL,ADCH레지스터로부터 읽어낸후,
	//ADC_Temp 변수에 저장함.

	ADC_Temp = (int)ADCL + ((int)ADCH <<8);
}

	//AD변환을 10번 수행함.
unsigned int ADC_10Read(unsigned char chn){
	int cnt = 0;				//AD 변환을 10번 수행하기 위한 변수 선언.
	unsigned int average = 0;	//AD 변환 결과의 평균값을 저장하기 위한 변수

		for(cnt= 0; cnt <10; cnt++){
		ADC_Read(chn);
		average += ADC_Temp;			//변환 결과를 더하여 저장함.
		}


average /= 10;					//저장한 결과값을 10으로 나누어
						//평균갑을 average 변수에 저장

return average;
}

