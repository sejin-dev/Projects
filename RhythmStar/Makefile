all: libMyPeri.a buzzertest.elf ledtest.elf fndtest.elf colorledtest.elf textlcdtest.elf buttontest.elf temperaturetest.elf bitmap1.elf
   

buzzertest.elf: libMyPeri.a buzzer.h buzzertest.c
	arm-linux-gnueabi-gcc -o buzzertest2.elf buzzertest.c -L. -lMyPeri


ledtest.elf: libMyPeri.a ledtest.c led.h
	arm-linux-gnueabi-gcc ledtest.c -L. -lMyPeri -o ledtest.elf 


fndtest.elf: libMyPeri.a fnd.h fndtest.c
	arm-linux-gnueabi-gcc -o fndtest.elf fndtest.c -L. -lMyPeri



colorledtest.elf: libMyPeri.a colorLed.h colorledtest.c
	arm-linux-gnueabi-gcc -o colorledtest.elf colorledtest.c -L. -lMyPeri


buttontest.elf: libMyPeri.a button.h buttontest.c
	arm-linux-gnueabi-gcc -o buttontest.elf buttontest.c -L. -lMyPeri -lpthread
	
	
textlcdtest.elf: libMyPeri.a textlcd.h textlcdtest.c 
	arm-linux-gnueabi-gcc -o textlcdtest.elf textlcdtest.c -L. -lMyPeri


temperaturetest.elf: libMyPeri.a temperature.h temperaturetest.c
	arm-linux-gnueabi-gcc -o temperaturetest.elf temperaturetest.c -L. -lMyPeri
	
bitmap1.elf: libMyPeri.a bitmap.c bitmap.h
	arm-linux-gnueabi-gcc bitmap.c -o bitmap1.elf -L. -lMyPeri
   
libMyPeri.a: buzzer.h buzzer.c fnd.h fnd.c led.h led.c colorLed.h colorLed.c button.h button.c textlcd.h textlcd.c temperature.h temperature.c bitmap.h bitmap.c
	arm-linux-gnueabi-gcc -c buzzer.c -o buzzer.o 
	arm-linux-gnueabi-gcc -c led.c -o led.o
	arm-linux-gnueabi-gcc -c fnd.c -o fnd.o
	arm-linux-gnueabi-gcc -c colorLed.c -o colorLed.o
	arm-linux-gnueabi-gcc -c button.c -o button.o
	arm-linux-gnueabi-gcc -c textlcd.c -o textlcd.o 
	arm-linux-gnueabi-gcc -c temperature.c -o temperature.o 
	arm-linux-gnueabi-gcc -c bitmap.c -o bitmap.o
	arm-linux-gnueabi-ar rc libMyPeri.a buzzer.o led.o fnd.o colorLed.o button.o textlcd.o temperature.o bitmap.o

clean:
	rm -rf *.o *.a *.elf

