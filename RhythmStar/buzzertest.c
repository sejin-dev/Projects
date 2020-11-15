#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "buzzer.h"


const int musicScale[MAX_SCALE_STEP] =
{
	262, 
	294,
	330,
	349,
	392,
	440,
	494,  
	523
};

void doHelp(void)
{
	printf("Usage:\n");
	printf("buzzertest <buzzerNo> \n");
	printf("buzzerNo: \n");
	printf("do(1),re(2),mi(3),fa(4),sol(5),ra(6),si(7),do(8) \n");
	printf("off(0)\n");
}


int main(int argc , char **argv)
{
	 int freIndex; 
	 if (argc < 2 || findBuzzerSysPath() )  {
		  printf("Error!\n"); 
		  doHelp(); 
		  return 1; 
		  } 
		  
		  freIndex = atoi(argv[1]);
		  printf("freIndex :%d \n",freIndex);
		  
		  if ( freIndex > MAX_SCALE_STEP ) { 
			  printf(" <buzzerNo> over range \n"); 
			  doHelp(); 
			  return 1; 
			  }
			  
			  
		if ( freIndex == 0){ 
			buzzerEnable(0);
			 } 
		 else if(freIndex!=0) { 
				 setFrequency(musicScale[freIndex-1]);
				 buzzerEnable(1);
				  } 
				  return 0;
}
