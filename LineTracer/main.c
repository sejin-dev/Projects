
#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 16000000
#define sensor_in PORTA		//입력핀 = PA 0~7
#define sensor_out PORTC	//출력핀 = PC 0
#define out_on 0x01
#define out_off 0x00
#define sensor_in_on 0x00

#define speed1		OCR1BH = 0x0c;	OCR1BL = 0x62;	//최저속도	
#define speed2		OCR1BH = 0x0c;	OCR1BL = 0x70;	
#define speed3		OCR1BH = 0x0c;	OCR1BL = 0xFF;	
#define speed4		OCR1BH = 0x0d;	OCR1BL = 0xFF;	
#define move_stop  	OCR1BH = 0x00;	OCR1BL = 0x00;OCR1AH = 0x0b;OCR1AL = 0xb8; black_line=black_line+1 ;//정지
//=============================실전===========================
#define left_turn3  OCR1AH = 0x09;OCR1AL = 0x60; speed1;//2400 1.2ms  좌회전 3(FULL)
#define left_turn2  OCR1AH = 0x0a;OCR1AL = 0x28; speed2;//2600 1.3ms  좌회전 2(Meidium
#define left_turn1  OCR1AH = 0x0a;OCR1AL = 0xf0; speed3; //2800 1.4ms  좌회전 1(small)
#define straight   OCR1AH = 0x0b;	OCR1AL = 0xb8; speed4;//3000 1.5ms  직진
#define right_turn1 OCR1AH = 0x0c;OCR1AL = 0x80; speed3;//3200 1.6ms  우회전 1(small)
#define right_turn2 OCR1AH = 0x0d;OCR1AL = 0x48; speed2;//3400 1.7ms  우회전 2(Meidium)
#define right_turn3 OCR1AH = 0x0e;OCR1AL = 0x10; speed1;//3600 1.8ms  우회전 1(FULL)

//==============================TEST==========================
//#define left_turn3  OCR1AH = 0x09;OCR1AL = 0x60; speed1;//2400 1.2ms  좌회전 3(FULL)
//#define left_turn2  OCR1AH = 0x0a;OCR1AL = 0x28; speed1;//2600 1.3ms  좌회전 (Meidium)
//#define left_turn1  OCR1AH = 0x0a;OCR1AL = 0xf0; speed1;  //2800 1.4ms  좌회전1(small)
//#define straight    OCR1AH = 0x0b;OCR1AL = 0xb8; speed1	//3000 1.5ms  직진
//#define right_turn1 OCR1AH = 0x0c;OCR1AL = 0x80; speed1;	//3200 1.6ms  우회전 (small)
//#define right_turn2 OCR1AH = 0x0d;OCR1AL = 0x48; speed1;//3400 1.7ms  우회전 Meidium)
//#define right_turn3 OCR1AH = 0x0e;OCR1AL = 0x10; speed1;//3600 1.8ms  우회전 1(FULL)

uint8_t speed,black_line;
long result ;

void sensor_ready(void)
{
	DDRA = 0x00;		//수광부 ready
	DDRC = 0x01;		//발광부 ready
}
///////////////////////////////////////////////////////////
//////////////////////  Lim Se jin ////////////////////////
///////////////////////////////////////////////////////////
////////////////////// PWM+ ADC + Sensor //////////////////
///////////////////////////////////////////////////////////

////////////////////////Start setting//////////////////////
void Start_setting(void)
{
	_delay_ms(5000);
	_delay_ms(5000);
	for(speed=0x0b;speed<0x0d;speed++)
		{	
			OCR1AH = 0x0b;OCR1AL = 0xb8; _delay_ms(1000);//3000 1.5ms  직진
			OCR1BH = speed;OCR1BL = 0xb8; _delay_ms(1000);					
		}
}
/////////////////////////PWM////////////////////////////////////
void Init_PWM(void)
{	
	// Pre-sdcaler 8
	
	DDRB = 0x60;
	TCCR1A |= (1<<COM1A1)| (1<<COM1A1);	//OC1A on
	TCCR1A |= (1<<COM1B1)| (1<<COM1B1);	//OC1B on
	//TCCR1A |= (1<<COM1C1)| (1<<COM1C1);	//OC1C on
	

	//pre-scaler 8
	TCCR1B |=0x02;

	TCCR1A |= (1<<WGM11) | (0<<WGM10);
	TCCR1B |= (1<<WGM13) | (1<<WGM12);

	////TOP 20ms =40000
	ICR1H = 0x9c;	ICR1L = 0x40;	
	
	//출력값 설정. 0.1ms당 200. 최대 40000;
	//OCR1AH = 0x08;	OCR1AL = 0x98;	//2200 1.1ms
	OCR1AH = 0x0b;	OCR1AL = 0xb8;		
	OCR1BH = 0x0b;	OCR1BL = 0xb8;		
	//OCR1AH = 0x05;	OCR1AL = 0xdc;	_delay_ms(2000);//1500 0.75ms
	//OCR1AH = 0x07;	OCR1AL = 0x6c;	_delay_ms(2000);//1900 0.95ms
}
//////////////////////////////ADC///////////////////////////
void ADC_setting(void)
{
	
	DDRF = 0x00; PORTF = 0x00;

	ADMUX = 0x00;
	ADCSRA = 0xE6;		
}

///////////////////////Buzzer /////////////////////////////
void Buzzer(void)
{
	DDRB |= (1<<2);
}

//////////////////////////////Main////////////////////////////////
int main(void)
{
	uint8_t val,way_val;
	sensor_ready();
	ADC_setting();
	Init_PWM();
	
Start_setting();
	Buzzer();

	while(black_line <1)
	{
		result = ADC;	//ADC Read
		sensor_out |= out_on;	
		_delay_ms(1);	//발광부 작동대기시간.
		way_val = PINA & 0xff;		
		//sensor_out |= out_off;	
		//-----------------------------------
		if (way_val != 0b11111111){val = way_val;}
		switch(val)
		{
	///////////우측센서시감지 ////////////
	case 0b11101111: right_turn1 ;  break;
	case 0b11001111: right_turn2 ;  break;
	case 0b11011111: right_turn2 ;  break;
	case 0b10101111: right_turn3 ;  break;			
	case 0b10011111: right_turn3 ;  break;			
	case 0b10001111: right_turn3 ;  break;
	case 0b01101111: right_turn3 ;  break;
	case 0b01011111: right_turn3 ;  break;
	case 0b01001111: right_turn3 ;  break;
	case 0b00111111: right_turn3 ;  break;
	case 0b00101111: right_turn3 ;  break;
	case 0b00011111: right_turn3 ;  break;
	case 0b00001111: right_turn3 ;  break;
	case 0b10111111: right_turn3 ;  break;
	case 0b01111111: right_turn3 ;  break;			

	///////////좌측센서 동시감지 ////////////
	case 0b11110111: left_turn1 ; break;
	case 0b11111011: left_turn2 ;  break;
	case 0b11110011: left_turn2 ;  break;
	case 0b11110101: left_turn3 ;  break;
	case 0b11111001: left_turn3 ;  break;
	case 0b11110001: left_turn3 ;  break;
	case 0b11111110: left_turn3 ;  break;
	case 0b11110110: left_turn3 ;  break;
	case 0b11111010: left_turn3 ;  break;
	case 0b11111101: left_turn3 ;  break;
	case 0b11110010: left_turn3 ;  break;
	case 0b11111100: left_turn3 ;  break;
	case 0b11110100: left_turn3 ;  break;
	case 0b11111000: left_turn3 ;  break;
	case 0b11110000: left_turn3 ;  break;
	case 0x11111110: left_turn3 ;  break;
	case 0x11111101: left_turn3 ;  break;
						    
	/////////////  정지  //////////////
	


























































	case 0b00000000: move_stop ; break;
	case 0b00000010: move_stop ; break;
	case 0b00000100: move_stop ; break;
	case 0b00000110: move_stop ; break;
	case 0b00001000: move_stop ; break;
	case 0b00001010: move_stop ; break;
	case 0b00001100: move_stop ; break;
	case 0b00001110: move_stop ; break;
	case 0b00010000: move_stop ; break;
	case 0b00010010: move_stop ; break;
	case 0b00010100: move_stop ; break;
	case 0b00010110: move_stop ; break;
	case 0b00011000: move_stop ; break;
	case 0b00011010: move_stop ; break;
	case 0b00011100: move_stop ; break;
	case 0b00011110: move_stop ; break;
	case 0b00100000: move_stop ; break;
	case 0b00100010: move_stop ; break;
	case 0b00100100: move_stop ; break;
	case 0b00100110: move_stop ; break;
	case 0b00101000: move_stop ; break;
	case 0b00101010: move_stop ; break;
	case 0b00101100: move_stop ; break;
	case 0b00101110: move_stop ; break;
	case 0b00110000: move_stop ; break;
	case 0b00110010: move_stop ; break;
	case 0b00110100: move_stop ; break;
	case 0b00110110: move_stop ; break;
	case 0b00111000: move_stop ; break;
	case 0b00111010: move_stop ; break;
	case 0b00111100: move_stop ; break;
	case 0b00111110: move_stop ; break;
	case 0b01000000: move_stop ; break;
	case 0b01000010: move_stop ; break;
	case 0b01000100: move_stop ; break;
	case 0b01000110: move_stop ; break;
	case 0b01001000: move_stop ; break;
	case 0b01001010: move_stop ; break;
	case 0b01001100: move_stop ; break;
	case 0b01001110: move_stop ; break;
	case 0b01010000: move_stop ; break;
	case 0b01010010: move_stop ; break;
	case 0b01010100: move_stop ; break;
	case 0b01010110: move_stop ; break;
	case 0b01011000: move_stop ; break;
	case 0b01011010: move_stop ; break;
	case 0b01011100: move_stop ; break;
	case 0b01011110: move_stop ; break;
	case 0b01100000: move_stop ; break;
	case 0b01100010: move_stop ; break;
	case 0b01100100: move_stop ; break;
	case 0b01100110: move_stop ; break;
	case 0b01101000: move_stop ; break;
	case 0b01101010: move_stop ; break;
	case 0b01101100: move_stop ; break;			case 0b01101110: move_stop ; break;
	case 0b01110010: move_stop ; break;
	case 0b01110100: move_stop ; break;
	case 0b01110110: move_stop ; break;
	case 0b01111000: move_stop ; break;
	case 0b01111010: move_stop ; break;






	case 0b01111100: move_stop ; break;
	case 0b01111110: move_stop ; break;
	  
	/////////////  직진   //////////////
	default: straight;  break;

	}
	if(ADC>120){OCR1BH = 0x00;OCR1BL = 0x00;_delay_ms(2000);	
	PORTB |=  (1<<2) ;	_delay_ms(3000); 
	PORTB &= ~(1<<2) ;  _delay_ms(1000);
	PORTB |=  (1<<2) ;	_delay_ms(3000); 
	PORTB &= ~(1<<2) ;  _delay_ms(1000);
	_delay_ms(10000);	}

	else if(ADC >100){speed1;_delay_ms(1000);}	//60cm
	else if(ADC >60) {speed2;_delay_ms(1000);}	//80cm
	_delay_ms(1000);
	}
	return 0;
}
