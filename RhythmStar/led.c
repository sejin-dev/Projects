#include "led.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h> 
static unsigned int ledValue = 0; 
static int fd = 0; 
int ledOnOff (int ledNum, int OnOff) 
{ 
	int i=1; 
	i=i<<ledNum; 
	ledValue = ledValue& (~i);
	 if (OnOff !=0) ledValue |= i;
	  write (fd, &ledValue, 4);
	   }
	  
 int ledLibInit(void) 
{
	fd=open("/dev/periled",O_WRONLY);
	ledValue=0;	  
	}
	
int ledLibExit(void) 
{
	 ledValue = 0;
	 ledOnOff (0, 0); 
	 close(fd); 
	 
	 }
